
/// @file AssignExpr.cc
/// @brief AssignExpr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "AssignExpr.h"
#include "AssignMgr.h"
#include "TpgNode.h"


BEGIN_NAMESPACE_DRUID

// @brief AssignList からの変換コンストラクタ
AssignExpr::AssignExpr(
  const AssignList& cube
)
{
  AssignMgr mgr;
  mExpr = mgr.to_expr(cube);
  mMap = mgr.assign_map();
}

// @brief 正規化した論理式を返す．
Expr
AssignExpr::normalize() const
{
  std::unordered_map<SizeType, SizeType> varmap;
  auto n = variable_num();
  for ( SizeType i = 0; i < n; ++ i ) {
    auto as = assign(i);
    auto node = as.node();
    auto time = as.time();
    SizeType new_var = (node->id() * 2) + time;
    varmap.emplace(i, new_var);
  }
  return mExpr.remap_var(varmap);
}

END_NAMESPACE_DRUID
