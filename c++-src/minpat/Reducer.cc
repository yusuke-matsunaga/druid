
/// @file Reducer.cc
/// @brief Reducer の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "Reducer.h"
#include "FaultInfo.h"
#include "TpgNetwork.h"
#include "TpgNodeSet.h"
#include "TpgFFR.h"
#include "TpgFault.h"
#include "TestVector.h"
#include "TestCover.h"
#include "BaseEnc.h"
#include "BoolDiffEnc.h"
#include "FaultAnalyzer.h"
#include "FFRDomChecker.h"
#include "DomCandGen.h"
#include "DomChecker.h"
#include "SimpleDomChecker.h"
#include "TrivialChecker1.h"
#include "TrivialChecker2.h"
#include "TrivialChecker3.h"
#include "ym/Timer.h"
#include <random>


BEGIN_NAMESPACE_DRUID

// @brief 支配関係を用いて故障を削減する．
vector<const TpgFault*>
Reducer::reduce(
  FaultInfoMgr& finfo_mgr,
  const JsonValue& option
)
{
  Reducer red{finfo_mgr};
  red.run(option);
  return finfo_mgr.active_fault_list();
}

// @brief コンストラクタ
Reducer::Reducer(
  FaultInfoMgr& finfo_mgr
) : mMgr{finfo_mgr},
    mFFRFaultList{finfo_mgr.network(), finfo_mgr.active_fault_list()},
    mInputListArray(finfo_mgr.network().ffr_num()),
    mDomCandListArray(finfo_mgr.network().max_fault_id()),
    mRevCandListArray(finfo_mgr.network().max_fault_id())
{
  mFaultNum = mFFRFaultList.fault_list().size();

  // FFR の構造を解析して関係ある入力ノードを求める．
  SizeType nn = network().node_num();
  for ( auto& ffr: network().ffr_list() ) {
    auto tmp_list = TpgNodeSet::get_tfo_list(nn, ffr->root());
    auto& input_list = mInputListArray[ffr->id()];
    TpgNodeSet::get_tfi_list(nn, tmp_list,
			     [&](const TpgNode* node) {
			       if ( node->is_ppi() ) {
				 input_list.push_back(node->id());
			       }
			     });
    sort(input_list.begin(), input_list.end());
  }
}

// @brief 故障削減する．
void
Reducer::run(
  const JsonValue& option
)
{
  Timer timer;
  timer.start();

  bool debug = get_debug(option);

  // 故障シミュレーションを用いて支配関係の候補リストを作る．
  SizeType limit = 1;
  if ( option.is_object() && option.has_key("loop_limit") ) {
    limit = option.get("loop_limit").get_int();
  }
  gen_dom_cands(limit, debug);

  // FFR内の支配関係を調べる．
  ffr_reduction(option);

  bool do_trivial_check = true;
  if ( option.is_object() && option.has_key("do_trivial_check") ) {
    do_trivial_check = option.get("do_trivial_check").get_bool();
  }

  if ( do_trivial_check ) {
    // 簡単なチェックを行う．
    trivial_reduction1(option);
    trivial_reduction2(option);
    trivial_reduction3(option);
  }
  // 最終チェックを行う．
  global_reduction(option, do_trivial_check);

  timer.stop();
  if ( debug ) {
    cout << "Total CPU time: " << timer.get_time() << endl;
  }
}

// @grep 故障シミュレーションを用いて被支配故障の候補を生成する．
void
Reducer::gen_dom_cands(
  SizeType limit,
  bool debug
)
{
  Timer timer;
  timer.start();

  if ( debug ) {
    cout << "---------------------------------------" << endl;
    cout << "Fault Simulation" << endl;
  }

  std::mt19937 randgen;
  vector<TestVector> tv_list;
  tv_list.reserve(mFFRFaultList.fault_list().size());
  for ( auto fault: mFFRFaultList.fault_list() ) {
    auto& finfo = fault_info(fault);
    if ( finfo.status() != FaultStatus::Detected ) {
      continue;
    }
    TestVector tv{network(), finfo.pi_assign()};
    tv.fix_x_from_random(randgen);
    tv_list.push_back(tv);
  }

  DomCandGen dc_gen{network(), mFFRFaultList.fault_list(), tv_list};
  dc_gen.run(limit, mDomCandListArray);

  // mDomCandListArray の逆向きのリストを作る．
  for ( auto fault1: mFFRFaultList.fault_list() ) {
    for ( auto fault2: dom_cand_list(fault1) ) {
      mRevCandListArray[fault2->id()].push_back(fault1);
    }
  }

  if ( debug ) {
    timer.stop();
    SizeType n = 0;
    for ( auto fault: mFFRFaultList.fault_list() ) {
      n += dom_cand_list(fault).size();
    }
    cout << "Total Candidates:                      " << n << endl;
    cout << "CPU time:                              " << timer.get_time() << endl;
  }
}

