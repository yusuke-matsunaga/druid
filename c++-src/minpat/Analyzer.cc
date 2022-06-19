
/// @file Analyzer.cc
/// @brief Analyzer の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 2022 Yusuke Matsunaga
/// All rights reserved.

#include "Analyzer.h"
#include "FaultInfo.h"
#include "DtpgFFR.h"
#include "DtpgFFR2.h"
#include "UndetChecker.h"
#include "DomChecker.h"
#include "TpgFFR.h"
#include "TpgFault.h"
#include "MatrixGen.h"
#include "ym/McMatrix.h"
#include "ym/Range.h"
#include <random>


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

bool debug = false;

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
Analyzer::Analyzer(
  const TpgNetwork& network,
  FaultType fault_type
) : mNetwork{network},
    mFaultType{fault_type}
{
}

int nex_num;

// @brief デストラクタ
Analyzer::~Analyzer()
{
}

// @brief 故障の支配関係を調べて故障リストを縮約する．
void
Analyzer::fault_reduction(
  vector<const TpgFault*>& fault_list,
  const string& algorithm
)
{
  // 実は fault_list は参考程度にしか使わない．
  vector<bool> mark(mNetwork.max_fault_id(), false);
  for ( auto fault: fault_list ) {
    mark[fault->id()] = true;
  }
  auto fi_list = gen_fault_list(mark);

  vector<pair<string, string>> opt_list;
  parse_option(algorithm, opt_list);

  for ( auto opt_pair: opt_list ) {
    auto alg = opt_pair.first;
    auto opt = opt_pair.second;
    if ( alg == "red1" ) {
      dom_reduction1(fi_list);
    }
    else if ( alg == "red2" ) {
      dom_reduction2(fi_list);
    }
  }

  fault_list.clear();
  for ( auto fi: fi_list ) {
    fault_list.push_back(fi->fault());
  }
}

// @brief 検出可能故障リストを作る．
vector<FaultInfo*>
Analyzer::gen_fault_list(
  const vector<bool>& mark
)
{
  string just_type;

  vector<FaultInfo*> fi_list;

  std::mt19937 randgen;
  int n0 = 0;
  int n1 = 0;
  for ( auto& ffr: mNetwork.ffr_list() ) {
    // FFR ごとに検出可能な故障をもとめる．
    DtpgFFR dtpg{mNetwork, mFaultType, ffr, just_type};
    vector<FaultInfo*> tmp_fi_list;
    for ( auto fault: ffr.fault_list() ) {
      if ( !mark[fault->id()] ) {
	continue;
      }
      auto ffr_cond = fault->ffr_propagate_condition(mFaultType);
      auto assumptions = dtpg.conv_to_literal_list(ffr_cond);
      SatBool3 sat_res = dtpg.check(assumptions);
      if ( sat_res == SatBool3::True ) {
	auto testvect = dtpg.backtrace(fault->tpg_onode()->ffr_root(), ffr_cond);
	testvect.fix_x_from_random(randgen);
	auto suf_cond = dtpg.get_sufficient_condition(fault->tpg_onode()->ffr_root());
	FaultInfo* fi = new FaultInfo(fault, ffr_cond, suf_cond, testvect);
	tmp_fi_list.push_back(fi);
	++ n0;
      }
    }

    // 支配関係を調べ，代表故障のみを残す．
    SizeType nf = tmp_fi_list.size();
    vector<bool> mark(nf, true);
    for ( auto i1: Range(nf) ) {
      if ( !mark[i1] ) {
	continue;
      }
      auto fi1 = tmp_fi_list[i1];
      auto fault1 = fi1->fault();
      const NodeValList& ffr_cond1 = fi1->mand_cond();
      // ffr_cond1 を否定した節を加える．
      // 制御変数は clit1
      SatLiteral clit1 = dtpg.new_variable();
      vector<SatLiteral> tmp_lits;
      tmp_lits.reserve(ffr_cond1.size() + 1);
      tmp_lits.push_back(~clit1);
      for ( auto nv: ffr_cond1 ) {
	SatLiteral lit1 = dtpg.conv_to_literal(nv);
	tmp_lits.push_back(~lit1);
      }
      dtpg.add_clause(tmp_lits);

      for ( auto i2: Range(nf) ) {
	if ( i2 == i1 || !mark[i2] ) {
	  continue;
	}
	auto fi2 = tmp_fi_list[i2];
	auto fault2 = fi2->fault();
	const NodeValList& ffr_cond2 = fi2->mand_cond();
	auto assumptions = dtpg.conv_to_literal_list(ffr_cond2);
	assumptions.push_back(clit1);
	SatBool3 sat_res = dtpg.check(assumptions);
	if ( sat_res == SatBool3::False ) {
	  // fault2 を検出する条件のもとで fault1 を検出しない
	  // 割り当てが存在しない．→ fault1 は支配されている．
	  mark[i1] = false;
	  break;
	}
      }
    }
    for ( auto i: Range(nf) ) {
      if ( mark[i] ) {
	auto fi = tmp_fi_list[i];
	fi_list.push_back(fi);
	++ n1;
      }
    }
  }
  if ( debug ) {
    cout << "# of initial faults: " << n0 << endl
	 << "after FFR dominance reduction: " << n1 << endl;
  }

  return fi_list;
}

