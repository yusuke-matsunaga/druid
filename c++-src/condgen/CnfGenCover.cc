
/// @file CnfGenCover.cc
/// @brief CnfGenCover の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "CnfGenCover.h"
#include "ym/Expr.h"
#include "Expr2Cnf.h"



BEGIN_NAMESPACE_DRUID

// @brief 条件を CNF に変換する．
vector<vector<SatLiteral>>
CnfGenCover::make_cnf(
  StructEngine& engine,
  const vector<DetCond>& cond_list
)
{
  vector<vector<SatLiteral>> ans_list;
  ans_list.reserve(cond_list.size());
  for ( auto& cond: cond_list ) {
    vector<SatLiteral> assumptions;
    if ( cond.type() == DetCond::Detected ) {
      auto literal_list = to_literal_list(cond);
      auto expr = to_expr(literal_list);
      auto lits = expr_to_cnf(engine, expr);
      assumptions.reserve(cond.mandatory_condition().size() + lits.size());
      for ( auto as: cond.mandatory_condition() ) {
	auto lit = engine.conv_to_literal(as);
	assumptions.push_back(lit);
      }
      for ( auto lit: lits ) {
	assumptions.push_back(lit);
      }
    }
    ans_list.push_back(assumptions);
  }
  return ans_list;
}

// @brief カバーをCNFに変換した時の CNF のサイズを見積もる．
CnfSize
CnfGenCover::calc_cnf_size(
  const vector<DetCond>& cond_list
)
{
  auto ans = CnfSize::zero();
  for ( auto& cond: cond_list ) {
    if ( cond.type() == DetCond::Detected ) {
      auto literal_list = to_literal_list(cond);
      auto expr = to_expr(literal_list);
      ans += Expr2Cnf::calc_cnf_size(expr);
    }
  }

  return ans;
}

END_NAMESPACE_DRUID
