
/// @file ExprGen_Naive.cc
/// @brief ExprGen_Naive の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "ExprGen_Naive.h"


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
ExprGen_Naive::conv(
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

END_NAMESPACE_DRUID
