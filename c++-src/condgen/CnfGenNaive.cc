
/// @file CnfGenNaive.cc
/// @brief CnfGenNaive の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "CnfGenNaive.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

Expr
lits_to_expr(
  const vector<Literal>& lits
)
{
  auto expr = Expr::one();
  for ( auto lit: lits ) {
    auto expr1 = Expr::literal(lit);
    expr &= expr1;
  }
  return expr;
}

END_NONAMESPACE

// @brief DetCond::CondData から Expr を作る．
Expr
CnfGenNaive::to_expr(
  const DetCond::CondData& cond
)
{
  auto expr = lits_to_expr(cond.mand_cond);
  if ( cond.cube_list.empty() ) {
    return expr;
  }
  auto cov_expr = Expr::zero();
  for ( auto lits: cond.cube_list ) {
    auto expr1 = lits_to_expr(lits);
    cov_expr |= expr1;
  }
  return expr & cov_expr;
}

// @brief Expr のリストから CNF を作る．
vector<vector<SatLiteral>>
CnfGenNaive::expr_to_cnf(
  StructEngine& engine,
  const vector<Expr>& expr_list
)
{
  vector<vector<SatLiteral>> lits_list;
  lits_list.reserve(expr_list.size());
  for ( auto& expr: expr_list ) {
    auto lits = engine.make_cnf(expr);
    lits_list.push_back(lits);

  }
  return lits_list;
}

BEGIN_NONAMESPACE

// Expr を CNF に変換した際のサイズを計算する．
// 結果は size に格納される．
// この関数自身は expr に対応するリテラル数を返す．
SizeType
calc_expr_size(
  const Expr& expr,
  CnfSize& size
)
{
  if ( expr.is_zero() ) {
    throw std::logic_error{"expr is zero"};
  }
  if ( expr.is_one() ) {
    return 0;
  }
  if ( expr.is_literal() ) {
    return 1;
  }
  if ( expr.is_and() ) {
    auto n = 0;
    for ( auto& expr1: expr.operand_list() ) {
      n += calc_expr_size(expr1, size);
    }
    return n;
  }
  if ( expr.is_or() ) {
    for ( auto& expr1: expr.operand_list() ) {
      auto n1 = calc_expr_size(expr1, size);
      if ( n1 > 1 ) {
	size += CnfSize{n1, n1 * 2};
      }
    }
    auto n = expr.operand_num();
    size += CnfSize{1, n + 1};
    return 1;
  }
  if ( expr.is_xor() ) {
    throw std::logic_error{"EXOR cannot be handled"};
  }
  return 0;
}

END_NONAMESPACE

// @brief Expr のリストから CNF サイズを見積もる．
CnfSize
CnfGenNaive::expr_cnf_size(
  const vector<Expr>& expr_list
)
{
  auto ans = CnfSize::zero();
  for ( auto& expr: expr_list ) {
    calc_expr_size(expr, ans);
  }
  return ans;
}

END_NAMESPACE_DRUID
