
/// @file AssignExpr.cc
/// @brief AssignExpr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "AssignExpr.h"
#include "AssignMgr.h"


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

END_NAMESPACE_DRUID
