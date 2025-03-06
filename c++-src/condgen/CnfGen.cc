
/// @file CnfGen.cc
/// @brief CnfGen の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "CnfGen.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス CnfGen
//////////////////////////////////////////////////////////////////////

// @brief 条件を CNF に変換する．
vector<vector<SatLiteral>>
CnfGen::make_cnf(
  StructEngine& engine,
  const vector<DetCond>& cond_list
)
{
  // 条件を Expr に変換する．
  auto expr_list = _make_expr_list(cond_list);

  // Expr のリストを CNF に変換する．
  auto lits_list = expr_to_cnf(engine, expr_list);

  return lits_list;
}

// @brief 条件を CNF に変換した時の CNF のサイズを見積もる．
CnfSize
CnfGen::calc_cnf_size(
  const vector<DetCond>& cond_list
)
{
  // 条件を Expr に変換する．
  auto expr_list = _make_expr_list(cond_list);

  // Expr のリストの CNF サイズを見積もる．
  auto size = expr_cnf_size(expr_list);

  return size;
}

END_NAMESPACE_DRUID
