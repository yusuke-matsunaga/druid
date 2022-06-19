
/// @file FaultReducer.cc
/// @brief FaultReducer の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "FaultReducer.h"
#include "DtpgFFR.h"
#include "UndetChecker.h"
#include "DomChecker.h"
#include "TpgFFR.h"
#include "TpgFault.h"
#include "NodeValList.h"
#include "MatrixGen.h"
#include "ym/Range.h"
#include <random>


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// オプション文字列をパーズする
// * オプションは
//   <オプションキーワード>[:<オプション値>][, <オプションキーワード>[:<オプション値>]]
//   の形式と仮定する．
// * 空白の除去は行わない．単純に ',' と ':' のみで区切る．
// * 結果を opt_list に格納する．
void
parse_option(
  const string& option_str,
  vector<pair<string, string>>& opt_list
)
{
  // ',' で区切る
  string tmp_str(option_str);
  for ( ; ; ) {
    string::size_type p = tmp_str.find_first_of(',');
    string tmp = tmp_str.substr(0, p);
    // tmp を ':' で区切る．
    string::size_type q = tmp.find_first_of(':');
    if ( q == string::npos ) {
      // ':' がなかった
      opt_list.push_back(make_pair(tmp, string()));
    }
    else {
      string l_str = tmp.substr(0, q);
      string r_str = tmp.substr(q + 1, string::npos);
      opt_list.push_back(make_pair(l_str, r_str));
    }
    if ( p == string::npos ) {
      // 末尾だったので終わる．
      break;
    }
    // tmp_str を切り詰める．
    tmp_str = tmp_str.substr(p + 1, string::npos);
  }
}

END_NONAMESPACE

// @brief コンストラクタ
FaultReducer::FaultReducer(
  const TpgNetwork& network,
  FaultType fault_type
) : mNetwork{network},
    mFaultType{fault_type},
    mDebug{false}
{
  mFsim.init_fsim3(mNetwork, mFaultType);
}

// @brief デストラクタ
FaultReducer::~FaultReducer()
{
}

// @breif 内部で用いる SAT ソルバのタイプの設定を行う．
void
FaultReducer::set_solver_type(
  const SatSolverType& solver_type
)
{
  mSolverType = solver_type;
}

// @brief デバッグフラグをセットする．
void
FaultReducer::set_debug(
  bool debug
)
{
  mDebug = debug;
}

// @brief 故障の支配関係を調べて故障リストを縮約する．
void
FaultReducer::fault_reduction(
  vector<const TpgFault*>& fault_list,
  const string& algorithm
)
{
  // algorithm を解析する．
  vector<pair<string, string>> opt_list;
  parse_option(algorithm, opt_list);

  // 必要条件を求めるか否か
  bool need_mand_cond = false;
  for ( auto opt_pair: opt_list ) {
    auto alg = opt_pair.first;
    auto opt = opt_pair.second;
    if ( (alg == "red1" || alg == "red3") && opt == "" ) {
      need_mand_cond = true;
    }
  }

  // 初期化する．
  init(fault_list, need_mand_cond);

  make_dom_candidate(1);

  ffr_reduction();

  // algorithm に従って縮約を行う．
  for ( auto opt_pair: opt_list ) {
    auto alg = opt_pair.first;
    auto opt = opt_pair.second;
    bool simple = (opt == "simple");
    if ( alg == "red1" ) {
      dom_reduction1(simple);
    }
    else if ( alg == "red2" ) {
      dom_reduction2();
    }
    else if ( alg == "red3" ) {
      dom_reduction3(simple);
    }
  }

  // mDeleted のついていない故障を fault_list に入れる．
  fault_list.clear();
  for ( auto fault: mFaultList ) {
    if ( !mFaultInfoArray[fault->id()].mDeleted ) {
      fault_list.push_back(fault);
    }
  }
}

