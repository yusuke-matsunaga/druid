
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
#include "SimpleDomChecker.h"
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
    mDelMark(network.max_fault_id())
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
  // FFRの根のノード番号をキーにして故障のリストを保持する辞書
  unordered_map<SizeType, vector<const TpgFault*>> ffr_fault_map;
  for ( auto fault: fault_list ) {
    auto root = fault->ffr_root();
    if ( ffr_fault_map.count(root->id()) == 0 ) {
      ffr_fault_map.emplace(root->id(), vector<const TpgFault*>{});
    }
    ffr_fault_map.at(root->id()).push_back(fault);
  }

  for ( auto ffr: mNetwork.ffr_list() ) {
    // FFR 単位の故障リスト
    if ( ffr_fault_map.count(ffr->root()->id()) == 0 ) {
      // 対象の故障がなかった．
      continue;
    }
    auto& ffr_fault_list = ffr_fault_map.at(ffr->root()->id());

    FFRDomChecker checker{mNetwork, ffr, mOption};
    ++ dom_num;

    // 支配関係を調べ，代表故障のみを残す．
    for ( auto fault1: ffr_fault_list ) {
      if ( mDelMark[fault1->id()] ) {
	continue;
      }
      vector<const TpgFault*> fault2_list;
      for ( auto fault2: mDomCandListArray[fault1->id()] ) {
	if ( fault2->ffr_root() == fault1->ffr_root() && !mDelMark[fault2->id()] ) {
	  fault2_list.push_back(fault2);
	}
      }
      if ( !fault2_list.empty() ) {
	++ check_num;
	success_num += checker.check(fault1, fault2_list, mDelMark);
      }
    }
  }

  vector<const TpgFault*> ans_list;
  for ( auto fault: fault_list ) {
    if ( !mDelMark[fault->id()] ) {
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

#if 0
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

  // 根のノード番号をキーにしたFFRの辞書
  unordered_map<SizeType, const TpgFFR*> ffr_map;
  for ( auto ffr: mNetwork.ffr_list() ) {
    auto root = ffr->root();
    ffr_map.emplace(root->id(), ffr);
  }

  // チェックが必要な故障を持つFFRのリスト
  vector<const TpgFFR*> ffr_list;
  // FFR番号をキーにして関連する故障のリストを持つ辞書
  unordered_map<SizeType, vector<const TpgFault*>> ffr_fault_list_map;
  for ( auto fault: fault_list ) {
    auto root = fault->ffr_root();
    auto ffr = ffr_map.at(root->id());
    if ( ffr_fault_list_map.count(ffr->id()) == 0 ) {
      ffr_fault_list_map.emplace(ffr->id(), vector<const TpgFault*>{});
      ffr_list.push_back(ffr);
    }
    ffr_fault_list_map.at(ffr->id()).push_back(fault);
  }

  SizeType check_num = 0;
  SizeType dom_num = 0;
  SizeType success_num = 0;
  for ( auto ffr1: ffr_list ) {
    // ffr1 に含まれる故障の被支配故障の候補が属するFFRを求める．
    vector<const TpgFFR*> ffr2_list;
    unordered_set<SizeType> ffr2_mark;
    // 故障番号とFFR番号のペアをキーにして故障のリストを保持する辞書
    unordered_map<Key, vector<const TpgFault*>> fault2_list_map;
    for ( auto fault1: ffr_fault_list_map.at(ffr1->id()) ) {
      if ( mDelMark[fault1->id()] ) {
	continue;
      }
      for ( auto fault2: mDomCandListArray[fault1->id()] ) {
	if ( mDelMark[fault2->id()] ) {
	  continue;
	}
	auto ffr2 = ffr_map.at(fault2->ffr_root()->id());
	if ( ffr2 == ffr1 ) {
	  continue;
	}
	if ( ffr2_mark.count(ffr2->id()) == 0 ) {
	  ffr2_mark.emplace(ffr2->id());
	  ffr2_list.push_back(ffr2);
	}
	auto key = Key{fault1->id(), ffr2->id()};
	if ( fault2_list_map.count(key) == 0 ) {
	  fault2_list_map.emplace(key, vector<const TpgFault*>{});
	}
	fault2_list_map.at(key).push_back(fault2);
      }
    }
    vector<const TpgFault*> fault2_list;
    for ( auto ffr2: ffr2_list ) {
      ++ dom_num;
      DomChecker dom_checker{mNetwork, ffr1, ffr2, mOption};
      for ( auto fault1: ffr_fault_list_map.at(ffr1->id()) ) {
	if ( mDelMark[fault1->id()] ) {
	  continue;
	}
	// fault1 の被支配故障の候補のうち ffr2 に関係するものを集める．
	auto key = Key{fault1->id(), ffr2->id()};
	if ( fault2_list_map.count(key) > 0 ) {
	  auto& fault2_list = fault2_list_map.at(key);
	  ++ check_num;
	  success_num += dom_checker.check(fault1, fault2_list, mDelMark);
	}
      }
    }
  }

  vector<const TpgFault*> ans_list;
  for ( auto fault: fault_list ) {
    if ( !mDelMark[fault->id()] ) {
      ans_list.push_back(fault);
    }
  }

  if ( mDebug ) {
    timer.stop();
    SizeType n = ans_list.size();
    cout << "after global dominance reduction:      " << n << endl;
    cout << "    # of total checkes:                " << check_num << endl
	 << "    # of total successes:              " << success_num << endl
	 << "    # of DomCheckers:                  " << dom_num << endl
	 << "CPU time:                              " << timer.get_time() << endl;
  }

  return ans_list;
}

#else

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

  // 根のノード番号をキーにしたFFRの辞書
  unordered_map<SizeType, const TpgFFR*> ffr_map;
  for ( auto ffr: mNetwork.ffr_list() ) {
    auto root = ffr->root();
    ffr_map.emplace(root->id(), ffr);
  }

  // チェックが必要な故障を持つFFRのリスト
  vector<const TpgFFR*> ffr_list;
  // FFR番号をキーにして関連する故障のリストを持つ辞書
  unordered_map<SizeType, vector<const TpgFault*>> ffr_fault_list_map;
  for ( auto fault: fault_list ) {
    auto root = fault->ffr_root();
    auto ffr = ffr_map.at(root->id());
    if ( ffr_fault_list_map.count(ffr->id()) == 0 ) {
      ffr_fault_list_map.emplace(ffr->id(), vector<const TpgFault*>{});
      ffr_list.push_back(ffr);
    }
    ffr_fault_list_map.at(ffr->id()).push_back(fault);
  }

  SizeType check_num = 0;
  SizeType dom_num = 0;
  SizeType success_num = 0;
  for ( auto ffr1: ffr_list ) {
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
      if ( mDelMark[fault1->id()] ) {
	continue;
      }
      for ( auto fault2: mDomCandListArray[fault1->id()] ) {
	if ( mDelMark[fault2->id()] ) {
	  continue;
	}
	if ( !fault2_mark[fault2->id()] ) {
	  fault2_mark[fault2->id()] = true;
	  fault2_list.push_back(fault2);
	}
	auto ffr2 = ffr_map.at(fault2->ffr_root()->id());
	if ( ffr2 == ffr1 ) {
	  continue;
	}
	if ( ffr2_mark.count(ffr2->id()) == 0 ) {
	  ffr2_mark.emplace(ffr2->id());
	  ffr2_list.push_back(ffr2);
	}
	auto key = Key{fault1->id(), ffr2->id()};
	if ( fault2_list_map.count(key) == 0 ) {
	  fault2_list_map.emplace(key, vector<const TpgFault*>{});
	}
	fault2_list_map.at(key).push_back(fault2);
      }
    }
    if ( fault2_list.empty() ) {
      continue;
    }
    SimpleDomChecker checker1{mNetwork, ffr1, fault2_list, mOption};
    for ( auto ffr2: ffr2_list ) {
      ++ dom_num;
      DomChecker checker2{mNetwork, ffr1, ffr2, mOption};
      for ( auto fault1: ffr_fault_list_map.at(ffr1->id()) ) {
	if ( mDelMark[fault1->id()] ) {
	  continue;
	}
	auto key = Key{fault1->id(), ffr2->id()};
	if ( fault2_list_map.count(key) == 0 ) {
	  continue;
	}
	// fault1 の検出条件と ffr2 の根の出力の故障伝搬条件を調べる．
	if ( checker2.precheck(fault1) ) {
	  // fault1 の検出条件と fault2 の FFR 内の検出条件を調べる．
	  ++ check_num;
	  auto& fault2_list = fault2_list_map.at(key);
	  success_num += checker1.check(fault1, fault2_list, mDelMark);
	}
      }
    }
  }

  vector<const TpgFault*> ans_list;
  for ( auto fault: fault_list ) {
    if ( !mDelMark[fault->id()] ) {
      ans_list.push_back(fault);
    }
  }

  if ( mDebug ) {
    timer.stop();
    SizeType n = ans_list.size();
    cout << "after global dominance reduction:      " << n << endl;
    cout << "    # of total checkes:                " << check_num << endl
	 << "    # of total successes:              " << success_num << endl
	 << "    # of DomCheckers:                  " << dom_num << endl
	 << "CPU time:                              " << timer.get_time() << endl;
  }

  return ans_list;
}
#endif

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