// @brief 異なる FFR 間の支配故障の簡易チェックを行う．
void
Analyzer::dom_reduction1(
  vector<FaultInfo*>& fi_list
)
{
  Timer timer;
  timer.start();

  SizeType nf = fi_list.size();
  vector<const TpgFault*> fault_list;
  fault_list.reserve(nf);
  vector<TestVector> tv_list;
  tv_list.reserve(nf);
  for ( auto fi: fi_list ) {
    fault_list.push_back(fi->fault());
    tv_list.push_back(fi->testvect());
  }
  MatrixGen matgen(fault_list, tv_list, mNetwork, mFaultType);
  McMatrix matrix = matgen.generate();

  string just_type;

  int check_num = 0;
  int success_num = 0;
  //int checker_num = 0;

  vector<bool> mark(nf, false);
  for ( int i1: Range(0, nf) ) {
    auto fi1 = fi_list[i1];
    auto fault1 = fi1->fault();
    UndetChecker undet_checker(mNetwork, mFaultType, fault1);

    // i2 が i1 を支配している時
    // i2 に含まれる列は必ず i1 にも含まれなければならない．
    vector<bool> col_mark(nf, false);
    for ( auto col: matrix.row_list(i1) ) {
      col_mark[col] = true;
    }
    for ( auto i2: Range(nf) ) {
      if ( i2 == i1 || mark[i2] ) {
	continue;
      }
      bool not_covered = false;
      for ( auto col: matrix.row_list(i2) ) {
	if ( !col_mark[col] ) {
	  not_covered = true;
	  break;
	}
      }
      if ( not_covered ) {
	continue;
      }
      auto fi2 = fi_list[i2];
      auto fault2 = fi2->fault();
      if ( fault1->tpg_onode()->ffr_root() == fault2->tpg_onode()->ffr_root() ) {
	// 同じ FFR ならチェック済み
	continue;
      }

      ++ check_num;
      NodeValList ffr_cond = fault2->ffr_propagate_condition(mFaultType);
      SatBool3 res = undet_checker.check(ffr_cond);
      if ( res == SatBool3::False ) {
	++ success_num;
	// fault2 が検出可能の条件のもとで fault が検出不能となることはない．
	// fault2 が fault を支配している．
	mark[i1] = true;
	break;
      }
    }
  }

  int rpos = 0;
  int wpos = 0;
  for ( ; rpos < nf; ++ rpos ) {
    if ( !mark[rpos] ) {
      fi_list[wpos] = fi_list[rpos];
      ++ wpos;
    }
  }
  if ( wpos < nf ) {
    fi_list.erase(fi_list.begin() + wpos, fi_list.end());
  }

  timer.stop();
  if ( debug ) {
    cout << "after semi-global dominance reduction: " << wpos << endl
	 << "# of total checks:                     " << check_num << endl
	 << "# of total successes:                  " << success_num << endl
	 << "CPU time:                              " << timer.get_time() << endl;
  }
}