// @brief 同一FFR内の支配関係を用いて故障を削減する．
void
Reducer::ffr_reduction(
  const JsonValue& option
)
{
  Timer timer;
  timer.start();

  bool debug = get_debug(option);
  if ( debug ) {
    cout << "---------------------------------------" << endl;
    cout << "# of initial faults:                   "
	 << mFaultNum << endl;
  }

  SizeType check_num = 0;
  SizeType dom_num = 0;
  SizeType success_num = 0;

  for ( auto ffr: ffr_list() ) {
    FFRDomChecker checker{network(), ffr, option};
    ++ dom_num;

    // 支配関係を調べ，代表故障のみを残す．
    // FFR 単位の故障リスト
    for ( auto fault1: fault_list(ffr) ) {
      if ( fault_info(fault1).is_deleted() ) {
	continue;
      }
      auto fault1_root = fault1->ffr_root();
      vector<const TpgFault*> fault2_list;
      for ( auto fault2: dom_cand_list(fault1) ) {
	if ( fault2->ffr_root() == fault1_root &&
	     !fault_info(fault2).is_deleted() ) {
	  ++ check_num;
	  if ( checker.check(fault1, fault2) ) {
	    mMgr.delete_fault(fault2);
	    -- mFaultNum;
	    ++ success_num;
	  }
	}
      }
    }
  }

  if ( debug ) {
    timer.stop();
    cout << "after FFR dominance reduction:         " << mFaultNum << endl;
    cout << "    # of total checkes:                " << check_num << endl
	 << "    # of total successes:              " << success_num << endl
	 << "    # of FFRDomCheckers:               " << dom_num << endl
	 << "CPU time:                              " << timer.get_time() << endl;
  }
}

// @brief trivial な故障間の支配関係のチェックを行う．
void
Reducer::trivial_reduction1(
  const JsonValue& option
)
{
  Timer timer;
  timer.start();

  bool debug = get_debug(option);
  if ( debug ) {
    cout << "---------------------------------------" << endl;
  }

  vector<const TpgFault*> tmp_fault_list;
  vector<bool> mark(network().max_fault_id(), false);
  for ( auto fault1: mFFRFaultList.fault_list() ) {
    auto& finfo1 = fault_info(fault1);
    if ( finfo1.is_deleted() ) {
      continue;
    }
    if ( !finfo1.is_trivial() ) {
      continue;
    }
    if ( !mark[fault1->id()] ) {
      mark[fault1->id()] = true;
      tmp_fault_list.push_back(fault1);
    }
    for ( auto fault2: dom_cand_list(fault1) ) {
      auto& finfo2 = fault_info(fault2);
      if ( !finfo2.is_trivial() ) {
	continue;
      }
      if ( !mark[fault2->id()] ) {
	mark[fault2->id()] = true;
	tmp_fault_list.push_back(fault2);
      }
    }
  }

  TrivialChecker1 checker{network(), tmp_fault_list, option};

  SizeType check_num = 0;
  SizeType success_num = 0;
  for ( auto fault1: mFFRFaultList.fault_list() ) {
    auto& finfo1 = fault_info(fault1);
    if ( finfo1.is_deleted() ) {
      continue;
    }
    if ( !finfo1.is_trivial() ) {
      continue;
    }
    auto cond1 = finfo1.mandatory_condition();
    for ( auto fault2: dom_cand_list(fault1) ) {
      auto& finfo2 = fault_info(fault2);
      if ( finfo2.is_deleted() ) {
	continue;
      }
      if ( !finfo2.is_trivial() ) {
	continue;
      }
      if ( !check_intersect(fault1, fault2) ) {
	continue;
      }
      auto cond2 = finfo2.sufficient_condition();
      ++ check_num;
      if ( checker.check(cond1, cond2) ) {
	mMgr.delete_fault(fault2);
	-- mFaultNum;
	++ success_num;
      }
    }
  }

  if ( debug ) {
    timer.stop();
    cout << "after trivial_reduction1:              " << mFaultNum << endl;
    cout << "    # of total checkes:                " << check_num << endl
	 << "    # of total successes:              " << success_num << endl
	 << "CPU time:                              " << timer.get_time() << endl;
  }
}

