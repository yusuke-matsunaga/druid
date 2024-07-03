
/// @file FaultReducer.cc
/// @brief FaultReducer の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "FaultReducer.h"
#include "FFRDomChecker.h"
#include "DomCandGen.h"
#include "DomChecker.h"
#include "DomChecker2.h"
#include "SimpleDomChecker.h"
#include "BaseEnc.h"
#include "BoolDiffEnc.h"
#include "TpgFFR.h"
#include "TpgFault.h"
#include "ym/Range.h"
#include <random>

BEGIN_NAMESPACE_DRUID

struct Key {
  SizeType fault_id;
  SizeType ffr_id;
};

bool
operator==(
  const Key& left,
  const Key& right
)
{
  return left.fault_id == right.fault_id &&
    left.ffr_id == right.ffr_id;
}

END_NAMESPACE_DRUID

BEGIN_NAMESPACE_STD

template<>
struct hash<DRUID_NAMESPACE::Key>
{
  SizeType
  operator()(
    const DRUID_NAMESPACE::Key& x
  ) const
  {
    return x.fault_id * 1023 + x.ffr_id;
  }
};

END_NAMESPACE_STD

BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
FaultReducer::FaultReducer(
  const TpgNetwork& network,
  const JsonValue& option
) : mNetwork{network},
    mOption{option},
    mDomCandListArray(network.max_fault_id()),
    mDelMark(network.max_fault_id()),
    mSuffCondArray(network.max_fault_id()),
    mMandCondArray(network.max_fault_id())
{
  if ( option.is_object() &&
       option.has_key("debug") ) {
    mDebug = option.get("debug").get_bool();
  }
}

// @brief 支配関係を用いて故障を削減する．
vector<const TpgFault*>
FaultReducer::run(
  const vector<const TpgFault*>& fault_list,
  const vector<TestVector>& tv_list
)
{
  gen_dom_cands(fault_list, tv_list);
  auto fault_list1 = ffr_reduction(fault_list);
  //fault_analysis(fault_list1);
  return global_reduction(fault_list1);
}

// @brief 同一FFR内の支配関係を用いて故障を削減する．
vector<const TpgFault*>
FaultReducer::ffr_reduction(
  const vector<const TpgFault*>& fault_list
)
{
  Timer timer;
  if ( mDebug ) {
    cout << "---------------------------------------" << endl;
    cout << "# of initial faults:                   "
	 << fault_list.size() << endl;
    timer.start();
  }

  SizeType check_num = 0;
  SizeType dom_num = 0;
  SizeType success_num = 0;
  // FFR番号をキーにして故障のリストを保持する辞書
  unordered_map<SizeType, vector<const TpgFault*>> ffr_fault_map;
  for ( auto fault: fault_list ) {
    auto ffr = mNetwork.ffr(fault);
    if ( ffr_fault_map.count(ffr->id()) == 0 ) {
      ffr_fault_map.emplace(ffr->id(), vector<const TpgFault*>{});
    }
    ffr_fault_map.at(ffr->id()).push_back(fault);
  }

  for ( auto ffr: mNetwork.ffr_list() ) {
    // FFR 単位の故障リスト
    if ( ffr_fault_map.count(ffr->id()) == 0 ) {
      // 対象の故障がなかった．
      continue;
    }
    auto& ffr_fault_list = ffr_fault_map.at(ffr->id());

    FFRDomChecker checker{mNetwork, ffr, mOption};
    ++ dom_num;

    // 支配関係を調べ，代表故障のみを残す．
    for ( auto fault1: ffr_fault_list ) {
      if ( is_deleted(fault1) ) {
	continue;
      }
      auto fault1_root = fault1->ffr_root();
      vector<const TpgFault*> fault2_list;
      for ( auto fault2: mDomCandListArray[fault1->id()] ) {
	if ( fault2->ffr_root() == fault1_root && !is_deleted(fault2) ) {
	  ++ check_num;
	  if ( checker.check(fault1, fault2) ) {
	    set_deleted(fault2);
	    ++ success_num;
	  }
	}
      }
    }
  }

  vector<const TpgFault*> ans_list;
  for ( auto fault: fault_list ) {
    if ( !is_deleted(fault) ) {
      ans_list.push_back(fault);
    }
  }

  if ( mDebug ) {
    timer.stop();
    SizeType n = ans_list.size();
    cout << "after FFR dominance reduction:         " << n << endl;
    cout << "    # of total checkes:                " << check_num << endl
	 << "    # of total successes:              " << success_num << endl
	 << "    # of FFRDomCheckers:               " << dom_num << endl
	 << "CPU time:                              " << timer.get_time() << endl;
  }

  return ans_list;
}

