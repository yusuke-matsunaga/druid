
/// @file ConflictChecker.cc
/// @brief ConflictChecker の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "ConflictChecker.h"
#include "FaultInfoMgr.h"
#include "Fsim.h"
#include "TestVector.h"
#include "TpgNode.h"
#include "FFRFaultList.h"
#include "LocalImp.h"
#include "StructEngine.h"
#include "BoolDiffEnc.h"
#include "ym/Timer.h"
#include <random>


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
ConflictChecker::ConflictChecker(
  const FaultInfoMgr& mgr
) : mMgr{mgr}
{
  mFaultList = mMgr.active_fault_list();
  mFaultNum = mFaultList.size();
}

// @brief 衝突している故障を求める．
void
ConflictChecker::run(
  const JsonValue& option
)
{
  Timer timer;
  timer.start();

  bool localimp = false;
  if ( option.is_object() && option.has_key("localimp") ) {
    localimp = option.get("localimp").get_bool();
  }
  bool globalimp = false;
  if ( option.is_object() && option.has_key("globalimp") ) {
    globalimp = option.get("globalimp").get_bool();
  }

  check_compatible();

  if ( !globalimp ) {
    check_trivial(localimp);
  }

  check_ffr();

  if ( globalimp ) {
    check_mandatory_condition();
  }

  check_final();

  timer.stop();

  cout << "Conflict Check:    " << endl
       << "Conflict Pair:     " << mConflictPair.size() << endl
       << "CPU time:          " << timer.get_time() << endl;
}

// @brief ネットワークを返す．
const TpgNetwork&
ConflictChecker::network() const
{
  return mMgr.network();
}

// @brief シミュレーションを行い両立故障を求める．
void
ConflictChecker::check_compatible()
{
  Timer timer;
  timer.start();

  std::mt19937 randgen;
  vector<TestVector> tv_list;
  tv_list.reserve(mFaultNum);
  for ( auto fault: mFaultList ) {
    auto& finfo = mMgr.fault_info(fault);
    TestVector tv{network(), finfo.pi_assign()};
    tv.fix_x_from_random(randgen);
    tv_list.push_back(tv);
  }

  Fsim fsim{network(), mFaultList, false, false};
  vector<TestVector> tv_buff;
  tv_buff.reserve(Fsim::PP_BITLEN);
  mCompatPair.clear();
  SizeType np = 0;
  vector<pair<const TpgFault*, PackedVal>> f_list;
  f_list.reserve(mFaultNum);
  for ( auto tv: tv_list ) {
    tv_buff.push_back(tv);
    if ( tv_buff.size() == Fsim::PP_BITLEN ||
	 np + tv_buff.size() == tv_list.size() ) {
      f_list.clear();
      fsim.ppsfp(tv_buff,
		 [&](const TpgFault* fault,
		     const DiffBitsArray& dbits_array){
		   auto bits = dbits_array.dbits_union();
		   f_list.push_back({fault, bits});
		 });
      SizeType n = f_list.size();
      if ( n == 0 ) {
	continue;
      }
      for ( SizeType i1 = 0; i1 < n - 1; ++ i1 ) {
	auto& p1 = f_list[i1];
	auto fault1 = p1.first;
	auto b1 = p1.second;
	for ( SizeType i2 = i1 + 1; i2 < n; ++ i2 ) {
	  auto& p2 = f_list[i2];
	  auto fault2 = p2.first;
	  auto b2 = p2.second;
	  if ( (b1 & b2) != PV_ALL0 ) {
	    SizeType key = gen_key(fault1, fault2);
	    if ( mCompatPair.count(key) == 0 ) {
	      mCompatPair.emplace(key);
	    }
	  }
	}
      }
      tv_buff.clear();
      np += Fsim::PP_BITLEN;
    }
  }
  timer.stop();
  cout << "compatible pairs: " << mCompatPair.size() << endl;
  cout << "CPU time:         " << timer.get_time() << endl;
}