// @brief 異なる FFR 間の支配故障の簡易チェックを行う．
void
Analyzer::dom_reduction2(
  vector<FaultInfo*>& fi_list
)
{
  Timer timer;
  timer.start();

  int nf = fi_list.size();
  vector<const TpgFault*> fault_list;
  fault_list.reserve(nf);
  vector<TestVector> tv_list;
  tv_list.reserve(nf);
  unordered_map<int, int> fid_map;
  for ( auto fi: fi_list ) {
    int row = fault_list.size();
    auto fault = fi->fault();
    fid_map.emplace(fault->id(), row);
    fault_list.push_back(fault);
    tv_list.push_back(fi->testvect());
  }
  MatrixGen matgen(fault_list, tv_list, mNetwork, mFaultType);
  McMatrix matrix = matgen.generate();

  int check_num = 0;
  int dom_num = 0;
  int success_num = 0;
  int n = fi_list.size();
  vector<bool> mark(mNetwork.max_fault_id(), false);
  for ( auto fi: fi_list ) {
    auto fault = fi->fault();
    mark[fault->id()] = true;
  }
  for ( int i1: Range(0, n) ) {
    auto fi1 = fi_list[i1];
    auto fault1 = fi1->fault();
    // fault2 が fault1 を支配している時
    // fault2 に含まれる列は必ず fault1 にも含まれなければならない．
    vector<bool> col_mark(nf, false);
    for ( auto col: matrix.row_list(i1) ) {
      col_mark[col] = true;
    }
    for ( auto& ffr2: mNetwork.ffr_list() ) {
      if ( ffr2.root() == fault1->tpg_onode()->ffr_root() ) {
	continue;
      }
      vector<const TpgFault*> fault2_list;
      for ( auto fault2: ffr2.fault_list() ) {
	if ( !mark[fault2->id()] ) {
	  continue;
	}

	bool not_covered = false;
	int i2 = fid_map.at(fault2->id());
	for ( auto col: matrix.row_list(i2) ) {
	  if ( !col_mark[col] ) {
	    not_covered = true;
	    break;
	  }
	}
	if ( not_covered ) {
	  continue;
	}
	fault2_list.push_back(fault2);
      }
      if ( fault2_list.empty() ) {
	continue;
      }
      ++ dom_num;
      DomChecker dom_checker(mNetwork, mFaultType, ffr2.root(), fault1);
      for ( auto fault2: fault2_list ) {
	++ check_num;
	SatBool3 res = dom_checker.check_detectable(fault2);
	if ( res == SatBool3::False ) {
	  ++ success_num;
	  // fault2 が検出可能の条件のもとで fault1 が検出不能となることはない．
	  // fault2 が fault1 を支配している．
	  mark[fault1->id()] = false;
	  break;
	}
      }
      if ( !mark[fault1->id()] ) {
	break;
      }
    }
  }
  int rpos = 0;
  int wpos = 0;
  for ( ; rpos < n; ++ rpos ) {
    auto fi = fi_list[rpos];
    auto fault = fi->fault();
    if ( mark[fault->id()] ) {
      fi_list[wpos] = fi;
      ++ wpos;
    }
  }
  if ( wpos < n ) {
    fi_list.erase(fi_list.begin() + wpos, fi_list.end());
  }
  timer.stop();
  if ( debug ) {
    cout << "after global dominance reduction: " << fi_list.size() << endl;
    cout << "# of total checkes:   " << check_num << endl
	 << "# of total successes: " << success_num << endl
	 << "# of DomCheckers:     " << dom_num << endl
	 << "CPU time:             " << timer.get_time() << endl;
  }
}

