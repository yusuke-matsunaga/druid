
/// @file FaultReducer.cc
/// @brief FaultReducer の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "FaultReducer.h"
#include "BoolDiffEngine.h"
#include "DomCandGen.h"
#include "UndetChecker.h"
#include "DomChecker.h"
#include "TpgFFR.h"
#include "TpgFault.h"
#include "NodeValList.h"
#include "ym/Range.h"
#include <random>


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
FaultReducer::FaultReducer(
  const TpgNetwork& network,
  const vector<const TpgFault*>& fault_list,
  const vector<TestVector>& tv_list,
  const JsonValue& option
) : mNetwork{network},
    mFaultList{fault_list},
    mTvList{tv_list}
{
  if ( option.is_object() ) {
    if ( option.has_key("loop_limit") ) {
      auto val = option.get("loop_limit");
      mLoopLimit = val.get_int();
    }
    if ( option.has_key("ffr_checker") ) {
      mFFRCheckerOption = option.get("ffr_checker");
    }
    if ( option.has_key("dom_chcker_param") ) {
      mDomCheckerParam = SatInitParam{option.get("dom_checker_param")};
    }
    if ( option.has_key("undet_chcker_param") ) {
      mUndetCheckerParam = SatInitParam{option.get("undet_checker_param")};
    }
#if 0
    if ( option.has_key("algorithm") ) {
      auto val = option.get("algorithm");
      mAlgorithm = val.get_string();
    }
    if ( option.has_key("simple") ) {
      auto val = option.get("simple");
      mSimple = val.get_bool();
    }
#endif
    if ( option.has_key("debug") ) {
      auto val = option.get("debug");
      mDebug = val.get_bool();
    }
  }
}

// @brief 故障の支配関係を調べて故障リストを縮約する．
vector<const TpgFault*>
FaultReducer::run()
{
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

  if ( mDebug ) {
    mTimer.reset();
    mTimer.start();
  }

  DomCandGen dc_gen{mNetwork, mFaultList, mTvList};
  dc_gen.run(mLoopLimit, mDomCandList);

  if ( mDebug ) {
    mTimer.stop();
    cout << "Fault Simulation" << endl;
    cout << "CPU time:                              " << mTimer.get_time() << endl;
  }

  ffr_reduction();

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
#else
  dom_reduction();
#endif

  // mDeleted のついていない故障を fault_list に入れる．
  vector<const TpgFault*> ans_list;
  ans_list.reserve(count_faults());
  for ( auto fault: mFaultList ) {
    if ( !is_deleted(fault) ) {
      ans_list.push_back(fault);
    }
  }
  return ans_list;
}

// @brief 同一 FFR 内の支配故障のチェックを行う．
void
FaultReducer::ffr_reduction()
{
  if ( mDebug ) {
    cout << "# of initial faults:                   "
	 << mFaultList.size() << endl;
    mTimer.reset();
    mTimer.start();
  }

  for ( auto ffr: mNetwork.ffr_list() ) {
    // FFR 単位の故障リスト
    vector<const TpgFault*> tmp_fault_list;
    for ( auto fault: mFFRFaultList[ffr->id()] ) {
      if ( !is_deleted(fault) ) {
	tmp_fault_list.push_back(fault);
      }
    }
    if ( tmp_fault_list.empty() ) {
      // 対象の故障がなかった．
      continue;
    }

    BoolDiffEngine engine{mNetwork, ffr->root(), mFFRCheckerOption};

    // 支配関係を調べ，代表故障のみを残す．
    SizeType nf = tmp_fault_list.size();
    for ( auto i1: Range(nf) ) {
      auto fault1 = tmp_fault_list[i1];
      if ( is_deleted(fault1) ) {
	continue;
      }
      auto f1_ffr_cond = fault1->ffr_propagate_condition();
      auto assumptions = engine.conv_to_literal_list(f1_ffr_cond);
      for ( auto fault2: mDomCandList[fault1->id()] ) {
	if ( fault2->ffr_root() != fault1->ffr_root() ) {
	  continue;
	}
	if ( is_deleted(fault2) ) {
	  continue;
	}
	auto f2_ffr_cond = fault2->ffr_propagate_condition();
	f2_ffr_cond.diff(f1_ffr_cond);
	// f2_ffr_cond が空の場合には UNSAT となる．
	bool unsat = true;
	vector<SatLiteral> assumptions1{assumptions};
	// プレースホルダ
	assumptions1.push_back(SatLiteral::X);
	for ( auto nv: f2_ffr_cond ) {
	  auto lit1 = engine.conv_to_literal(nv);
	  assumptions1[assumptions.size()] = ~lit1;
	  if ( engine.check(assumptions1) == SatBool3::True ) {
	    unsat = false;
	    break;
	  }
	}
	if ( unsat ) {
	  // fault1 を検出する条件のもとでは fault2 も検出される．
	  // → fault2 は支配されている．
	  delete_fault(fault2);
	}
      }
    }
  }

  if ( mDebug ) {
    mTimer.stop();
    SizeType n = count_faults();
    cout << "after FFR dominance reduction:         " << n << endl;
    cout << "CPU time:                              " << mTimer.get_time() << endl;
  }
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
#else

// @brief 異なる FFR 間の支配故障の簡易チェックを行う．
void
FaultReducer::dom_reduction()
{
  if ( mDebug ) {
    mTimer.reset();
    mTimer.start();
  }

  SizeType check_num = 0;
  SizeType dom_num = 0;
  SizeType success_num = 0;
  for ( auto fault1: mFaultList ) {
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
	// 支配する故障の候補中に fault1 が含まれるか調べる．
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
      DomChecker dom_checker{mNetwork, ffr2->root(), fault1, mDomCheckerParam};
      for ( auto fault2: fault2_list ) {
	++ check_num;
	SatBool3 res = dom_checker.check(fault2);
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
    cout << "    # of total checkes:                " << check_num << endl
	 << "    # of total successes:              " << success_num << endl
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

END_NAMESPACE_DRUID
