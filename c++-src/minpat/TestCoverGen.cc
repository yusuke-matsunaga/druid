
/// @file TestCoverGen.cc
/// @brief TestCoverGen の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "TestCoverGen.h"
#include "TestCover.h"
#include "FaultAnalyzer.h"
#include "FFRFaultList.h"
#include "FFRDomChecker.h"
#include "DomCandGen.h"
#include "DomChecker.h"
#include "SimpleDomChecker.h"
#include "TrivialChecker1.h"
#include "TrivialChecker2.h"
#include "TrivialChecker3.h"
#include "ExCubeGen.h"
#include "BaseEnc.h"
#include "BoolDiffEnc.h"
#include "TpgNodeSet.h"
#include "TpgFFR.h"
#include "TpgFault.h"
#include "ym/Range.h"
#include <random>
#include "NaiveDomChecker.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
TestCoverGen::TestCoverGen(
  const TpgNetwork& network,
  const JsonValue& option
) : mNetwork{network},
    mOption{option},
    mInputListArray(network.ffr_num()),
    mDomCandListArray(network.max_fault_id()),
    mFaultInfoArray(network.max_fault_id())
{
  if ( option.is_object() ) {
    if ( option.has_key("no_analysis") ) {
      mNoAnalysis = option.get("no_analysis").get_bool();
    }
    if ( option.has_key("debug") ) {
      mDebug = option.get("debug").get_bool();
    }
  }

  // FFR の構造を解析して関係ある入力ノードを求める．
  SizeType nn = network.node_num();
  for ( auto& ffr: network.ffr_list() ) {
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

// @brief 支配故障を求め，テストカバーを生成する．
vector<TestCover>
TestCoverGen::run(
  const vector<const TpgFault*>& fault_list,
  const vector<TestVector>& tv_list
)
{
  // 故障シミュレーションを用いて支配関係の候補リストを作る．
  gen_dom_cands(fault_list, tv_list);

  // FFR番号をキーにして故障のリストを保持する辞書
  FFRFaultList ffr_fault_list{mNetwork, fault_list};
  mFaultNum = fault_list.size();

  Timer timer;
  timer.start();
  // FFR内の支配関係を調べる．
  ffr_reduction(ffr_fault_list);
  if ( mNoAnalysis ) {
    global_reduction(ffr_fault_list, false);
    fault_analysis(ffr_fault_list);
  }
  else {
    // 故障の検出条件を調べる．
    fault_analysis(ffr_fault_list);
    // 簡単なチェックを行う．
    trivial_reduction1(ffr_fault_list);
    trivial_reduction2(ffr_fault_list);
    trivial_reduction3(ffr_fault_list);
    // 最終チェックを行う．
    global_reduction(ffr_fault_list, true);
  }
  timer.stop();
  if ( mDebug ) {
    cout << "Total CPU time: " << timer.get_time() << endl;
  }

  // 拡張テストカバーを作る．
  vector<TestCover> cover_list;
  cover_list.reserve(mFaultNum);
  for ( auto ffr: ffr_fault_list.ffr_list() ) {
    ExCubeGen gen{mNetwork, ffr, mOption};
    for ( auto fault: ffr_fault_list.fault_list(ffr) ) {
      if ( is_deleted(fault) ) {
	continue;
      }
      auto& info = mFaultInfoArray[fault->id()];
      auto mand_cond = info.mMandCond;
      auto suff_cond = info.mSuffCond;
      auto testcover = gen.run(fault, mand_cond, suff_cond);
      cover_list.push_back(testcover);
    }
  }
  return cover_list;
}

// @brief 同一FFR内の支配関係を用いて故障を削減する．
void
TestCoverGen::ffr_reduction(
  const FFRFaultList& ffr_fault_list
)
{
  Timer timer;
  if ( mDebug ) {
    cout << "---------------------------------------" << endl;
    cout << "# of initial faults:                   "
	 << mFaultNum << endl;
    timer.start();
  }

  SizeType check_num = 0;
  SizeType dom_num = 0;
  SizeType success_num = 0;

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

  if ( mDebug ) {
    timer.stop();
    cout << "after FFR dominance reduction:         " << mFaultNum << endl;
    cout << "    # of total checkes:                " << check_num << endl
	 << "    # of total successes:              " << success_num << endl
	 << "    # of FFRDomCheckers:               " << dom_num << endl
	 << "CPU time:                              " << timer.get_time() << endl;
  }
}

// @grep 故障シミュレーションを用いて被支配故障の候補を生成する．
void
TestCoverGen::gen_dom_cands(
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
TestCoverGen::fault_analysis(
  const FFRFaultList& ffr_fault_list
)
{
  Timer timer;
  if ( mDebug ) {
    cout << "---------------------------------------" << endl;
    timer.start();
  }

  SizeType nt = 0;

  // FFR 単位で処理を行う．
  for ( auto ffr: ffr_fault_list.ffr_list() ) {
    FaultAnalyzer analyzer{mNetwork, ffr, mOption};
    for ( auto fault: ffr_fault_list.fault_list(ffr) ) {
      if ( is_deleted(fault) ) {
	continue;
      }
      auto& finfo = mFaultInfoArray[fault->id()];
      finfo.mTrivial = analyzer.extract_condition(fault,
						  finfo.mSuffCond,
						  finfo.mMandCond);
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
void
TestCoverGen::trivial_reduction1(
  const FFRFaultList& ffr_fault_list
)
{
  Timer timer;
  if ( mDebug ) {
    cout << "---------------------------------------" << endl;
    timer.start();
  }

  vector<const TpgFault*> tmp_fault_list;
  vector<bool> mark(mNetwork.max_fault_id(), false);
  for ( auto fault1: ffr_fault_list.fault_list() ) {
    if ( is_deleted(fault1) ) {
      continue;
    }
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
  for ( auto fault1: ffr_fault_list.fault_list() ) {
    if ( is_deleted(fault1) || !is_trivial(fault1) ) {
      continue;
    }
    auto cond1 = mandatory_condition(fault1);
    for ( auto fault2: dom_cand_list(fault1) ) {
      if ( is_deleted(fault2) || !is_trivial(fault2) ) {
	continue;
      }
      if ( !check_intersect(fault1, fault2) ) {
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

  if ( mDebug ) {
    timer.stop();
    cout << "after trivial_reduction1:              " << mFaultNum << endl;
    cout << "    # of total checkes:                " << check_num << endl
	 << "    # of total successes:              " << success_num << endl
	 << "CPU time:                              " << timer.get_time() << endl;
  }
}

// @brief trivial な故障が支配されている場合のチェックを行う．
void
TestCoverGen::trivial_reduction2(
  const FFRFaultList& ffr_fault_list
)
{
  Timer timer;
  if ( mDebug ) {
    cout << "---------------------------------------" << endl;
    timer.start();
  }

  SizeType check_num = 0;
  SizeType success_num = 0;
  for ( auto ffr1: ffr_fault_list.ffr_list() ) {
    // ffr1 の TFI of TFO にマークをつける．
    vector<bool> tfi_mark(mNetwork.node_num(), false);
    auto tmp_list = TpgNodeSet::get_tfo_list(mNetwork.node_num(),
					     ffr1->root());
    TpgNodeSet::get_tfi_list(mNetwork.node_num(),
			     tmp_list,
			     [&](const TpgNode* node){
			       tfi_mark[node->id()] = true;
			     });

    vector<const TpgFault*> fault2_list;
    vector<bool> f2_mark(mNetwork.max_fault_id(), false);
    for ( auto fault1: ffr_fault_list.fault_list(ffr1) ) {
      if ( is_deleted(fault1) || is_trivial(fault1) ) {
	continue;
      }
      for ( auto fault2: dom_cand_list(fault1) ) {
	if ( is_deleted(fault2) || !is_trivial(fault2) ) {
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
    TrivialChecker2 checker{mNetwork, ffr1, fault2_list, mOption};
    for ( auto fault1: ffr_fault_list.fault_list(ffr1) ) {
      if ( is_deleted(fault1) || is_trivial(fault1) ) {
	continue;
      }
      for ( auto fault2: dom_cand_list(fault1) ) {
	if ( is_deleted(fault2) || !is_trivial(fault2) ) {
	  continue;
	}
	if ( !check_intersect(fault1, fault2) ) {
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

  if ( mDebug ) {
    timer.stop();
    cout << "after trivial_reduction2:              " << mFaultNum << endl;
    cout << "    # of total checkes:                " << check_num << endl
	 << "    # of total successes:              " << success_num << endl
	 << "CPU time:                              " << timer.get_time() << endl;
  }
}

// @brief fault1 が trivial な場合の処理
void
TestCoverGen::trivial_reduction3(
  const FFRFaultList& ffr_fault_list
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

  auto& fault_list = ffr_fault_list.fault_list();
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
    TrivialChecker1 checker1{mNetwork, tmp_list, mOption};
    for ( auto ffr2: ffr2_list ) {
      ++ dom2_num;
      TrivialChecker3 checker2{mNetwork, fault1_list, ffr2, mOption};
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

  if ( mDebug ) {
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
TestCoverGen::global_reduction(
  const FFRFaultList& ffr_fault_list,
  bool skip_trivial
)
{
  Timer timer;
  if ( mDebug ) {
    cout << "---------------------------------------" << endl;
    timer.start();
  }

  // skip_trivial == true の時は trivial な支配故障のチェックは
  // 済んでいる．

  SizeType check1_num = 0;
  SizeType check2_num = 0;
  SizeType dom1_num = 0;
  SizeType dom2_num = 0;
  SizeType success_num = 0;
  SizeType nffr = ffr_fault_list.ffr_list().size();
  for ( auto ffr1: ffr_fault_list.ffr_list() ) {
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

  if ( mDebug ) {
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
TestCoverGen::check_intersect(
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
TestCoverGen::check_intersect(
  const TpgFault* fault1,
  const TpgFault* fault2
)
{
  return check_intersect(mNetwork.ffr(fault1), mNetwork.ffr(fault2));
}

// @brief 2つの故障が共通部分を持つか調べる．
bool
TestCoverGen::check_intersect(
  const TpgFault* fault1,
  const TpgFFR* ffr2
)
{
  return check_intersect(mNetwork.ffr(fault1), ffr2);
}

END_NAMESPACE_DRUID
