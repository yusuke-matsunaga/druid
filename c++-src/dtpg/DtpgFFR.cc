
/// @file DtpgFFR.cc
/// @brief DtpgFFR の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgFFR.h"

#include "TpgFault.h"
#include "TpgFFR.h"
#include "NodeValList.h"
#include "TestVector.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
// @param[in] network 対象のネットワーク
// @param[in] fault_type 故障の種類
// @param[in] just_type Justifier の種類を表す文字列
// @param[in] ffr 故障伝搬の起点となる FFR
// @param[in] solver_type SATソルバの実装タイプ
DtpgFFR::DtpgFFR(const TpgNetwork& network,
		 FaultType fault_type,
		 const TpgFFR& ffr,
		 const string& just_type,
		 const SatSolverType& solver_type) :
  DtpgEngine(network, fault_type, ffr.root(), just_type, solver_type)
{
  cnf_begin();

  // 変数割り当て
  prepare_vars();

  // 正常回路の CNF を生成
  gen_good_cnf();

  // 故障回路の CNF を生成
  gen_faulty_cnf();

  //////////////////////////////////////////////////////////////////////
  // 故障の検出条件(正確には mRoot から外部出力までの故障の伝搬条件)
  //////////////////////////////////////////////////////////////////////
  {
    vector<SatLiteral> odiff;
    odiff.reserve(output_list().size());
    for ( auto node: output_list() ) {
      SatLiteral dlit(dvar(node));
      odiff.push_back(dlit);
    }
    solver().add_clause(odiff);

    if ( !root_node()->is_ppo() ) {
      // root_node() の dlit が1でなければならない．
      SatLiteral dlit0(dvar(root_node()));
      solver().add_clause(dlit0);
    }
  }

  cnf_end();
}

// @brief デストラクタ
DtpgFFR::~DtpgFFR()
{
}

// @brief テスト生成を行なう．
// @param[in] fault 対象の故障
// @param[out] testvect テストパタンを格納する変数
// @return 結果を返す．
DtpgResult
DtpgFFR::gen_pattern(const TpgFault* fault)
{
  const TpgNode* ffr_root = fault->tpg_onode()->ffr_root();
  ASSERT_COND( ffr_root == root_node() );

  // FFR 内の故障伝搬条件を ffr_cond に入れる．
  NodeValList ffr_cond = ffr_propagate_condition(fault, fault_type());

  // ffr_cond の内容を assumptions に追加する．
  vector<SatLiteral> assumptions;
  conv_to_assumptions(ffr_cond, assumptions);

  SatBool3 sat_res = solve(assumptions);
  if ( sat_res == SatBool3::True ) {
    NodeValList suf_cond = get_sufficient_condition();
    suf_cond.merge(ffr_cond);
    TestVector testvect = backtrace(suf_cond);
    return DtpgResult(testvect);
  }
  else if ( sat_res == SatBool3::False ) {
    return DtpgResult::make_untestable();
  }
  else { // sat_res == SatBool3::X
    return DtpgResult::make_undetected();
  }
}

// @brief テスト生成を行なう．
// @param[in] fault 対象の故障
// @param[in] k 求めるベクタ数
// @param[out] tv_list ベクタを入れるリスト
// @return 結果を返す．
//
// * tv_list[0] は DtpgResult のベクタと同じ．
// * tv_list の要素数が k より少ない場合がある．
DtpgResult
DtpgFFR::gen_k_patterns(const TpgFault* fault,
			int k,
			vector<TestVector>& tv_list)
{
  const TpgNode* ffr_root = fault->tpg_onode()->ffr_root();
  ASSERT_COND( ffr_root == root_node() );

  // FFR 内の故障伝搬条件を ffr_cond に入れる．
  NodeValList ffr_cond = ffr_propagate_condition(fault, fault_type());

  // ffr_cond の内容を assumptions に追加する．
  vector<SatLiteral> assumptions;
  conv_to_assumptions(ffr_cond, assumptions);

  SatBool3 sat_res = solve(assumptions);
  if ( sat_res == SatBool3::True ) {
    NodeValList suf_cond = get_sufficient_condition();
    suf_cond.merge(ffr_cond);
    TestVector testvect = backtrace(suf_cond);
    DtpgResult ans(testvect);
    tv_list.clear();
    tv_list.push_back(testvect);

    if ( k > 1 ) {
      // ここで追加するCNF式を制御する変数
      SatVarId cvar = solver().new_variable();
      SatLiteral clit(cvar);

      for ( auto i: Range(k - 1) ) {
	// testvect の否定を表すCNF式を追加する．
	vector<SatLiteral> tmp_lits;
	tmp_lits.push_back(~clit);
	for ( auto pos: Range(testvect.ppi_num()) ) {
	  Val3 val = testvect.ppi_val(pos);
	  if ( val == Val3::_X ) {
	    continue;
	  }
	  const TpgNode* node = network().ppi(pos);
	  SatLiteral lit(gvar(node));
	  if ( val == Val3::_0 ) {
	    tmp_lits.push_back(lit);
	  }
	  else {
	    tmp_lits.push_back(~lit);
	  }
	}
	solver().add_clause(tmp_lits);
	vector<SatLiteral> assumptions1(assumptions);
	assumptions1.push_back(clit);
	SatBool3 sat_res = solve(assumptions1);
	if ( sat_res == SatBool3::True ) {
	  NodeValList suf_cond = get_sufficient_condition();
	  suf_cond.merge(ffr_cond);
	  testvect = backtrace(suf_cond);
	  tv_list.push_back(testvect);
	}
	else {
	  break;
	}
      }
    }

    return ans;
  }
  else if ( sat_res == SatBool3::False ) {
    return DtpgResult::make_untestable();
  }
  else { // sat_res == SatBool3::X
    return DtpgResult::make_undetected();
  }
}

// @brief テストパタンの核となる式を求める．
// @param[in] fault 対象の故障
// @param[in] k 繰り返し回数
// @return テストパタンの核となる論理式
//
// 検出不能の場合は定数０が返される．
Expr
DtpgFFR::gen_core_expr(const TpgFault* fault,
		       int k)
{
  const TpgNode* ffr_root = fault->tpg_onode()->ffr_root();
  ASSERT_COND( ffr_root == root_node() );

  // FFR 内の故障伝搬条件を ffr_cond に入れる．
  NodeValList ffr_cond = ffr_propagate_condition(fault, fault_type());

  // ffr_cond の内容を assumptions に追加する．
  vector<SatLiteral> assumptions;
  conv_to_assumptions(ffr_cond, assumptions);

  Expr expr = Expr::zero();

  SatBool3 sat_res = solve(assumptions);
  if ( sat_res == SatBool3::True ) {
    NodeValList suf_cond = get_sufficient_condition();
    NodeValList mand_cond = get_mandatory_condition(ffr_cond, suf_cond);
    SatVarId cvar = solver().new_variable();
    SatLiteral clit(cvar);
    Expr expr1 = get_sufficient_conditions();
    expr |= expr1;
    for ( auto i: Range(k) ) {
      add_negation(expr1, clit);
      vector<SatLiteral> assumptions1(assumptions);
      assumptions1.push_back(clit);

      SatBool3 tmp_res = solve(assumptions1);
      if ( tmp_res == SatBool3::False ) {
	break;
      }
      expr1 = get_sufficient_conditions();
      expr |= expr1;
    }
  }

  return expr;
}

END_NAMESPACE_DRUID