// @brief trivial な故障が支配されている場合のチェックを行う．
void
Reducer::trivial_reduction2(
  const JsonValue& option
)
{
  Timer timer;
  timer.start();

  bool debug = get_debug(option);
  if ( debug ) {
    cout << "---------------------------------------" << endl;
  }

  SizeType check_num = 0;
  SizeType success_num = 0;
  for ( auto ffr1: ffr_list() ) {
    vector<const TpgFault*> fault2_list;
    vector<bool> f2_mark(network().max_fault_id(), false);
    for ( auto fault1: fault_list(ffr1) ) {
      auto& finfo1 = fault_info(fault1);
      if ( finfo1.is_deleted() ) {
	continue;
      }
      if ( finfo1.is_trivial() ) {
	continue;
      }
      for ( auto fault2: dom_cand_list(fault1) ) {
	auto& finfo2 = fault_info(fault2);
	if ( finfo2.is_deleted() ) {
	  continue;
	}
	if ( !finfo2.is_trivial() ) {
	  continue;
	}
	if ( !check_intersect(fault1, fault2) ) {
	  continue;
	}
	if ( !f2_mark[fault2->id()] ) {
	  f2_mark[fault2->id()] = true;
	  fault2_list.push_back(fault2);
	}
      }
    }
    if ( fault2_list.empty() ) {
      continue;
    }
    TrivialChecker2 checker{network(), ffr1, fault2_list, option};
    for ( auto fault1: fault_list(ffr1) ) {
      auto& finfo1 = fault_info(fault1);
      if ( finfo1.is_deleted() ) {
	continue;
      }
      if ( finfo1.is_trivial() ) {
	continue;
      }
      for ( auto fault2: dom_cand_list(fault1) ) {
	auto& finfo2 = fault_info(fault2);
	if ( finfo2.is_deleted() ) {
	  continue;
	}
	if ( !finfo2.is_trivial() ) {
	  continue;
	}
	if ( !check_intersect(fault1, fault2) ) {
	  continue;
	}
	auto cond2 = finfo2.sufficient_condition();
	++ check_num;
	if ( checker.check(fault1, fault2, cond2) ) {
	  mMgr.delete_fault(fault2);
	  -- mFaultNum;
	  ++ success_num;
	}
      }
    }
  }

  if ( debug ) {
    timer.stop();
    cout << "after trivial_reduction2:              " << mFaultNum << endl;
    cout << "    # of total checkes:                " << check_num << endl
	 << "    # of total successes:              " << success_num << endl
	 << "CPU time:                              " << timer.get_time() << endl;
  }
}