// @brief 内部のデータ構造を初期化する．
void
FaultReducer::init(
  const vector<const TpgFault*>& fault_list,
  bool need_mand_cond
)
{
  if ( mDebug ) {
    mTimer.reset();
    mTimer.start();
  }

  mFsim.set_skip_all();

  // mFaultInfoArray を初期化する．
  // fault_list に含まれる故障の mDelted だけ false にする．
  mFaultInfoArray.clear();
  mFaultInfoArray.resize(mNetwork.max_fault_id());
  for ( auto id: Range(mNetwork.max_fault_id()) ) {
    auto& fi = mFaultInfoArray[id];
    fi.mDeleted = true;
    fi.mPat = 0UL;
    fi.mDetCount = 0;
  }

  mFaultList.clear();
  mFaultList.reserve(fault_list.size());
  for ( auto fault: fault_list ) {
    mFaultList.push_back(fault);
    mFaultInfoArray[fault->id()].mDeleted = false;
    mFsim.clear_skip(fault);
  }

  // 各々の故障のテストベクタをもとめる(故障シミュレーション用)
  std::mt19937 rg;
  for ( auto& ffr: mNetwork.ffr_list() ) {
    string just_type;
    DtpgFFR dtpg(mNetwork, mFaultType, ffr, just_type);
    for ( auto fault: ffr.fault_list() ) {
      auto& fi = mFaultInfoArray[fault->id()];
      if ( !fi.mDeleted ) {
	fi.mFFRCond = fault->ffr_propagate_condition(mFaultType);
	auto assumptions = dtpg.conv_to_literal_list(fi.mFFRCond);
	SatBool3 sat_res = dtpg.check(assumptions);
	ASSERT_COND( sat_res == SatBool3::True );
	TestVector tv = dtpg.get_tv();
	tv.fix_x_from_random(rg);
	mTvList.push_back(tv);
	if ( need_mand_cond ) {
	  // 十分条件(の一つ)を求める．
	  NodeValList suff_cond = dtpg.get_sufficient_condition(ffr.root());
	  fi.mSuffCond = suff_cond;
	  // 必要条件を求める．
	  // 明らかに必要条件は十分条件の部分集合になっている．
	  // さらに FFR 内の伝搬条件は常に必要条件となっている．
	  // のこりの条件をひとつづつ取り出して反転したものを
	  // 加えてSATを解く．もしもUNSATになったらそれは必要条件となる．
	  suff_cond.diff(fi.mFFRCond);
	  NodeValList mand_cond = fi.mFFRCond;
	  for ( auto nv: suff_cond ) {
	    vector<SatLiteral> assumptions1(assumptions);
	    SatLiteral lit = dtpg.conv_to_literal(nv);
	    assumptions1.push_back(~lit);
	    SatBool3 res = dtpg.check(assumptions1);
	    if ( res == SatBool3::False ) {
	      mand_cond.add(nv);
	    }
	  }
	  fi.mMandCond = mand_cond;
	}
      }
    }
  }

  if ( mDebug ) {
    mTimer.stop();
    cout << "TestVector generation" << endl;
    cout << "CPU time:                              " << mTimer.get_time() << endl;
  }
}

// @brief 故障シミュレーションを行って支配故障の候補を作る．
void
FaultReducer::make_dom_candidate(
  int loop_limit
)
{
  if ( mDebug ) {
    mTimer.reset();
    mTimer.start();
  }

  mFsim.clear_patterns();
  int wpos = 0;
  for ( auto tv: mTvList ) {
    mFsim.set_pattern(wpos, tv);
    ++ wpos;
    if ( wpos == kPvBitLen ) {
      do_fsim();
      mFsim.clear_patterns();
      wpos = 0;
    }
  }
  if ( wpos > 0 ) {
    do_fsim();
  }
  // mTvList を空にする．
  vector<TestVector>().swap(mTvList);

  std::mt19937 rg;
  TestVector tv(mNetwork.input_num(), mNetwork.dff_num(), mFaultType);
  for ( int nc_count = 0; nc_count < loop_limit; ) {
    for ( auto i: Range(kPvBitLen) ) {
      tv.set_from_random(rg);
      mFsim.set_pattern(i, tv);
    }
    if ( do_fsim() ) {
      nc_count = 0;
    }
    else {
      ++ nc_count;
    }
  }

  if ( mDebug ) {
    mTimer.stop();
    cout << "Fault Simulation" << endl;
    cout << "CPU time:                              " << mTimer.get_time() << endl;
  }
}

