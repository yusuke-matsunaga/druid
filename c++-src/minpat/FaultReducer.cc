
/// @file FaultReducer.cc
/// @brief FaultReducer の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "FaultReducer.h"
#include "FaultAnalyzer.h"
#include "FFRFaultList.h"
#include "FFRDomChecker.h"
#include "DomCandGen.h"
#include "DomChecker.h"
#include "DomChecker2.h"
#include "SimpleDomChecker.h"
#include "TrivialChecker1.h"
#include "TrivialChecker2.h"
#include "TrivialChecker3.h"
#include "BaseEnc.h"
#include "BoolDiffEnc.h"
#include "TpgNodeSet.h"
#include "TpgFFR.h"
#include "TpgFault.h"
#include "ym/Range.h"
#include <random>
#include "NaiveDomChecker.h"


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
    mFaultInfoArray(network.max_fault_id())
{
  if ( option.is_object() &&
       option.has_key("debug") ) {
    mDebug = option.get("debug").get_bool();
  }
}

// @brief 支配関係を用いて故障を削減する．
vector<FaultInfo>
FaultReducer::run(
  const vector<const TpgFault*>& fault_list,
  const vector<TestVector>& tv_list
)
{
  gen_dom_cands(fault_list, tv_list);
  auto fault_list1 = ffr_reduction(fault_list);
  bool analyze = true;
  if ( mOption.is_object() &&
       mOption.has_key("analyze") ) {
    analyze = mOption.get("analyze").get_bool();
  }
  if ( analyze ) {
    fault_analysis(fault_list1);
    fault_list1 = trivial_reduction1(fault_list1);
    fault_list1 = trivial_reduction2(fault_list1);
    fault_list1 = trivial_reduction3(fault_list1);
    fault_list1 = global_reduction(fault_list1, true);
  }
  else {
    fault_list1 = global_reduction(fault_list1, false);
  }
  vector<FaultInfo> ans_list;
  ans_list.reserve(fault_list1.size());
  for ( auto fault: fault_list1 ) {
    auto& info = mFaultInfoArray[fault->id()];
    auto mand_cond = info.mMandCond;
    auto suff_cond = info.mSuffCond;
    ans_list.push_back(FaultInfo{fault, mand_cond, suff_cond});
  }
  return ans_list;
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
  FFRFaultList ffr_fault_list{mNetwork, fault_list};

  for ( auto ffr: ffr_fault_list.ffr_list() ) {
    // FFR 単位の故障リスト
    auto& fault1_list = ffr_fault_list.fault_list(ffr);

    FFRDomChecker checker{mNetwork, ffr, mOption};
    ++ dom_num;

    // 支配関係を調べ，代表故障のみを残す．
    for ( auto fault1: fault1_list ) {
      if ( is_deleted(fault1) ) {
	continue;
      }
      auto fault1_root = fault1->ffr_root();
      vector<const TpgFault*> fault2_list;
      for ( auto fault2: dom_cand_list(fault1) ) {
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

  // mDomCandListArray の逆向きのリストを作る．
  for ( auto fault1: fault_list ) {
    for ( auto fault2: dom_cand_list(fault1) ) {
      mFaultInfoArray[fault2->id()].mRevCandList.push_back(fault1);
    }
  }

  if ( mDebug ) {
    timer.stop();
    SizeType n = 0;
    for ( auto f: fault_list ) {
      n += dom_cand_list(f).size();
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

  SizeType nt = 0;

  FFRFaultList ffr_fault_list{mNetwork, fault_list};
  // FFR 単位で処理を行う．
  for ( auto ffr: ffr_fault_list.ffr_list() ) {
    FaultAnalyzer analyzer{mNetwork, ffr, mOption};
    for ( auto fault: ffr_fault_list.fault_list(ffr) ) {
      if ( is_deleted(fault) ) {
	continue;
      }
      auto& finfo = mFaultInfoArray[fault->id()];
      analyzer.extract_condition(fault, finfo.mSuffCond, finfo.mMandCond);
      finfo.mTrivial = compare(finfo.mSuffCond, finfo.mMandCond) == 3;
      if ( finfo.mTrivial ) {
	++ nt;
      }
    }
  }

  if ( mDebug ) {
    timer.stop();
    cout << "# of Trivial Condition Faults:         " << nt << endl;
    cout << "CPU time:                              " << timer.get_time() << endl;
  }
}

// @brief trivial な故障間の支配関係のチェックを行う．
vector<const TpgFault*>
FaultReducer::trivial_reduction1(
  const vector<const TpgFault*>& fault_list
)
{
  Timer timer;
  if ( mDebug ) {
    cout << "---------------------------------------" << endl;
    timer.start();
  }

  vector<const TpgFault*> tmp_fault_list;
  vector<bool> mark(mNetwork.max_fault_id(), false);
  for ( auto fault1: fault_list ) {
    if ( !is_trivial(fault1) ) {
      continue;
    }
    if ( !mark[fault1->id()] ) {
      mark[fault1->id()] = true;
      tmp_fault_list.push_back(fault1);
    }
    for ( auto fault2: dom_cand_list(fault1) ) {
      if ( !is_trivial(fault2) ) {
	continue;
      }
      if ( !mark[fault2->id()] ) {
	mark[fault2->id()] = true;
	tmp_fault_list.push_back(fault2);
      }
    }
  }

  TrivialChecker1 checker{mNetwork, tmp_fault_list, mOption};

  SizeType check_num = 0;
  SizeType success_num = 0;
  for ( auto fault1: fault_list ) {
    if ( is_deleted(fault1) || !is_trivial(fault1) ) {
      continue;
    }
    auto cond1 = mandatory_condition(fault1);
    for ( auto fault2: dom_cand_list(fault1) ) {
      if ( is_deleted(fault2) || !is_trivial(fault2) ) {
	continue;
      }
      auto cond2 = mandatory_condition(fault2);
      ++ check_num;
      if ( checker.check(cond1, cond2) ) {
	if ( 0 ) {
	  NaiveDomChecker checker2{mNetwork, fault1, fault2, mOption};
	  if ( !checker2.check() ) {
	    cout << fault1->str() << " " << fault2->str() << endl;
	  }
	}
	set_deleted(fault2);
	++ success_num;
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
    cout << "after trivial_reduction1:              " << n << endl;
    cout << "    # of total checkes:                " << check_num << endl
	 << "    # of total successes:              " << success_num << endl
	 << "CPU time:                              " << timer.get_time() << endl;
  }

  return ans_list;
}

// @brief trivial な故障が支配されている場合のチェックを行う．
vector<const TpgFault*>
FaultReducer::trivial_reduction2(
  const vector<const TpgFault*>& fault_list
)
{
  Timer timer;
  if ( mDebug ) {
    cout << "---------------------------------------" << endl;
    timer.start();
  }

  FFRFaultList ffr_fault_list{mNetwork, fault_list};

  SizeType check_num = 0;
  SizeType success_num = 0;
  for ( auto ffr1: ffr_fault_list.ffr_list() ) {
    vector<const TpgFault*> fault2_list;
    vector<bool> mark(mNetwork.max_fault_id(), false);
    for ( auto fault1: ffr_fault_list.fault_list(ffr1) ) {
      if ( is_trivial(fault1) ) {
	continue;
      }
      for ( auto fault2: dom_cand_list(fault1) ) {
	if ( !is_trivial(fault2) ) {
	  continue;
	}
	if ( !mark[fault2->id()] ) {
	  mark[fault2->id()] = true;
	  fault2_list.push_back(fault2);
	}
      }
    }
    if ( fault2_list.empty() ) {
      continue;
    }
    TrivialChecker2 checker{mNetwork, ffr1, fault2_list, mOption};
    for ( auto fault1: ffr_fault_list.fault_list(ffr1) ) {
      if ( is_deleted(fault1) || is_trivial(fault1) ) {
	continue;
      }
      for ( auto fault2: dom_cand_list(fault1) ) {
	if ( is_deleted(fault2) || !is_trivial(fault2) ) {
	  continue;
	}
	auto cond2 = mandatory_condition(fault2);
	++ check_num;
	if ( checker.check(fault1, fault2, cond2) ) {
	  set_deleted(fault2);
	  ++ success_num;
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
    cout << "after trivial_reduction2:              " << n << endl;
    cout << "    # of total checkes:                " << check_num << endl
	 << "    # of total successes:              " << success_num << endl
	 << "CPU time:                              " << timer.get_time() << endl;
  }

  return ans_list;
}

// @brief fault1 が trivial な場合の処理
vector<const TpgFault*>
FaultReducer::trivial_reduction3(
  const vector<const TpgFault*>& fault_list
)
{
  Timer timer;
  if ( mDebug ) {
    cout << "---------------------------------------" << endl;
    timer.start();
  }

  SizeType check1_num = 0;
  SizeType check2_num = 0;
  SizeType dom1_num = 0;
  SizeType dom2_num = 0;
  SizeType success_num = 0;

  FFRFaultList ffr_fault_list{mNetwork, fault_list};

  for ( auto rpos = fault_list.begin(); rpos != fault_list.end(); ) {
    // 支配故障の候補リスト
    vector<const TpgFault*> fault1_list;
    // 被支配故障の候補の可能性のある故障のリスト
    vector<const TpgFault*> fault2_list;
    // fault2_list のマーク
    vector<bool> fault2_mark(mNetwork.max_fault_id(), false);
    // 被支配故障の候補が属するFFRのリスト
    vector<const TpgFFR*> ffr2_list;
    unordered_set<SizeType> ffr2_mark;
    // 故障番号とFFR番号のペアをキーにして故障のリストを保持する辞書
    unordered_map<Key, vector<const TpgFault*>> fault2_list_map;
    for ( ; fault1_list.size() < 50 && rpos != fault_list.end(); ++ rpos ) {
      auto fault1 = *rpos;
      if ( is_deleted(fault1) || !is_trivial(fault1) ) {
	continue;
      }
      fault1_list.push_back(fault1);
      auto ffr1 = mNetwork.ffr(fault1);
      for ( auto fault2: dom_cand_list(fault1) ) {
	if ( is_deleted(fault2) || is_trivial(fault2) ) {
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
      if ( fault2_list.empty() ) {
	continue;
      }
    }
    auto tmp_list{fault2_list};
    tmp_list.insert(tmp_list.end(), fault1_list.begin(), fault1_list.end());
    ++ dom1_num;
    TrivialChecker1 checker1{mNetwork, tmp_list, mOption};
    for ( auto ffr2: ffr2_list ) {
      ++ dom2_num;
      TrivialChecker3 checker2{mNetwork, fault1_list, ffr2, mOption};
      for ( auto fault1: fault1_list ) {
	auto key = Key{fault1->id(), ffr2->id()};
	if ( fault2_list_map.count(key) == 0 ) {
	  continue;
	}
	// fault1 の検出条件と ffr2 の根の出力の故障伝搬条件を調べる．
	++ check2_num;
	auto cond1 = mandatory_condition(fault1);
	if ( !checker2.check(cond1) ) {
	  continue;
	}
	// fault1 の検出条件と fault2 の FFR 内の検出条件を調べる．
	auto& fault2_list = fault2_list_map.at(key);
	for ( auto fault2: fault2_list ) {
	  if ( is_deleted(fault2) || is_trivial(fault2) ) {
	    continue;
	  }
	  ++ check1_num;
	  auto cond2 = fault2->ffr_propagate_condition();
	  if ( checker1.check(cond1, cond2) ) {
	    set_deleted(fault2);
	    ++ success_num;
	  }
	}
      }
    }
  }

  vector<const TpgFault*> ans_list;
  SizeType nt = 0;
  for ( auto fault: fault_list ) {
    if ( !is_deleted(fault) ) {
      ans_list.push_back(fault);
      if ( mFaultInfoArray[fault->id()].mTrivial ) {
	++ nt;
      }
    }
  }

  if ( mDebug ) {
    timer.stop();
    SizeType n = ans_list.size();
    cout << "after trivial_reduction3:              " << n << "(" << nt << ")" << endl;
    cout << "    # of total checkes(1):             " << check1_num << endl
	 << "    # of total checkes(2):             " << check2_num << endl
	 << "    # of total successes:              " << success_num << endl
	 << "    # of DomCheckers(1):               " << dom1_num << endl
	 << "    # of DomCheckers(2):               " << dom2_num << endl
	 << "CPU time:                              " << timer.get_time() << endl;
  }

  return ans_list;
}

// @brief 異なる FFR 間の支配故障のチェックを行う．
vector<const TpgFault*>
FaultReducer::global_reduction(
  const vector<const TpgFault*>& fault_list,
  bool skip_trivial
)
{
  Timer timer;
  if ( mDebug ) {
    cout << "---------------------------------------" << endl;
    timer.start();
  }

  // trivial な支配故障のチェックは simple_global_reduction()
  // で行っている．

  FFRFaultList ffr_fault_list{mNetwork, fault_list};

  SizeType check1_num = 0;
  SizeType check2_num = 0;
  SizeType dom1_num = 0;
  SizeType dom2_num = 0;
  SizeType success_num = 0;
  SizeType nffr = ffr_fault_list.ffr_list().size();
  SizeType ffr_count = 0;
  for ( auto ffr1: ffr_fault_list.ffr_list() ) {
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
    for ( auto fault1: ffr_fault_list.fault_list(ffr1) ) {
      if ( is_deleted(fault1) || (skip_trivial && is_trivial(fault1)) ) {
	continue;
      }
      for ( auto fault2: dom_cand_list(fault1) ) {
	if ( is_deleted(fault2) || (skip_trivial && is_trivial(fault2)) ) {
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
    if ( fault2_list.empty() ) {
      continue;
    }
    ++ dom1_num;
    SimpleDomChecker checker1{mNetwork, ffr1, fault2_list, mOption};
    for ( auto ffr2: ffr2_list ) {
      ++ dom2_num;
      DomChecker checker2{mNetwork, ffr1, ffr2, mOption};
      for ( auto fault1: ffr_fault_list.fault_list(ffr1) ) {
	if ( is_deleted(fault1) || (skip_trivial && is_trivial(fault1)) ) {
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
	  if ( is_deleted(fault2) || (skip_trivial && is_trivial(fault2)) ) {
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
  SizeType nt = 0;
  for ( auto fault: fault_list ) {
    if ( is_deleted(fault) ) {
      continue;
    }
    ans_list.push_back(fault);
    if ( mFaultInfoArray[fault->id()].mTrivial ) {
      ++ nt;
    }
  }

  if ( mDebug ) {
    timer.stop();
    SizeType n = ans_list.size();
    cout << "after global dominance reduction:      " << n << "(" << nt << ")" << endl;
    cout << "    # of total checkes(1):             " << check1_num << endl
	 << "    # of total checkes(2):             " << check2_num << endl
	 << "    # of total successes:              " << success_num << endl
	 << "    # of DomCheckers(1):               " << dom1_num << endl
	 << "    # of DomCheckers(2):               " << dom2_num << endl
	 << "CPU time:                              " << timer.get_time() << endl;
  }

  return ans_list;
}

END_NAMESPACE_DRUID