// @brief 割り当てが衝突しているペアを求める．
void
ConflictChecker::check_trivial(
  bool localimp
)
{
  Timer timer;
  timer.start();

  // mFaultList 内で mandatory_condition が衝突している
  // ペアを求める．
  mConflictPair.clear();
  LocalImp imp{network()};
  vector<AssignList> cond_array(mFaultNum);
  for ( SizeType i = 0; i < mFaultNum; ++ i ) {
    auto fault = mFaultList[i];
    auto& cond = mMgr.fault_info(fault).mandatory_condition();
    if ( localimp ) {
      cond_array[i] = imp.run(cond);
    }
    else {
      cond_array[i] = cond;
    }
  }
#if 1
  for ( SizeType i1 = 0; i1 < mFaultNum - 1; ++ i1 ) {
    auto fault1 = mFaultList[i1];
    auto& cond1 = cond_array[i1];
    for ( SizeType i2 = i1 + 1; i2 < mFaultNum; ++ i2 ) {
      auto fault2 = mFaultList[i2];
      auto& cond2 = cond_array[i2];
      SizeType key = gen_key(fault1, fault2);
      if ( mCompatPair.count(key) > 0 ) {
	continue;
      }
      if ( compare(cond1, cond2) == -1 ) {
	mConflictPair.emplace(key);
      }
    }
  }
#else
  vector<vector<const TpgFault*>> f_list_array(network().node_num() * 4);
  for ( SizeType id = 0; id < mFaultNum; ++ id ) {
    auto fault = mFaultList[id];
    auto& assign = cond_array[id];
    for ( auto nv: assign ) {
      auto node = nv.node();
      auto time = nv.time();
      auto v = nv.val();
      SizeType offset = v ? 1 : 0;
      SizeType index = node->id() * 4 + time * 2 + offset;
      f_list_array[index].push_back(fault);
    }
  }
  for ( SizeType node_id = 0; node_id < network().node_num(); ++ node_id ) {
    for ( int time = 0; time < 2; ++ time ) {
      SizeType index0 = node_id * 4 + time * 2 + 0;
      SizeType index1 = node_id * 4 + time * 2 + 1;
      auto& f0_list = f_list_array[index0];
      auto& f1_list = f_list_array[index1];
      if ( f0_list.empty() || f1_list.empty() ) {
	continue;
      }
      for ( auto fault1: f0_list ) {
	for ( auto fault2: f1_list ) {
	  SizeType key = gen_key(fault1, fault2);
	  if ( mConflictPair.count(key) == 0 ) {
	    mConflictPair.emplace(key);
	  }
	}
      }
    }
  }
#endif
  timer.stop();
  cout << "trivial conflict pairs: " << mConflictPair.size() << endl;
  cout << "CPU time:               " << timer.get_time() << endl;
}

// @brief FFR 内の衝突チェックを行う．
void
ConflictChecker::check_ffr()
{
  Timer timer;
  timer.start();

  FFRFaultList ffr_fault_list{network(), mFaultList};
  for ( auto ffr: ffr_fault_list.ffr_list() ) {
    auto bd_enc = new BoolDiffEnc(ffr->root());
    StructEngine::Builder builder;
    builder.add_subenc(bd_enc);
    builder.add_extra_prev_node(ffr->root());
    auto engine = builder.new_obj(network());
    auto& fault_list = ffr_fault_list.fault_list(ffr);
    SizeType n = fault_list.size();
    for ( SizeType i1 = 0; i1 < n - 1; ++ i1 ) {
      auto fault1 = fault_list[i1];
      auto f1_cond = fault1->ffr_propagate_condition();
      auto assumptions1 = engine->conv_to_literal_list(f1_cond);
      assumptions1.push_back(bd_enc->prop_var());
      for ( SizeType i2 = i1 + 1; i2 < n; ++ i2 ) {
	auto fault2 = fault_list[i2];
	SizeType key = gen_key(fault1, fault2);
	if ( mConflictPair.count(key) > 0 ||
	     mCompatPair.count(key) > 0 ) {
	  continue;
	}
	auto f2_cond = fault2->ffr_propagate_condition();
	auto assumptions2 = engine->conv_to_literal_list(f2_cond);
	assumptions2.insert(assumptions2.end(),
			    assumptions1.begin(),
			    assumptions1.end());
	if ( engine->solver().solve(assumptions2) == SatBool3::False ) {
	  // 同時には検出できない．
	  mConflictPair.emplace(key);
	}
      }
    }
  }

  timer.stop();
  cout << "after check_ffr()" << endl;
  cout << "conflict pairs:   " << mConflictPair.size() << endl;
  cout << "CPU time:         " << timer.get_time() << endl;
}