// @brief 初期化する
void
Analyzer::init(
  int loop_limit
)
{
  string just_type;

  nex_num = 0;
  vector<bool> mark(mNetwork.max_fault_id(), false);
  vector<NodeValList> ffr_cond_array(mNetwork.max_fault_id());
  vector<FaultInfo*> tmp_fi_map(mNetwork.max_fault_id(), nullptr);
  int n1 = 0;
  for ( auto& ffr: mNetwork.ffr_list() ) {
    // FFR ごとに検出可能な故障をもとめる．
    DtpgFFR dtpg(mNetwork, mFaultType, ffr, just_type);
    vector<const TpgFault*> fault_list;
    vector<NodeValList> ffr_cond_list;
    vector<NodeValList> suf_cond_list;
    for ( auto fault: ffr.fault_list() ) {
      NodeValList ffr_cond = fault->ffr_propagate_condition(mFaultType);
      auto assumptions = dtpg.conv_to_literal_list(ffr_cond);
      SatBool3 sat_res = dtpg.check(assumptions);
      if ( sat_res == SatBool3::True ) {
	fault_list.push_back(fault);
	ffr_cond_list.push_back(ffr_cond);
	ffr_cond_array[fault->id()] = ffr_cond;
	NodeValList suf_cond = dtpg.get_sufficient_condition(ffr.root());
	suf_cond.merge(ffr_cond);
	suf_cond_list.push_back(suf_cond);
	mark[fault->id()] = true;
      }
    }
    // 支配関係を調べ，代表故障のみを残す．
    SizeType nf = fault_list.size();
    for ( auto i1: Range(nf) ) {
      auto fault1 = fault_list[i1];
      if ( !mark[fault1->id()] ) {
	continue;
      }
      const NodeValList& ffr_cond1 = ffr_cond_list[i1];
      // ffr_cond1 を否定した節を加える．
      // 制御変数は clit1
      auto clit1 = dtpg.new_variable();
      vector<SatLiteral> tmp_lits;
      tmp_lits.reserve(ffr_cond1.size() + 1);
      tmp_lits.push_back(~clit1);
      for ( auto nv: ffr_cond1 ) {
	SatLiteral lit1 = dtpg.conv_to_literal(nv);
	tmp_lits.push_back(~lit1);
      }
      dtpg.add_clause(tmp_lits);
      for ( auto i2: Range(nf) ) {
	if ( i2 == i1 ) {
	  continue;
	}
	auto fault2 = fault_list[i2];
	if ( !mark[fault2->id()] ) {
	  continue;
	}
	const NodeValList& ffr_cond2 = ffr_cond_list[i2];
	auto assumptions = dtpg.conv_to_literal_list(ffr_cond2);
	assumptions.push_back(clit1);
	SatBool3 sat_res = dtpg.check(assumptions);
	if ( sat_res == SatBool3::False ) {
	  // fault2 を検出する条件のもとで fault1 を検出しない
	  // 割り当てが存在しない．→ fault1 は支配されている．
	  mark[fault1->id()] = false;
	  break;
	}
      }
    }

    for ( auto i: Range(nf) ) {
      auto fault = fault_list[i];
      if ( mark[fault->id()] ) {
	auto fi = analyze_fault(dtpg, fault, loop_limit);
	ASSERT_COND ( fi != nullptr );
	tmp_fi_map[fault->id()] = fi;
	++ n1;
      }
    }
  }
  if ( debug ) {
    cout << "# of initial faults: " << mNetwork.rep_fault_num() << endl
	 << "after FFR dominance reduction: " << n1 << endl;
  }

  for ( auto& ffr: mNetwork.ffr_list() ) {
    for ( auto fault: ffr.fault_list() ) {
      if ( !mark[fault->id()] ) {
	continue;
      }
      UndetChecker undet_checker(mNetwork, mFaultType, fault);
      for ( auto& ffr2: mNetwork.ffr_list() ) {
	if ( &ffr == &ffr2 ) {
	  continue;
	}
	for ( auto fault2: ffr2.fault_list() ) {
	  if ( !mark[fault2->id()] ) {
	    continue;
	  }
	  auto fi2 = tmp_fi_map[fault2->id()];
	  NodeValList mand_cond = fi2->mand_cond();
	  {
	    bool out_of_range = false;
	    for ( auto nv: mand_cond ) {
	      auto node = nv.node();
	      if ( undet_checker.gvar(node) == kSatLiteralX ) {
		out_of_range = true;
		break;
	      }
	    }
	    if ( out_of_range ) {
	      continue;
	    }
	  }
	  NodeValList ffr_cond = fault2->ffr_propagate_condition(mFaultType);
	  SatBool3 res = undet_checker.check(ffr_cond);
	  if ( res == SatBool3::False ) {
	    // fault2 が検出可能の条件のもとで fault が検出不能となることはない．
	    // fault2 が fault を支配している．
	    mark[fault->id()] = false;
	    break;
	  }
	}
	if ( !mark[fault->id()] ) {
	  break;
	}
      }
    }
  }
  int n2 = 0;
  for ( auto fault: mNetwork.rep_fault_list() ) {
    if ( mark[fault->id()] ) {
      ++ n2;
    }
  }
  if ( debug ) {
    cout << "after semi-global dominance reduction: " << n2 << endl;
  }

  for ( auto& ffr: mNetwork.ffr_list() ) {
    //DtpgFFR2 dtpg(sat_type, sat_option, sat_outp, mFaultType, just_type, mNetwork, ffr);
    for ( auto fault: ffr.fault_list() ) {
      if ( !mark[fault->id()] ) {
	continue;
      }
      for ( auto& ffr2: mNetwork.ffr_list() ) {
	if ( &ffr == &ffr2 ) {
	  continue;
	}
	DomChecker dom_checker(mNetwork, mFaultType, ffr2.root(), fault);
	for ( auto fault2: ffr2.fault_list() ) {
	  if ( !mark[fault2->id()] ) {
	    continue;
	  }
	  SatBool3 res = dom_checker.check_detectable(fault2);
	  if ( res == SatBool3::False ) {
	    // fault2 が検出可能の条件のもとで fault が検出不能となることはない．
	    // fault2 が fault を支配している．
	    mark[fault->id()] = false;
	    break;
	  }
	}
	if ( !mark[fault->id()] ) {
	  break;
	}
      }
    }
  }
  int n3 = 0;
  for ( auto fault: mNetwork.rep_fault_list() ) {
    if ( mark[fault->id()] ) {
      ++ n3;
    }
  }
  if ( debug ) {
    cout << "after global dominance reduction: " << n3 << endl;
  }

#if 0
  int nf = tmp_fault_list.size();
  vector<pair<FaultInfo*, FaultInfo*>> tmp_conflict_list;
  vector<pair<FaultInfo*, FaultInfo*>> tmp_compatible_list;
  for ( auto i1: Range(nf - 1) ) {
    FaultInfo* fi1 = tmp_fault_list[i1];
    if ( fi1->is_dominated() ) {
      continue;
    }
    const TpgFault* fault1 = fi1->fault();
    auto suff_cond1 = fi1->sufficient_cond();
    for ( auto i2: Range(i1 + 1, nf) ) {
      FaultInfo* fi2 = tmp_fault_list[i2];
      if ( fi2->is_dominated() ) {
	continue;
      }
      const TpgFault* fault2 = fi2->fault();
      auto suff_cond2 = fi2->sufficient_cond();
      int res = compare(suff_cond1, suff_cond2);
      if ( res == -1 ) {
	// ２つの故障は衝突している．
	tmp_conflict_list.push_back(make_pair(fi1, fi2));
      }
      else if ( res & 1 ) { // res == 1 or res == 3
	// fault1 が fault2 を支配している．
	fi2->set_dominated();
      }
      else if ( res & 2 ) {
	// fault2 が fault1 を支配している．
	fi1->set_dominated();
      }
      else {
	// fault1 と fault2 は両立する可能性がある．
	tmp_compatible_list.push_back(make_pair(fi1, fi2));
      }
    }
  }
  // mark のついた故障をドロップする．
  fault_list.clear();
  fault_list.reserve(nf);
  for ( auto fi: tmp_fault_list ) {
    if ( !fi->is_dominated() ) {
      fault_list.push_back(fi);
    }
  }

  // 衝突リストを作る．
  int n_confl = 0;
  int n_compat = 0;
  for ( auto fi_pair: tmp_conflict_list ) {
    FaultInfo* fi1 = fi_pair.first;
    FaultInfo* fi2 = fi_pair.second;
    if ( !fi1->is_dominated() && !fi2->is_dominated() ) {
      fi1->add_conflict(fi2);
      fi2->add_conflict(fi1);
      ++ n_confl;
    }
  }

  // 両立関係を調べて両立リストを作る．
  for ( auto fi_pair: tmp_compatible_list ) {
    FaultInfo* fi1 = fi_pair.first;
    FaultInfo* fi2 = fi_pair.second;
    if ( fi1->is_dominated() || fi2->is_dominated() ) {
      continue;
    }
    auto suff_cond1 = fi1->sufficient_cond();
    auto suff_cond2 = fi2->sufficient_cond();
    vector<SatLiteral> assumptions;
    dtpg.add_to_assumptions(suff_cond1, assumptions);
    dtpg.add_to_assumptions(suff_cond2, assumptions);
    SatBool3 sat_res = dtpg.check(assumptions);
    if ( sat_res == SatBool3::True ) {
      // 両立している．
      fi1->add_compatible(fi2);
      fi2->add_compatible(fi1);
      ++ n_compat;
    }
    else {
      // 衝突している．
      fi1->add_conflict(fi2);
      fi2->add_conflict(fi1);
      ++ n_confl;
    }
  }
#endif
}

