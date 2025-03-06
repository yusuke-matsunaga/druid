
/// @file CnfGenFactor.cc
/// @brief CnfGenFactor の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "CnfGenFactor.h"
#include "ym/SopCover.h"
#include "ym/Expr.h"
#include "LocalMap.h"


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

// @brief DetCond から Expr を作る．
Expr
CnfGenFactor::to_expr(
  const DetCond::CondData& cond
)
{
  auto expr = lits_to_expr(cond.mand_cond);

  // cube_list に現れる変数のみを集めた辞書を作る．
  LocalMap local_map;
  auto cover = local_map.to_cover(cond.cube_list);

  // ファクタリングを行う．
  auto local_expr = cover.bool_factor();

  // local_expr 中の ID を元に ID に置き換える．
  auto cov_expr = local_map.remap_expr(local_expr);
  return expr & cov_expr;
}

END_NAMESPACE_DRUID
