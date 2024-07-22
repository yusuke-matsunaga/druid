
/// @file FaultInfoMgr.cc
/// @brief FaultInfoMgr の実装ファイル
/// @author Yusuke Mnatsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "FaultInfoMgr.h"
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
#include "ExCubeGen.h"
#include "ym/Range.h"
#include <random>


BEGIN_NAMESPACE_DRUID

inline
bool
operator==(
  const FaultInfoMgr::Key& left,
  const FaultInfoMgr::Key& right
)
{
  return left.fault_id == right.fault_id &&
    left.ffr_id == right.ffr_id;
}

// @brief コンストラクタ
FaultInfoMgr::FaultInfoMgr(
  const TpgNetwork& network,
  const vector<const TpgFault*>& fault_list
) : mNetwork{network},
    mFaultList{fault_list},
    mFFRFaultList{network, fault_list},
    mFaultInfoArray(network.max_fault_id()),
    mInputListArray(network.ffr_num()),
    mDomCandListArray(network.max_fault_id()),
    mRevCandListArray(network.max_fault_id())
{
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

// @brief 故障情報を求める．
void
FaultInfoMgr::generate(
  const JsonValue& option
)
{
  Timer timer;
  timer.start();

  bool debug = get_debug(option);

  for ( auto fault: mFaultList ) {
    fault_info(fault).set_fault(fault);
  }

  // FFR ごとに故障検出を行う．
  mActiveFaultList.clear();
  for ( auto ffr: ffr_list() ) {
    BaseEnc base_enc{mNetwork, option};
    auto bd_enc = new BoolDiffEnc{base_enc, ffr->root(), option};
    base_enc.make_cnf({}, {ffr->root()});
    // まず FFR の出力のブール微分を行う．
    auto prop_lit = bd_enc->prop_var();
    auto res = base_enc.solver().solve({prop_lit});
    if ( res == SatBool3::False ) {
      // この FFR 内の故障はすべてテスト不能
      for ( auto fault: fault_list(ffr) ) {
	fault_info(fault).set_untestable();
      }
    }
    else if ( res == SatBool3::X ) {
      // アボート
      // これ以上処理を続けても意味がない．
      continue;
    }
    // res == SatBool3::True
    // 個々の故障について処理を行う．
    for ( auto fault: fault_list(ffr) ) {
      auto ffr_cond = fault->ffr_propagate_condition();
      auto assumptions = base_enc.conv_to_literal_list(ffr_cond);
      assumptions.push_back(prop_lit);
      auto res = base_enc.solver().solve(assumptions);
      if ( res == SatBool3::False ) {
	fault_info(fault).set_untestable();
      }
      else if ( res == SatBool3::True ) {
	// 十分条件を求める．
	auto suff_cond = bd_enc->extract_sufficient_condition();
	suff_cond.merge(ffr_cond);
	auto pi_assign = base_enc.justify(suff_cond);
	fault_info(fault).set_sufficient_condition(suff_cond, pi_assign);
	mActiveFaultList.push_back(fault);
	++ mFaultNum;
      }
    }
  }

  timer.stop();
  if ( debug ) {
    cout << "Total faults: " << mFaultNum << endl
	 << "CPU time:     " << timer.get_time() << endl;
  }
}

// @brief 支配関係を用いて削除マークをつける．
void
FaultInfoMgr::reduce(
  const JsonValue& option ///< [in] オプション
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

  bool no_analyze = false;
  if ( option.is_object() && option.has_key("no_anlyze") ) {
    no_analyze = option.get("no_analyze").get_bool();
  }
  if ( no_analyze ) {
    global_reduction(option, false);
    fault_analysis(option);
  }
  else {
    // 故障の検出条件を調べる．
    fault_analysis(option);
    // 簡単なチェックを行う．
    trivial_reduction1(option);
    trivial_reduction2(option);
    trivial_reduction3(option);
    // 最終チェックを行う．
    global_reduction(option, true);
  }

  timer.stop();
  if ( debug ) {
    cout << "Total CPU time: " << timer.get_time() << endl;
  }
}

// @brief 残った故障に対してテストカバーを作る．
vector<TestCover>
FaultInfoMgr::gen_cover(
  const JsonValue& option
)
{
  Timer timer;
  timer.start();

  bool debug = get_debug(option);

  vector<TestCover> cover_list;
  cover_list.reserve(mFaultNum);
  SizeType nc = 0;
  for ( auto ffr: ffr_list() ) {
    ExCubeGen gen{mNetwork, ffr, option};
    for ( auto fault: fault_list(ffr) ) {
      if ( is_deleted(fault) ) {
	continue;
      }
      auto& finfo = fault_info(fault);
      auto& mand_cond = finfo.mandatory_condition();
      auto& suff_cond = finfo.sufficient_condition();
      auto testcover = gen.run(fault, mand_cond, suff_cond);
      cover_list.push_back(testcover);
      nc += testcover.cube_list().size();
    }
  }

  timer.stop();
  if ( debug ) {
    cout << "Total # of cubes: " << nc << endl
	 << "CPU time:         " << timer.get_time() << endl;
  }

  return cover_list;
}

// @grep 故障シミュレーションを用いて被支配故障の候補を生成する．
void
FaultInfoMgr::gen_dom_cands(
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
  tv_list.reserve(mFaultNum * Fsim::PP_BITLEN);
  for ( auto fault: mActiveFaultList ) {
    auto& finfo = fault_info(fault);
    if ( finfo.status() != FaultStatus::Detected ) {
      continue;
    }
    for ( SizeType i = 0; i < Fsim::PP_BITLEN; ++ i ) {
      TestVector tv{mNetwork, finfo.pi_assign()};
      tv.fix_x_from_random(randgen);
      tv_list.push_back(tv);
    }
  }

  DomCandGen dc_gen{mNetwork, mActiveFaultList, tv_list};
  dc_gen.run(limit, mDomCandListArray);

  // mDomCandListArray の逆向きのリストを作る．
  for ( auto fault1: mActiveFaultList ) {
    for ( auto fault2: dom_cand_list(fault1) ) {
      mRevCandListArray[fault2->id()].push_back(fault1);
    }
  }

  if ( debug ) {
    timer.stop();
    SizeType n = 0;
    for ( auto fault: mActiveFaultList ) {
      n += dom_cand_list(fault).size();
    }
    cout << "Total Candidates:                      " << n << endl;
    cout << "CPU time:                              " << timer.get_time() << endl;
  }
}

// @brief 同一FFR内の支配関係を用いて故障を削減する．
void
FaultInfoMgr::ffr_reduction(
  const JsonValue& option
)
{
  Timer timer;
  timer.start();

  bool debug = get_debug(option);
  if ( debug ) {
    cout << "---------------------------------------" << endl;
    cout << "# of initial faults:                   "
	 << mFaultList.size() << endl;
  }

  SizeType check_num = 0;
  SizeType dom_num = 0;
  SizeType success_num = 0;

  for ( auto ffr: ffr_list() ) {
    // FFR 単位の故障リスト
    auto& fault1_list = fault_list(ffr);

    FFRDomChecker checker{mNetwork, ffr, option};
    ++ dom_num;

    // 支配関係を調べ，代表故障のみを残す．
    for ( auto fault1: fault1_list ) {
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
	    fault_info(fault2).set_deleted();
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

// @brief 故障の解析を行う．
void
FaultInfoMgr::fault_analysis(
  const JsonValue& option
)
{
  Timer timer;
  timer.start();

  bool debug = get_debug(option);
  if ( debug ) {
    cout << "---------------------------------------" << endl;
  }

  SizeType nt = 0;

  // FFR 単位で処理を行う．
  for ( auto ffr: ffr_list() ) {
    FaultAnalyzer analyzer{mNetwork, ffr, option};
    for ( auto fault: fault_list(ffr) ) {
      if ( is_deleted(fault) ) {
	continue;
      }
      auto mand_cond = analyzer.extract_condition(fault);
      fault_info(fault).set_mandatory_condition(mand_cond);
      if ( fault_info(fault).is_trivial() ) {
	++ nt;
      }
    }
  }

  if ( debug ) {
    timer.stop();
    cout << "# of Trivial Condition Faults:         " << nt << endl;
    cout << "CPU time:                              " << timer.get_time() << endl;
  }
}

// @brief trivial な故障間の支配関係のチェックを行う．
void
FaultInfoMgr::trivial_reduction1(
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
  vector<bool> mark(mNetwork.max_fault_id(), false);
  for ( auto fault1: mActiveFaultList ) {
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

  TrivialChecker1 checker{mNetwork, tmp_fault_list, option};

  SizeType check_num = 0;
  SizeType success_num = 0;
  for ( auto fault1: mActiveFaultList ) {
    if ( is_deleted(fault1) || !is_trivial(fault1) ) {
      continue;
    }
    auto cond1 = fault_info(fault1).mandatory_condition();
    for ( auto fault2: dom_cand_list(fault1) ) {
      if ( is_deleted(fault2) || !is_trivial(fault2) ) {
	continue;
      }
      if ( !check_intersect(fault1, fault2) ) {
	continue;
      }
      auto cond2 = fault_info(fault2).mandatory_condition();
      ++ check_num;
      if ( checker.check(cond1, cond2) ) {
	set_deleted(fault2);
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
FaultInfoMgr::trivial_reduction2(
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
    vector<bool> f2_mark(mNetwork.max_fault_id(), false);
    for ( auto fault1: fault_list(ffr1) ) {
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
    TrivialChecker2 checker{mNetwork, ffr1, fault2_list, option};
    for ( auto fault1: fault_list(ffr1) ) {
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
	auto cond2 = fault_info(fault2).mandatory_condition();
	++ check_num;
	if ( checker.check(fault1, fault2, cond2) ) {
	  set_deleted(fault2);
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
FaultInfoMgr::trivial_reduction3(
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

  SizeType nf = mActiveFaultList.size();
  SizeType N = 50;
  for ( SizeType start_pos = 0; start_pos < nf; start_pos += N ) {
    SizeType end_pos = std::min(start_pos + N, nf);
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
    for ( SizeType i = start_pos; i < end_pos; ++ i ) {
      auto fault1 = mActiveFaultList[i];
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
    TrivialChecker1 checker1{mNetwork, tmp_list, option};
    for ( auto ffr2: ffr2_list ) {
      ++ dom2_num;
      TrivialChecker3 checker2{mNetwork, fault1_list, ffr2, option};
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
FaultInfoMgr::global_reduction(
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
    vector<bool> fault2_mark(mNetwork.max_fault_id(), false);
    // ffr1 に含まれる故障の被支配故障の候補が属するFFRを求める．
    vector<const TpgFFR*> ffr2_list;
    unordered_set<SizeType> ffr2_mark;
    // 故障番号とFFR番号のペアをキーにして故障のリストを保持する辞書
    unordered_map<Key, vector<const TpgFault*>> fault2_list_map;
    for ( auto fault1: fault_list(ffr1) ) {
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
    SimpleDomChecker checker1{mNetwork, ffr1, fault2_list, option};
    for ( auto ffr2: ffr2_list ) {
      ++ dom2_num;
      DomChecker checker2{mNetwork, ffr1, ffr2, option};
      for ( auto fault1: fault_list(ffr1) ) {
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
FaultInfoMgr::check_intersect(
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
FaultInfoMgr::check_intersect(
  const TpgFault* fault1,
  const TpgFault* fault2
)
{
  return check_intersect(mNetwork.ffr(fault1), mNetwork.ffr(fault2));
}

// @brief 2つの故障が共通部分を持つか調べる．
bool
FaultInfoMgr::check_intersect(
  const TpgFault* fault1,
  const TpgFFR* ffr2
)
{
  return check_intersect(mNetwork.ffr(fault1), ffr2);
}

END_NAMESPACE_DRUID