// @brief 故障シミュレーションを行って支配故障の候補を作る．
bool
FaultReducer::do_fsim()
{
  bool changed = false;
  int n = mFsim.ppsfp();

  // mPat の設定
  bool first_time = false;
  for ( auto i: Range(n) ) {
    auto fault = mFsim.det_fault(i);
    auto pat = mFsim.det_fault_pat(i);
    auto& fi = mFaultInfoArray[fault->id()];
    fi.mPat = pat;
    if ( fi.mDetCount == 0 ) {
      first_time = true;
    }
  }

  vector<const TpgFault*> tmp_list;
  if ( first_time ) {
    tmp_list.reserve(n);
  }
  for ( auto i1: Range(n) ) {
    auto fault1 = mFsim.det_fault(i1);
    auto pat1 = mFsim.det_fault_pat(i1);
    auto& fi1 = mFaultInfoArray[fault1->id()];
    if ( fi1.mDetCount == 0 ) {
      // pat1 を含むパタンを持っている故障を tmp_list に入れる．
      tmp_list.clear();
      for ( auto i2: Range(n) ) {
	if ( i2 == i1 ) {
	  continue;
	}
	auto fault2 = mFsim.det_fault(i2);
	auto pat2 = mFsim.det_fault_pat(i2);
	if ( (pat1 & pat2) == pat1 ) {
	  // fault1 を検出しているパタンはすべて fault2 を検出している．
	  tmp_list.push_back(fault2);
	}
      }
      // tmp_list を mDomCandList にコピーする．
      fi1.mDomCandList = tmp_list;
    }
    else {
      // mDomCandList の要素のうち，pat1 を含むもののみを残す．
      int rpos1 = 0;
      int n1 = fi1.mDomCandList.size();
      int wpos1 = 0;
      for ( ; rpos1 < n1; ++ rpos1 ) {
	auto fault2 = fi1.mDomCandList[rpos1];
	auto pat2 = mFaultInfoArray[fault2->id()].mPat;
	if ( (pat1 & pat2) == pat1 ) {
	  fi1.mDomCandList[wpos1] = fault2;
	  ++ wpos1;
	}
      }
      if ( wpos1 < n1 ) {
	vector<const TpgFault*> tmp_list(wpos1);
	for ( auto i: Range(wpos1) ) {
	  tmp_list[i] = fi1.mDomCandList[i];
	}
	fi1.mDomCandList.swap(tmp_list);
	changed = true;
      }
    }
    fi1.mDetCount += count_ones(pat1);
  }

  // mPat のクリア
  for ( auto i: Range(n) ) {
    auto fault = mFsim.det_fault(i);
    mFaultInfoArray[fault->id()].mPat = 0UL;
  }

  return changed;
}

// @brief 同一 FFR 内の支配故障のチェックを行う．
void
FaultReducer::ffr_reduction()
{
  if ( mDebug ) {
    cout << "# of initial faults:                   " << mFaultList.size() << endl;
    mTimer.reset();
    mTimer.start();
  }

  for ( auto& ffr: mNetwork.ffr_list() ) {
    // FFR ごとに検出可能な故障をもとめる．
    vector<const TpgFault*> tmp_fault_list;
    for ( auto fault: ffr.fault_list() ) {
      if ( !mFaultInfoArray[fault->id()].mDeleted ) {
	tmp_fault_list.push_back(fault);
      }
    }
    if ( tmp_fault_list.empty() ) {
      // 対象の故障がなかった．
      continue;
    }

    string just_type;
    DtpgFFR dtpg(mNetwork, mFaultType, ffr, just_type);

    // 支配関係を調べ，代表故障のみを残す．
    int nf = tmp_fault_list.size();
    for ( auto i1: Range(nf) ) {
      auto fault1 = tmp_fault_list[i1];
      auto& fi1 = mFaultInfoArray[fault1->id()];
      if ( fi1.mDeleted ) {
	continue;
      }
      auto assumptions = dtpg.conv_to_literal_list(fi1.mFFRCond);
      for ( auto fault2: fi1.mDomCandList ) {
	if ( fault2->tpg_onode()->ffr_root() != fault1->tpg_onode()->ffr_root() ) {
	  continue;
	}
	auto& fi2 = mFaultInfoArray[fault2->id()];
	if ( fi2.mDeleted ) {
	  continue;
	}
	NodeValList ffr_cond2 = fi2.mFFRCond;
	ffr_cond2.diff(fi1.mFFRCond);
	bool unsat = true;
	vector<SatLiteral> assumptions1(assumptions);
	// プレースホルダ
	assumptions1.push_back(kSatLiteralX);
	for ( auto nv: ffr_cond2 ) {
	  SatLiteral lit1 = dtpg.conv_to_literal(nv);
	  assumptions1[assumptions.size()] = ~lit1;
	  if ( dtpg.check(assumptions1) == SatBool3::True ) {
	    unsat = false;
	    break;
	  }
	}
	if ( unsat ) {
	  // fault1 を検出する条件のもとでは fault2 も検出される．
	  // → fault2 は支配されている．
	  fi2.mDeleted = true;
	  // 不要となったベクタの領域を解放するハックコード
	  vector<const TpgFault*>().swap(fi2.mDomCandList);
	}
      }
    }
  }

  if ( mDebug ) {
    mTimer.stop();
    int n = count_faults();
    cout << "after FFR dominance reduction:         " << n << endl;
    cout << "CPU time:                              " << mTimer.get_time() << endl;
  }
}