// @grep 故障シミュレーションを用いて被支配故障の候補を生成する．
void
FaultReducer::gen_dom_cands(
  const vector<const TpgFault*>& fault_list,
  const vector<TestVector>& tv_list
)
{
  Timer timer;
  if ( mDebug ) {
    cout << "---------------------------------------" << endl;
    cout << "Fault Simulation" << endl;
    timer.start();
  }

  DomCandGen dc_gen{mNetwork, fault_list, tv_list};
  int loop_limit = 1;
  if ( mOption.is_object() && mOption.has_key("loop_limit") ) {
    loop_limit = mOption.get("loop_limit").get_int();
  }
  dc_gen.run(loop_limit, mDomCandListArray);

  if ( mDebug ) {
    timer.stop();
    SizeType n = 0;
    for ( auto f: fault_list ) {
      n += mDomCandListArray[f->id()].size();
    }
    cout << "Total Candidates:                      " << n << endl;
    cout << "CPU time:                              " << timer.get_time() << endl;
  }
}

// @brief 故障の解析を行う．
void
FaultReducer::fault_analysis(
  const vector<const TpgFault*>& fault_list
)
{
  Timer timer;
  if ( mDebug ) {
    cout << "---------------------------------------" << endl;
    timer.start();
  }
  // FFR 番号をキーにして関係する故障のリストを保持する辞書
  unordered_map<SizeType, vector<const TpgFault*>> ffr_fault_list_map;
  vector<const TpgFFR*> ffr_list;
  for ( auto fault: fault_list ) {
    auto ffr = mNetwork.ffr(fault);
    auto ffr_id = ffr->id();
    if ( ffr_fault_list_map.count(ffr_id) == 0 ) {
      ffr_fault_list_map.emplace(ffr_id, vector<const TpgFault*>{fault});
      ffr_list.push_back(ffr);
    }
    else {
      ffr_fault_list_map.at(ffr_id).push_back(fault);
    }
  }
  // FFR 単位で処理を行う．
  for ( auto ffr: ffr_list ) {
    BaseEnc base_enc{mNetwork, mOption};
    auto bd_enc = new BoolDiffEnc{base_enc, ffr->root(), mOption};
    base_enc.make_cnf({}, {ffr->root()});
    for ( auto fault: ffr_fault_list_map.at(ffr->id()) ) {
      auto ffr_cond = fault->ffr_propagate_condition();
      auto assumptions = base_enc.conv_to_literal_list(ffr_cond);
      assumptions.push_back(bd_enc->prop_var());
      auto res = base_enc.solver().solve(assumptions);
      ASSERT_COND( res == SatBool3::True );
      auto suff_cond = bd_enc->extract_sufficient_condition();
      mSuffCondArray[fault->id()] = suff_cond;
      NodeTimeValList mand_cond;
      auto assumptions1 = assumptions;
      assumptions1.push_back(SatLiteral::X);
      for ( auto nv: suff_cond ) {
	auto lit = base_enc.conv_to_literal(nv);
	assumptions1.back() = ~lit;
	auto res = base_enc.solver().solve(assumptions1);
	if ( res != SatBool3::True ) {
	  mand_cond.add(nv);
	}
      }
      mMandCondArray[fault->id()] = mand_cond;
    }
  }

  if ( mDebug ) {
    timer.stop();
    cout << "CPU time:                              " << timer.get_time() << endl;
  }
}

