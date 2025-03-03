
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

  // 結果を作る．
  vector<vector<SatLiteral>> ans_list(cond_list.size());
  for ( SizeType id = 0; id < cond_list.size(); ++ id ) {
    auto& cond = cond_list[id];
    if ( cond.type() == DetCond::Detected ) {
      vector<SatLiteral> assumptions;
      auto lits = lits_list[id];
      assumptions.reserve(cond.mandatory_condition().size() + lits.size());
      for ( auto as: cond.mandatory_condition() ) {
	auto lit = engine.conv_to_literal(as);
	assumptions.push_back(lit);
      }
      for ( auto lit: lits ) {
	assumptions.push_back(lit);
      }
      ans_list[id] = assumptions;
    }
  }
  return ans_list;
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