// @brief fault1 が trivial な場合の処理
void
Reducer::trivial_reduction3(
  const JsonValue& option
)
{
  Timer timer;
  timer.start();

  bool debug = get_debug(option);
  if ( debug ) {
    cout << "---------------------------------------" << endl;
  }

  SizeType check1_num = 0;
  SizeType check2_num = 0;
  SizeType dom1_num = 0;
  SizeType dom2_num = 0;
  SizeType success_num = 0;

  SizeType nf = mFFRFaultList.fault_list().size();
  SizeType N = 50;
  for ( SizeType start_pos = 0; start_pos < nf; start_pos += N ) {
    SizeType end_pos = std::min(start_pos + N, nf);
    // 支配故障の候補リスト
    vector<const TpgFault*> fault1_list;
    // 被支配故障の候補の可能性のある故障のリスト
    vector<const TpgFault*> fault2_list;
    // fault2_list のマーク
    vector<bool> fault2_mark(network().max_fault_id(), false);
    // 被支配故障の候補が属するFFRのリスト
    vector<const TpgFFR*> ffr2_list;
    unordered_set<SizeType> ffr2_mark;
    // 故障番号とFFR番号のペアをキーにして故障のリストを保持する辞書
    unordered_map<Key, vector<const TpgFault*>> fault2_list_map;
    for ( SizeType i = start_pos; i < end_pos; ++ i ) {
      auto fault1 = mFFRFaultList.fault_list()[i];
      auto& finfo1 = fault_info(fault1);
      if ( finfo1.is_deleted() ) {
	continue;
      }
      if ( !finfo1.is_trivial() ) {
	continue;
      }
      fault1_list.push_back(fault1);
      auto ffr1 = network().ffr(fault1);
      for ( auto fault2: dom_cand_list(fault1) ) {
	auto& finfo2 = fault_info(fault2);
	if ( finfo2.is_deleted() ) {
	  continue;
	}
	if ( finfo2.is_trivial() ) {
	  continue;
	}
	auto ffr2 = network().ffr(fault2);
	if ( ffr2 == ffr1 ) {
	  continue;
	}
	if ( !check_intersect(ffr1, ffr2) ) {
	  continue;
	}
	if ( !fault2_mark[fault2->id()] ) {
	  fault2_mark[fault2->id()] = true;
	  fault2_list.push_back(fault2);
	}
	auto ffr2_id = ffr2->id();
	if ( ffr2_mark.count(ffr2_id) == 0 ) {
	  ffr2_mark.emplace(ffr2_id);
	  ffr2_list.push_back(ffr2);
	}
	auto key = Key{fault1->id(), ffr2_id};
	if ( fault2_list_map.count(key) == 0 ) {
	  fault2_list_map.emplace(key, vector<const TpgFault*>{});
	}
	fault2_list_map.at(key).push_back(fault2);
      }
      if ( fault2_list.empty() ) {
	continue;
      }
    }
    auto tmp_list{fault2_list};
    tmp_list.insert(tmp_list.end(), fault1_list.begin(), fault1_list.end());
    ++ dom1_num;
    TrivialChecker1 checker1{network(), tmp_list, option};
    for ( auto ffr2: ffr2_list ) {
      ++ dom2_num;
      TrivialChecker3 checker2{network(), fault1_list, ffr2, option};
      for ( auto fault1: fault1_list ) {
	auto key = Key{fault1->id(), ffr2->id()};
	if ( fault2_list_map.count(key) == 0 ) {
	  continue;
	}
	if ( !check_intersect(fault1, ffr2) ) {
	  continue;
	}
	// fault1 の検出条件と ffr2 の根の出力の故障伝搬条件を調べる．
	++ check2_num;
	auto cond1 = fault_info(fault1).mandatory_condition();
	if ( !checker2.check(cond1) ) {
	  continue;
	}
	// fault1 の検出条件と fault2 の FFR 内の検出条件を調べる．
	auto& fault2_list = fault2_list_map.at(key);
	for ( auto fault2: fault2_list ) {
	  auto& finfo2 = fault_info(fault2);
	  if ( finfo2.is_deleted() ) {
	    continue;
	  }
	  if ( finfo2.is_trivial() ) {
	    continue;
	  }
	  ++ check1_num;
	  auto cond2 = fault2->ffr_propagate_condition();
	  if ( checker1.check(cond1, cond2) ) {
	    mMgr.delete_fault(fault2);
	    -- mFaultNum;
	    ++ success_num;
	  }
	}
      }
    }
  }

  if ( debug ) {
    timer.stop();
    cout << "after trivial_reduction3:              " << mFaultNum << endl;
    cout << "    # of total checkes(1):             " << check1_num << endl
	 << "    # of total checkes(2):             " << check2_num << endl
	 << "    # of total successes:              " << success_num << endl
	 << "    # of DomCheckers(1):               " << dom1_num << endl
	 << "    # of DomCheckers(2):               " << dom2_num << endl
	 << "CPU time:                              " << timer.get_time() << endl;
  }
}