// @brief 異なる FFR 間の支配故障の簡易チェックを行う．
void
FaultReducer::dom_reduction1(
  bool simple
)
{
  if ( mDebug ) {
    mTimer.reset();
    mTimer.start();
  }

  int check_num = 0;
  int success_num = 0;
  for ( auto fault1: mFaultList ) {
    auto& fi1 = mFaultInfoArray[fault1->id()];
    if ( fi1.mDeleted ) {
      continue;
    }
    UndetChecker undet_checker(mNetwork, mFaultType, fault1, mSolverType);
    for ( auto fault2: mFaultList ) {
      auto& fi2 = mFaultInfoArray[fault2->id()];
      if ( fault2 == fault1 || fi2.mDeleted ) {
	continue;
      }
      if ( fault1->tpg_onode()->ffr_root() == fault2->tpg_onode()->ffr_root() ) {
	// 同じ FFR ならチェック済み
	continue;
      }

      if ( !undet_checker.has_gvar(fault2->tpg_onode()) ) {
	continue;
      }
      // fault1 が fault2 の mDomCandList に含まれるか調べる．
      bool found = false;
      for ( auto fault3: fi2.mDomCandList ) {
	if ( fault3 == fault1 ) {
	  found = true;
	  break;
	}
      }
      if ( found ) {
	++ check_num;
	const auto& cond = simple ? fi2.mFFRCond : fi2.mMandCond;
	SatBool3 res = undet_checker.check(cond);
	if ( res == SatBool3::False ) {
	  ++ success_num;
	  // fault2 が検出可能の条件のもとで fault1 が検出不能となることはない．
	  // fault2 が fault1 を支配している．
	  fi1.mDeleted = true;
	  // 不要となったベクタの領域を解放するハックコード
	  vector<const TpgFault*>().swap(fi1.mDomCandList);
	  break;
	}
      }
    }
  }

  if ( mDebug ) {
    mTimer.stop();
    int n = count_faults();
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

  int check_num = 0;
  int dom_num = 0;
  int success_num = 0;
  for ( auto fault1: mFaultList ) {
    auto& fi1 = mFaultInfoArray[fault1->id()];
    if ( fi1.mDeleted ) {
      continue;
    }
    for ( auto& ffr2: mNetwork.ffr_list() ) {
      if ( ffr2.root() == fault1->tpg_onode()->ffr_root() ) {
	continue;
      }
      vector<const TpgFault*> fault2_list;
      for ( auto fault2: ffr2.fault_list() ) {
	auto& fi2 = mFaultInfoArray[fault2->id()];
	if ( fi2.mDeleted ) {
	  continue;
	}
	bool found = false;
	for ( auto fault3: fi2.mDomCandList ) {
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
      DomChecker dom_checker(mNetwork, mFaultType, ffr2.root(), fault1, mSolverType);
      for ( auto fault2: fault2_list ) {
	++ check_num;
	SatBool3 res = dom_checker.check_detectable(fault2);
	if ( res == SatBool3::False ) {
	  ++ success_num;
	  // fault2 が検出可能の条件のもとで fault1 が検出不能となることはない．
	  // fault2 が fault1 を支配している．
	  fi1.mDeleted = true;
	  // 不要となったベクタの領域を解放するハックコード
	  vector<const TpgFault*>().swap(fi1.mDomCandList);
	  break;
	}
      }
      if ( fi1.mDeleted ) {
	break;
      }
    }
  }

  if ( mDebug ) {
    mTimer.stop();
    int n = count_faults();
    cout << "after global dominance reduction:      " << n << endl;
    cout << "    # of total checkes:                " << check_num << endl
	 << "    # of total successes:              " << success_num << endl
	 << "    # of DomCheckers:                  " << dom_num << endl
	 << "CPU time:                              " << mTimer.get_time() << endl;
  }
}

// @brief 異なる FFR 間の支配故障の簡易チェックを行う．
void
FaultReducer::dom_reduction3(
  bool simple
)
{
  if ( mDebug ) {
    mTimer.reset();
    mTimer.start();
  }

  int check_num = 0;
  int dom_num = 0;
  int success_num = 0;
  int u_check_num = 0;
  int u_success_num = 0;
  for ( auto fault1: mFaultList ) {
    auto& fi1 = mFaultInfoArray[fault1->id()];
    if ( fi1.mDeleted ) {
      continue;
    }
    UndetChecker undet_checker(mNetwork, mFaultType, fault1, mSolverType);
    for ( auto& ffr2: mNetwork.ffr_list() ) {
      if ( ffr2.root() == fault1->tpg_onode()->ffr_root() ) {
	continue;
      }
      vector<const TpgFault*> fault2_list;
      for ( auto fault2: ffr2.fault_list() ) {
	auto& fi2 = mFaultInfoArray[fault2->id()];
	if ( fi2.mDeleted ) {
	  continue;
	}
	bool found = false;
	for ( auto fault3: fi2.mDomCandList ) {
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
	if ( undet_checker.has_gvar(fault2->tpg_onode()) ) {
	  ++ u_check_num;
	  auto& fi2 = mFaultInfoArray[fault2->id()];
	  const auto& cond = simple ? fi2.mFFRCond : fi2.mMandCond;
	  SatBool3 res = undet_checker.check(cond);
	  if ( res == SatBool3::False ) {
	    ++ u_success_num;
	    // fault2 が検出可能の条件のもとで fault1 が検出不能となることはない．
	    // fault2 が fault1 を支配している．
	    fi1.mDeleted = true;
	    // 不要となったベクタの領域を解放するハックコード
	    vector<const TpgFault*>().swap(fi1.mDomCandList);
	    break;
	  }
	}
      }
      if ( fi1.mDeleted ) {
	break;
      }

      ++ dom_num;
      DomChecker dom_checker(mNetwork, mFaultType, ffr2.root(), fault1, mSolverType);
      for ( auto fault2: fault2_list ) {
	++ check_num;
	SatBool3 res = dom_checker.check_detectable(fault2);
	if ( res == SatBool3::False ) {
	  ++ success_num;
	  // fault2 が検出可能の条件のもとで fault1 が検出不能となることはない．
	  // fault2 が fault1 を支配している．
	  fi1.mDeleted = true;
	  // 不要となったベクタの領域を解放するハックコード
	  vector<const TpgFault*>().swap(fi1.mDomCandList);
	  break;
	}
      }
      if ( fi1.mDeleted ) {
	break;
      }
    }
  }

  if ( mDebug ) {
    mTimer.stop();
    int n = count_faults();
    cout << "after global dominance reduction:      " << n << endl;
    cout << "    # of total checkes(1):             " << check_num << endl
	 << "    # of total successes(1):           " << success_num << endl
	 << "    # of total checkes(2):             " << u_check_num << endl
	 << "    # of total successes(2):           " << u_success_num << endl
	 << "    # of DomCheckers:                  " << dom_num << endl
	 << "CPU time:                              " << mTimer.get_time() << endl;
  }
}

// @brief mFaultList 中の mDeleted マークが付いていない故障数を数える．
int
FaultReducer::count_faults() const
{
  int n = 0;
  for ( auto fault: mFaultList ) {
    if ( !mFaultInfoArray[fault->id()].mDeleted ) {
      ++ n;
    }
  }
  return n;
}

END_NAMESPACE_DRUID