// @brief 故障の検出条件の解析を行う．
FaultInfo*
Analyzer::analyze_fault(
  DtpgFFR& dtpg,
  const TpgFault* fault,
  int loop_limit
)
{
#if 0
  // FFR 内の伝搬条件をリテラルに変換して加えたSAT問題を解く．
  NodeValList ffr_cond = fault->ffr_propagate_condition(mFaultType);
  SatBool3 sat_res;
  vector<SatBool3> model;
  {
    auto assumptions = dtpg.conv_to_literal_list(ffr_cond, assumptions);
    sat_res = dtpg.solve(assumptions, model);
  }
  if ( sat_res == SatBool3::True ) {
    // 検出可能
    // 検出条件の解析を行う．
    Expr expr = dtpg.get_sufficient_conditions(model);
    NodeValList tmp_cond = common_cube(expr);
    // 必要条件を求める．
    // ffr_cond は無条件で必要条件となる．
    NodeValList mand_cond(ffr_cond);
    for ( auto nv: tmp_cond ) {
      // nv を否定した条件を加えて解があるか調べる．
      auto assumptions = dtpg.conv_to_literal_list(mand_cond, assumptions);
      SatLiteral lit1 = dtpg.conv_to_literal(nv);
      assumptions.push_back(~lit1);
      if ( dtpg.check(assumptions) == SatBool3::False ) {
	// 充足不能なので nv は必要割当
	mand_cond.add(nv);
      }
    }
#if 0
    bool exhausted = true;
    expr = restrict(expr, mand_cond);
    if ( !expr.is_constant() ) {
      auto clit1 = dtpg.new_variable();
      // 別解を求める．
      exhausted = false;
      Expr expr1(expr);
      for ( int i = 0; i < loop_limit; ++ i ) {
	// expr1 を否定した節を追加する．
	dtpg.add_negation(expr1, clit1);
	auto assumptions = dtpg.conv_to_literal_list(mand_cond);
	assumptions.push_back(clit1);
	if ( dtpg.solve(assumptions) == SatBool3::True ) {
	  expr1 = dtpg.get_sufficient_conditions(fault, dtpg.model());
	  expr |= expr1;
	}
	else {
	  exhausted = true;
	  break;
	}
      }
    }
    if ( !exhausted ) {
      ++ nex_num;
    }
#endif
    auto fi = new FaultInfo(fault, mand_cond, expr);
    return fi;
  }
  else {
    return nullptr;
  }
#else
  return nullptr;
#endif
}

