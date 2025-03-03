
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

// @brief リテラルのリストから Expr を作る．
Expr
CnfGenFactor::to_expr(
  const vector<vector<Literal>>& literal_list
)
{
  if ( literal_list.empty() ) {
    return Expr::one();
  }

  // cube_list に現れる変数のみを集めた辞書を作る．
  LocalMap local_map;
  auto cover = local_map.to_cover(literal_list);

  // ファクタリングを行う．
  auto local_expr = cover.bool_factor();

  // local_expr 中の ID を元に ID に置き換える．
  auto expr = local_map.remap_expr(local_expr);
  return expr;
}

END_NAMESPACE_DRUID
