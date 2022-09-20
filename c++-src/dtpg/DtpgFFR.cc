
/// @file DtpgFFR.cc
/// @brief DtpgFFR の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgFFR.h"

#include "TpgFault.h"
#include "TpgFFR.h"
#include "NodeValList.h"
#include "TestVector.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
DtpgFFR::DtpgFFR(
  const TpgNetwork& network,
  FaultType fault_type,
  const TpgFFR& ffr,
  const string& just_type,
  const SatSolverType& solver_type
) : DtpgEngine{network, fault_type, ffr.root(), just_type, solver_type}
{
  make_cnf();
}

// @brief デストラクタ
DtpgFFR::~DtpgFFR()
{
}

// @brief テスト生成を行なう．
DtpgResult
DtpgFFR::gen_k_patterns(
  const TpgFault* fault,
  int k,
  vector<TestVector>& tv_list
)
{
  auto ffr_root = fault->tpg_onode()->ffr_root();
  ASSERT_COND( ffr_root == root_node() );

  // FFR 内の故障伝搬条件を ffr_cond に入れる．
  auto ffr_cond = fault->ffr_propagate_condition(fault_type());

  // ffr_cond の内容を assumptions に追加する．
  auto assumptions = conv_to_literal_list(ffr_cond);

  SatBool3 sat_res = check(assumptions);
  if ( sat_res == SatBool3::True ) {
    auto testvect = backtrace(ffr_root, ffr_cond);
    DtpgResult ans{testvect};
    tv_list.clear();
    tv_list.push_back(testvect);

    if ( k > 1 ) {
      // ここで追加するCNF式を制御する変数
      auto clit = solver().new_variable();

      for ( auto i: Range(k - 1) ) {
	// testvect の否定を表すCNF式を追加する．
	vector<SatLiteral> tmp_lits;
	tmp_lits.push_back(~clit);
	for ( auto pos: Range(testvect.ppi_num()) ) {
	  Val3 val = testvect.ppi_val(pos);
	  if ( val == Val3::_X ) {
	    continue;
	  }
	  auto node = network().ppi(pos);
	  auto lit = gvar(node);
	  if ( val == Val3::_0 ) {
	    tmp_lits.push_back(lit);
	  }
	  else {
	    tmp_lits.push_back(~lit);
	  }
	}
	solver().add_clause(tmp_lits);
	// assumptions のコピーを作る．
	auto assumptions1 = assumptions;
	assumptions1.push_back(clit);
	SatBool3 sat_res = check(assumptions1);
	if ( sat_res == SatBool3::True ) {
	  testvect = backtrace(ffr_root, ffr_cond);
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
    return DtpgResult{FaultStatus::Untestable};
  }
  else { // sat_res == SatBool3::X
    return DtpgResult{FaultStatus::Undetected};
  }
}

// @brief テストパタンの核となる式を求める．
Expr
DtpgFFR::gen_core_expr(
  const TpgFault* fault,
  int k
)
{
  auto ffr_root = fault->tpg_onode()->ffr_root();
  ASSERT_COND( ffr_root == root_node() );

  // FFR 内の故障伝搬条件を ffr_cond に入れる．
  auto ffr_cond = fault->ffr_propagate_condition(fault_type());

  // ffr_cond の内容を assumptions に追加する．
  auto assumptions = conv_to_literal_list(ffr_cond);

  Expr expr = Expr::make_zero();

  SatBool3 sat_res = check(assumptions);
  if ( sat_res == SatBool3::True ) {
    auto suf_cond = get_sufficient_condition(ffr_root);
    auto mand_cond = get_mandatory_condition(ffr_cond, suf_cond);
    auto clit = solver().new_variable();
    Expr expr1 = get_sufficient_conditions(ffr_root);
    expr |= expr1;
    for ( auto i: Range(k) ) {
      add_negation(expr1, clit);
      vector<SatLiteral> assumptions1(assumptions);
      assumptions1.push_back(clit);

      SatBool3 tmp_res = check(assumptions1);
      if ( tmp_res == SatBool3::False ) {
	break;
      }
      expr1 = get_sufficient_conditions(ffr_root);
      expr |= expr1;
    }
  }

  return expr;
}

END_NAMESPACE_DRUID