// @brief 論理式に含まれるキューブを求める．
NodeValList
Analyzer::common_cube(
  const Expr& expr
)
{
  ASSERT_COND( !expr.is_constant() );

  NodeValList cube;
  if ( expr.is_posi_literal() ) {
    int id = expr.varid().val();
    const TpgNode* node = mNetwork.node(id);
    cube.add(node, 1, true);
  }
  else if ( expr.is_nega_literal() ) {
    int id = expr.varid().val();
    const TpgNode* node = mNetwork.node(id);
    cube.add(node, 1, false);
  }
  else if ( expr.is_and() ) {
    int n = expr.child_num();
    for ( int i: Range(n) ) {
      NodeValList cube1 = common_cube(expr.child(i));
      cube += cube1;
    }
  }
  else if ( expr.is_or() ) {
    // 最初のキューブだけを使う．
    ASSERT_COND( expr.child_num() > 0 );
    cube = common_cube(expr.child(0));
  }
  else if ( expr.is_xor() ) {
    ASSERT_NOT_REACHED;
  }

  return cube;
}

// @brief 必要割り当てに従って論理式を簡単化する．
Expr
Analyzer::restrict(
  const Expr& expr,
  const NodeValList& mand_cond
)
{
  unordered_map<VarId, bool> val_map;
  for ( auto nv: mand_cond ) {
    const TpgNode* node = nv.node();
    bool val = nv.val();
    val_map.emplace(VarId(node->id()), val);
  }
  return _restrict_sub(expr, val_map);
}