// @brief 異なる FFR 間の支配故障のチェックを行う．
void
Reducer::global_reduction(
  const JsonValue& option,
  bool skip_trivial
)
{
  Timer timer;
  timer.start();

  bool debug = get_debug(option);
  if ( debug ) {
    cout << "---------------------------------------" << endl;
  }

  // skip_trivial == true の時は trivial な支配故障のチェックは
  // 済んでいる．

  SizeType check1_num = 0;
  SizeType check2_num = 0;
  SizeType dom1_num = 0;
  SizeType dom2_num = 0;
  SizeType success_num = 0;
  for ( auto ffr1: ffr_list() ) {
    // 候補の可能性のある故障のリスト
    vector<const TpgFault*> fault2_list;
    // fault2_list のマーク
    vector<bool> fault2_mark(network().max_fault_id(), false);
    // ffr1 に含まれる故障の被支配故障の候補が属するFFRを求める．
    vector<const TpgFFR*> ffr2_list;
    unordered_set<SizeType> ffr2_mark;
    // 故障番号とFFR番号のペアをキーにして故障のリストを保持する辞書
    unordered_map<Key, vector<const TpgFault*>> fault2_list_map;
    for ( auto fault1: fault_list(ffr1) ) {
      auto& finfo1 = fault_info(fault1);
      if ( finfo1.is_deleted() ) {
	continue;
      }
      if ( skip_trivial && finfo1.is_trivial() ) {
	continue;
      }
      for ( auto fault2: dom_cand_list(fault1) ) {
	auto& finfo2 = fault_info(fault2);
	if ( finfo2.is_deleted() ) {
	  continue;
	}
	if ( skip_trivial && finfo2.is_trivial() ) {
	  continue;
	}
	auto ffr2 = network().ffr(fault2);
	if ( ffr2 == ffr1 ) {
	  continue;
	}
	if ( !check_intersect(ffr1, ffr2) ) {
	  continue;
	}
	if ( !fault2_mark[fault2->id()] ) {
	  fault2_mark[fault2->id()] = true;
	  fault2_list.push_back(fault2);
	}
	auto ffr2_id = ffr2->id();
	if ( ffr2_mark.count(ffr2_id) == 0 ) {
	  ffr2_mark.emplace(ffr2_id);
	  ffr2_list.push_back(ffr2);
	}
	auto key = Key{fault1->id(), ffr2_id};
	if ( fault2_list_map.count(key) == 0 ) {
	  fault2_list_map.emplace(key, vector<const TpgFault*>{});
	}
	fault2_list_map.at(key).push_back(fault2);
      }
    }
    if ( fault2_list.empty() ) {
      continue;
    }
    ++ dom1_num;
    SimpleDomChecker checker1{network(), ffr1, fault2_list, option};
    for ( auto ffr2: ffr2_list ) {
      ++ dom2_num;
      DomChecker checker2{network(), ffr1, ffr2, option};
      for ( auto fault1: fault_list(ffr1) ) {
	auto& finfo1 = fault_info(fault1);
	if ( finfo1.is_deleted() ) {
	  continue;
	}
	if ( skip_trivial && finfo1.is_trivial() ) {
	  continue;
	}
	auto key = Key{fault1->id(), ffr2->id()};
	if ( fault2_list_map.count(key) == 0 ) {
	  continue;
	}
	if ( !check_intersect(ffr1, ffr2) ) {
	  continue;
	}
	// fault1 の検出条件と ffr2 の根の出力の故障伝搬条件を調べる．
	++ check2_num;
	if ( !checker2.check(fault1) ) {
	  continue;
	}
	// fault1 の検出条件と fault2 の FFR 内の検出条件を調べる．
	auto& fault2_list = fault2_list_map.at(key);
	for ( auto fault2: fault2_list ) {
	  auto& finfo2 = fault_info(fault2);
	  if ( finfo2.is_deleted() ) {
	    continue;
	  }
	  if ( skip_trivial && finfo2.is_trivial() ) {
	    continue;
	  }
	  ++ check1_num;
	  if ( checker1.check(fault1, fault2) ) {
	    mMgr.delete_fault(fault2);
	    -- mFaultNum;
	    ++ success_num;
	  }
	}
      }
    }
  }

  if ( debug ) {
    timer.stop();
    cout << "after global dominance reduction:      " << mFaultNum << endl;
    cout << "    # of total checkes(1):             " << check1_num << endl
	 << "    # of total checkes(2):             " << check2_num << endl
	 << "    # of total successes:              " << success_num << endl
	 << "    # of DomCheckers(1):               " << dom1_num << endl
	 << "    # of DomCheckers(2):               " << dom2_num << endl
	 << "CPU time:                              " << timer.get_time() << endl;
  }
}

// @brief 2つの FFR が共通部分を持つか調べる．
bool
Reducer::check_intersect(
  const TpgFFR* ffr1,
  const TpgFFR* ffr2
)
{
  auto& list_a = mInputListArray[ffr1->id()];
  auto& list_b = mInputListArray[ffr2->id()];
  auto rpos_a = list_a.begin();
  auto rpos_b = list_b.begin();
  auto epos_a = list_a.end();
  auto epos_b = list_b.end();
  while ( rpos_a != epos_a && rpos_b != epos_b ) {
    auto a = *rpos_a;
    auto b = *rpos_b;
    if ( a < b ) {
      ++ rpos_a;
    }
    else if ( a > b ) {
      ++ rpos_b;
    }
    else { // a == b
      return true;
    }
  }
  return false;
}

// @brief 2つの故障が共通部分を持つか調べる．
bool
Reducer::check_intersect(
  const TpgFault* fault1,
  const TpgFault* fault2
)
{
  return check_intersect(network().ffr(fault1), network().ffr(fault2));
}

// @brief 2つの故障が共通部分を持つか調べる．
bool
Reducer::check_intersect(
  const TpgFault* fault1,
  const TpgFFR* ffr2
)
{
  return check_intersect(network().ffr(fault1), ffr2);
}

END_NAMESPACE_DRUID