// @brief mandatory condition を使ったチェックを行う．
void
ConflictChecker::check_mandatory_condition()
{
  Timer timer;
  timer.start();

  SizeType check_num = 0;
  StructEngine::Builder builder;
  auto& node_list = network().node_list();
  builder.add_extra_node_list(node_list);
  builder.add_extra_prev_node_list(node_list);
  auto engine = builder.new_obj(network());
  for ( SizeType i1 = 0; i1 < mFaultNum - 1; ++ i1 ) {
    auto fault1 = mFaultList[i1];
    auto cond1 = mMgr.fault_info(fault1).mandatory_condition();
    auto assumptions1 = engine->conv_to_literal_list(cond1);
    for ( SizeType i2 = i1 + 1; i2 < mFaultNum; ++ i2 ) {
      auto fault2 = mFaultList[i2];
      if ( fault1->ffr_root() == fault2->ffr_root() ) {
	// 同じ FFR 内の故障はチェック済み
	continue;
      }
      auto key = gen_key(fault1, fault2);
      if ( mConflictPair.count(key) > 0 ||
	   mCompatPair.count(key) > 0 ) {
	continue;
      }
      auto cond2 = mMgr.fault_info(fault2).mandatory_condition();
      auto assumptions2 = engine->conv_to_literal_list(cond2);
      assumptions2.insert(assumptions2.end(),
			  assumptions1.begin(),
			  assumptions1.end());
      ++ check_num;
      if ( engine->solver().solve(assumptions2) == SatBool3::False ) {
	// 同時には検出できない．
	mConflictPair.emplace(key);
      }
    }
  }

  timer.stop();
  cout << "after check_mandatory_condition()" << endl;
  cout << "conflict pairs:   " << mConflictPair.size() << endl;
  cout << "CPU time:         " << timer.get_time() << endl;
}

// @brief 最終チェックを行う．
void
ConflictChecker::check_final()
{
  enum State {
    Init,
    Succeed,
    Failed
  };

  Timer timer;
  timer.start();

  FFRFaultList ffr_fault_list{network(), mFaultList};
  auto& ffr_list = ffr_fault_list.ffr_list();
  SizeType nffr = ffr_list.size();
  if ( nffr <= 1 ) {
    return;
  }
  SizeType check_num = 0;
  for ( SizeType i1 = 0; i1 < nffr - 1; ++ i1 ) {
    auto ffr1 = ffr_list[i1];
    if ( ffr_fault_list.fault_list(ffr1).empty() ) {
      continue;
    }
    for ( SizeType i2 = i1 + 1; i2 < nffr; ++ i2 ) {
      auto ffr2 = ffr_list[i2];
      if ( ffr_fault_list.fault_list(ffr2).empty() ) {
	continue;
      }

      auto bd_enc1 = new BoolDiffEnc(ffr1->root());
      auto bd_enc2 = new BoolDiffEnc(ffr2->root());
      StructEngine::Builder builder;
      builder.add_subenc(bd_enc1);
      builder.add_subenc(bd_enc2);
      builder.add_extra_prev_node(ffr1->root());
      builder.add_extra_prev_node(ffr2->root());
      auto engine = builder.new_obj(network());
      auto pvar1 = bd_enc1->prop_var();
      auto pvar2 = bd_enc2->prop_var();
      auto state = Init;
      for ( auto f1: ffr_fault_list.fault_list(ffr1) ) {
	auto cond1 = f1->ffr_propagate_condition();
	auto assumptions1 = engine->conv_to_literal_list(cond1);
	assumptions1.push_back(pvar1);
	assumptions1.push_back(pvar2);
	for ( auto f2: ffr_fault_list.fault_list(ffr2) ) {
	  SizeType key = gen_key(f1, f2);
	  if ( mConflictPair.count(key) > 0 ||
	       mCompatPair.count(key) ) {
	    continue;
	  }
	  if ( state == Failed ) {
	    // 検出できない
	    if ( mConflictPair.count(key) == 0 ) {
	      mConflictPair.emplace(key);
	    }
	    continue;
	  }
	  auto cond2 = f2->ffr_propagate_condition();
	  auto assumptions2 = engine->conv_to_literal_list(cond2);
	  assumptions2.insert(assumptions2.end(),
			      assumptions1.begin(),
			      assumptions1.end());
	  ++ check_num;
	  auto res = engine->solver().solve(assumptions2);
	  if ( res == SatBool3::False ) {
	    // 同時には検出できない．
	    if ( mConflictPair.count(key) == 0 ) {
	      mConflictPair.emplace(key);
	    }
	    if ( state == Init ) {
	      // FFRの出力のみの条件を調べる．
	      auto res = engine->solve({pvar1, pvar2});
	      if ( res == SatBool3::True ) {
		state = Succeed;
	      }
	      else {
		state = Failed;
	      }
	    }
	  }
	  else if ( res == SatBool3::True ) {
	    state = Succeed;
	  }
	}
      }
    }
  }

  timer.stop();
  cout << "after final_check()" << endl;
  cout << "conflict pairs:   " << mConflictPair.size() << endl
       << "# of checks:      " << check_num << endl;
  cout << "CPU time:         " << timer.get_time() << endl;
}

END_NAMESPACE_DRUID