// @brief restrict の下請け関数
Expr
Analyzer::_restrict_sub(
  const Expr& expr,
  const unordered_map<VarId, bool>& val_map
)
{
  ASSERT_COND( !expr.is_constant() );

  if ( expr.is_posi_literal() ) {
    VarId var = expr.varid();
    if ( val_map.count(var) > 0 ) {
      bool val = val_map.at(var);
      if ( val == false ) {
	return Expr::make_zero();
      }
      else if ( val == true ) {
	return Expr::make_one();
      }
    }
    return expr;
  }
  else if ( expr.is_nega_literal() ) {
    VarId var = expr.varid();
    bool val;
    if ( val_map.count(var) > 0 ) {
      bool val = val_map.at(var);
      if ( val == false ) {
	return Expr::make_one();
      }
      else if ( val == true ) {
	return Expr::make_zero();
      }
    }
    return expr;
  }
  else if ( expr.is_and() ) {
    int n = expr.child_num();
    ASSERT_COND( n > 0 );
    Expr ans_expr = _restrict_sub(expr.child(0), val_map);
    for ( int i: Range(1, n) ) {
      ans_expr &= _restrict_sub(expr.child(i), val_map);
    }
    return ans_expr;
  }
  else if ( expr.is_or() ) {
    int n = expr.child_num();
    ASSERT_COND( n > 0 );
    Expr ans_expr = _restrict_sub(expr.child(0), val_map);
    for ( int i: Range(1, n) ) {
      ans_expr |= _restrict_sub(expr.child(i), val_map);
    }
    return ans_expr;
  }
  else if ( expr.is_xor() ) {
    ASSERT_NOT_REACHED;
  }

  return Expr::make_zero();
}

END_NAMESPACE_DRUID