// @brief 異なる FFR 間の支配故障のチェックを行う．
vector<const TpgFault*>
FaultReducer::global_reduction(
  const vector<const TpgFault*>& fault_list
)
{
  Timer timer;
  if ( mDebug ) {
    cout << "---------------------------------------" << endl;
    timer.start();
  }

  // チェックが必要な故障を持つFFRのリスト
  vector<const TpgFFR*> ffr_list;
  // FFR番号をキーにして関連する故障のリストを持つ辞書
  unordered_map<SizeType, vector<const TpgFault*>> ffr_fault_list_map;
  for ( auto fault: fault_list ) {
    auto ffr = mNetwork.ffr(fault);
    auto ffr_id = ffr->id();
    if ( ffr_fault_list_map.count(ffr_id) == 0 ) {
      ffr_fault_list_map.emplace(ffr_id, vector<const TpgFault*>{});
      ffr_list.push_back(ffr);
    }
    ffr_fault_list_map.at(ffr_id).push_back(fault);
  }

  SizeType check1_num = 0;
  SizeType check2_num = 0;
  SizeType dom1_num = 0;
  SizeType dom2_num = 0;
  SizeType success_num = 0;
  SizeType nffr = ffr_list.size();
  SizeType ffr_count = 0;
  for ( auto ffr1: ffr_list ) {
    if ( 0 ) {
      SizeType nfault = ffr_fault_list_map.at(ffr1->id()).size();
      cout << ffr_count << " / " << nffr << endl;
    }
    ++ ffr_count;
    // 候補の可能性のある故障のリスト
    vector<const TpgFault*> fault2_list;
    // fault2_list のマーク
    vector<bool> fault2_mark(mNetwork.max_fault_id(), false);
    // ffr1 に含まれる故障の被支配故障の候補が属するFFRを求める．
    vector<const TpgFFR*> ffr2_list;
    unordered_set<SizeType> ffr2_mark;
    // 故障番号とFFR番号のペアをキーにして故障のリストを保持する辞書
    unordered_map<Key, vector<const TpgFault*>> fault2_list_map;
    for ( auto fault1: ffr_fault_list_map.at(ffr1->id()) ) {
      if ( is_deleted(fault1) ) {
	continue;
      }
      for ( auto fault2: mDomCandListArray[fault1->id()] ) {
	if ( is_deleted(fault2) ) {
	  continue;
	}
	auto ffr2 = mNetwork.ffr(fault2);
	if ( ffr2 == ffr1 ) {
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
    if ( 0 ) {
      SizeType nfault1 = ffr_fault_list_map.at(ffr1->id()).size();
      SizeType nfault2 = fault2_list.size();
      SizeType nffr2 = ffr2_list.size();
      cout << "  " << nfault1 << " x " << nfault2
	   << "(" << nffr2 << ")" << endl;
    }
    if ( fault2_list.empty() ) {
      continue;
    }
    ++ dom1_num;
    SimpleDomChecker checker1{mNetwork, ffr1, fault2_list, mOption};
    for ( auto ffr2: ffr2_list ) {
      ++ dom2_num;
      DomChecker checker2{mNetwork, ffr1, ffr2, mOption};
      for ( auto fault1: ffr_fault_list_map.at(ffr1->id()) ) {
	if ( is_deleted(fault1) ) {
	  continue;
	}
	auto key = Key{fault1->id(), ffr2->id()};
	if ( fault2_list_map.count(key) == 0 ) {
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
	  if ( is_deleted(fault2) ) {
	    continue;
	  }
	  ++ check1_num;
	  if ( checker1.check(fault1, fault2) ) {
	    set_deleted(fault2);
	    ++ success_num;
	  }
	}
      }
    }
  }
  vector<const TpgFault*> ans_list;
  for ( auto fault: fault_list ) {
    if ( !is_deleted(fault) ) {
      ans_list.push_back(fault);
    }
  }

  if ( mDebug ) {
    timer.stop();
    SizeType n = ans_list.size();
    cout << "after global dominance reduction:      " << n << endl;
    cout << "    # of total checkes(1):             " << check1_num << endl
	 << "    # of total checkes(2):             " << check2_num << endl
	 << "    # of total successes:              " << success_num << endl
	 << "    # of DomCheckers(1):               " << dom1_num << endl
	 << "    # of DomCheckers(2):               " << dom2_num << endl
	 << "CPU time:                              " << timer.get_time() << endl;
  }

  return ans_list;
}

#if 0
// @brief 故障の支配関係を調べて故障リストを縮約する．
vector<const TpgFault*>
FaultReducer::run()
{
#if 0
  // ノードごとの故障リストを作る．
  // 同時に max_fault_id を求める．
  SizeType n = mFaultList.size();
  vector<vector<const TpgFault*>> node_fault_list(mNetwork.node_num());
  SizeType max_fault_id = 0;
  for ( auto f: mFaultList ) {
    auto node = f->origin_node();
    node_fault_list[node->id()].push_back(f);
    max_fault_id = std::max(max_fault_id, f->id());
  }
  ++ max_fault_id;

  // 削除マークを初期化する．
  mDeleted.clear();
  mDeleted.resize(max_fault_id, true);
  for ( auto f: mFaultList ) {
    mDeleted[f->id()] = false;
  }

  mDomCandList.clear();
  mDomCandList.resize(max_fault_id, {});

  // FFRごとの故障リストを作る．
  mFFRFaultList.clear();
  mFFRFaultList.resize(mNetwork.ffr_num());
  for ( auto ffr: mNetwork.ffr_list() ) {
    auto& dst_list = mFFRFaultList[ffr->id()];
    for ( auto node: ffr->node_list() ) {
      for ( auto f: node_fault_list[node->id()] ) {
	dst_list.push_back(f);
      }
    }
  }
#endif

  fault_list = ffr_reduction(network, fault_list, option);

  auto cand_list_array = generate_dom_cands(network, fault_list, tv_list, option);

  return global_reduction(network, fault_list, cand_list_array, option);

#if 0
  // algorithm に従って縮約を行う．
  if ( mAlgorithm == "red1" ) {
    dom_reduction1();
  }
  else if ( mAlgorithm == "red2" ) {
    dom_reduction2();
  }
  else if ( mAlgorithm == "red3" ) {
    dom_reduction3();
  }

  // mDeleted のついていない故障を fault_list に入れる．
  vector<const TpgFault*> ans_list;
  ans_list.reserve(count_faults());
  for ( auto fault: mFaultList ) {
    if ( !is_deleted(fault) ) {
      ans_list.push_back(fault);
    }
  }
  return ans_list;
#endif
}

#if 0
// @brief 異なる FFR 間の支配故障の簡易チェックを行う．
void
FaultReducer::dom_reduction1()
{
  if ( mDebug ) {
    mTimer.reset();
    mTimer.start();
  }

  SizeType check_num = 0;
  SizeType success_num = 0;
  for ( auto fault1: mFaultList ) {
    if ( is_deleted(fault1) ) {
      continue;
    }
    UndetChecker undet_checker{mNetwork, fault1, mUndetCheckerParam};
    for ( auto fault2: mFaultList ) {
      if ( fault2 == fault1 ) {
	continue;
      }
      if ( is_deleted(fault2) ) {
	continue;
      }
      if ( fault1->ffr_root() == fault2->ffr_root() ) {
	// 同じ FFR ならチェック済み
	continue;
      }

      if ( !undet_checker.has_gvar(fault2->origin_node()) ) {
	// fault1 が fault2 の起点に関係ない．
	continue;
      }
      // fault1 が fault2 の mDomCandList に含まれるか調べる．
      bool found = false;
      for ( auto fault3: mDomCandList[fault2->id()] ) {
	if ( fault3 == fault1 ) {
	  found = true;
	  break;
	}
      }
      if ( found ) {
	++ check_num;
	const auto& cond = mSimple ? fault2->ffr_propagate_condition() :
	  fi2.mandatory_condition();
	SatBool3 res = undet_checker.check(cond);
	if ( res == SatBool3::False ) {
	  ++ success_num;
	  // fault2 が検出可能の条件のもとで fault1 が検出不能となることはない．
	  // fault2 が fault1 を支配している．
	  delete_fault(fault1);
	  break;
	}
      }
    }
  }

  if ( mDebug ) {
    mTimer.stop();
    SizeType n = count_faults();
    cout << "after semi-global dominance reduction: " << n << endl
	 << "    # of total checks:                 " << check_num << endl
	 << "    # of total successes:              " << success_num << endl
	 << "CPU time:                              " << mTimer.get_time() << endl;
  }
}

// @brief 異なる FFR 間の支配故障の簡易チェックを行う．
void
FaultReducer::dom_reduction2()
{
  if ( mDebug ) {
    mTimer.reset();
    mTimer.start();
  }

  SizeType check_num = 0;
  SizeType dom_num = 0;
  SizeType success_num = 0;
  for ( auto fault1: mFaultList ) {
    auto fault1 = fi1.fault();
    if ( is_deleted(fault1) ) {
      continue;
    }
    for ( auto ffr2: mNetwork.ffr_list() ) {
      if ( ffr2->root() == fault1->ffr_root() ) {
	// 同じ FFR ならチェック済みのはず
	continue;
      }
      vector<const TpgFault*> fault2_list;
      for ( auto fault2: mFFRFaultList[ffr2->id()] ) {
	if ( is_deleted(fault2) ) {
	  continue;
	}
	auto& fi2 = mFaultInfoList[fault2->id()];
	bool found = false;
	for ( auto fault3: mDomCandList[fault2->id()] ) {
	  if ( fault3 == fault1 ) {
	    found = true;
	    break;
	  }
	}
	if ( found ) {
	  fault2_list.push_back(fault2);
	}
      }
      if ( fault2_list.empty() ) {
	continue;
      }
      ++ dom_num;
      DomChecker dom_checker{mNetwork, ffr2->root(), fault1->ffr_root(), {fault1}, mDomCheckerOption};
      for ( auto fault2: fault2_list ) {
	++ check_num;
	if ( dom_checker.check(fault2, fault1) ) {
	  ++ success_num;
	  // fault2 が検出可能の条件のもとで fault1 が検出不能となることはない．
	  // fault2 が fault1 を支配している．
	  delete_fault(fault1);
	  break;
	}
      }
      if ( is_deleted(fault1) ) {
	break;
      }
    }
  }

  if ( mDebug ) {
    mTimer.stop();
    SizeType n = count_faults();
    cout << "after global dominance reduction:      " << n << endl;
    cout << "    # of total checkes:                " << check_num << endl
	 << "    # of total successes:              " << success_num << endl
	 << "    # of DomCheckers:                  " << dom_num << endl
	 << "CPU time:                              " << mTimer.get_time() << endl;
  }
}

// @brief 異なる FFR 間の支配故障の簡易チェックを行う．
void
FaultReducer::dom_reduction3()
{
  if ( mDebug ) {
    mTimer.reset();
    mTimer.start();
  }

  SizeType check_num = 0;
  SizeType dom_num = 0;
  SizeType success_num = 0;
  SizeType u_check_num = 0;
  SizeType u_success_num = 0;
  for ( auto fi1: mFaultInfoList ) {
    auto fault1 = fi1.fault();
    if ( is_deleted(fault1) ) {
      continue;
    }
    UndetChecker undet_checker{mNetwork, fault1, mUndetCheckerParam};
    for ( auto ffr2: mNetwork.ffr_list() ) {
      if ( ffr2->root() == fault1->ffr_root() ) {
	continue;
      }
      vector<const TpgFault*> fault2_list;
      for ( auto fault2: mFFRFaultList[ffr2->id()] ) {
	if ( is_deleted(fault2) ) {
	  continue;
	}
	bool found = false;
	for ( auto fault3: mDomCandList[fault2->id()] ) {
	  if ( fault3 == fault1 ) {
	    found = true;
	    break;
	  }
	}
	if ( found ) {
	  fault2_list.push_back(fault2);
	}

      }
      if ( fault2_list.empty() ) {
	continue;
      }
      for ( auto fault2: fault2_list ) {
	if ( undet_checker.has_gvar(fault2->origin_node()) ) {
	  ++ u_check_num;
	  auto& fi2 = mFaultInfoList[fault2->id()];
	  const auto& cond = mSimple ? fault2->ffr_propagate_condition() :
	    fi2.mandatory_condition();
	  SatBool3 res = undet_checker.check(cond);
	  if ( res == SatBool3::False ) {
	    ++ u_success_num;
	    // fault2 が検出可能の条件のもとで fault1 が検出不能となることはない．
	    // fault2 が fault1 を支配している．
	    delete_fault(fault1);
	    break;
	  }
	}
      }
      if ( is_deleted(fault1) ) {
	break;
      }

      ++ dom_num;
      DomChecker dom_checker{mNetwork, ffr2->root(), fault1, mDomCheckerParam};
      for ( auto fault2: fault2_list ) {
	++ check_num;
	SatBool3 res = dom_checker.check_detectable(fault2);
	if ( res == SatBool3::False ) {
	  ++ success_num;
	  // fault2 が検出可能の条件のもとで fault1 が検出不能となることはない．
	  // fault2 が fault1 を支配している．
	  delete_fault(fault1);
	  break;
	}
      }
      if ( is_deleted(fault1) ) {
	break;
      }
    }
  }

  if ( mDebug ) {
    mTimer.stop();
    SizeType n = count_faults();
    cout << "after global dominance reduction:      " << n << endl;
    cout << "    # of total checkes(1):             " << check_num << endl
	 << "    # of total successes(1):           " << success_num << endl
	 << "    # of total checkes(2):             " << u_check_num << endl
	 << "    # of total successes(2):           " << u_success_num << endl
	 << "    # of DomCheckers:                  " << dom_num << endl
	 << "CPU time:                              " << mTimer.get_time() << endl;
  }
}
#endif

// @brief mFaultList 中の mDeleted マークが付いていない故障数を数える．
SizeType
FaultReducer::count_faults() const
{
  SizeType n = 0;
  for ( auto fault: mFaultList ) {
    if ( !is_deleted(fault) ) {
      ++ n;
    }
  }
  return n;
}
#endif

END_NAMESPACE_DRUID
