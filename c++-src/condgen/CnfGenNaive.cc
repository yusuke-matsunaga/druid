
/// @file CnfGenNaive.cc
/// @brief CnfGenNaive の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "CnfGenNaive.h"
#include "Expr2Cnf.h"


BEGIN_NAMESPACE_DRUID

// @brief リテラルのリストから Expr を作る．
Expr
CnfGenNaive::to_expr(
  const vector<vector<Literal>>& literal_list
)
{
  if ( literal_list.empty() ) {
    return Expr::one();
  }
  auto expr = Expr::zero();
  for ( auto& lits: literal_list ) {
    auto product = Expr::one();
    for ( auto lit: lits ) {
      product &= Expr::literal(lit);
    }
    expr |= product;
  }
  return expr;
}

// @brief Expr のリストから CNF を作る．
vector<vector<SatLiteral>>
CnfGenNaive::expr_to_cnf(
  StructEngine& engine,
  const vector<Expr>& expr_list
)
{
  // 変数番号とSATリテラルの対応表を作る．
  std::unordered_map<SizeType, SatLiteral> lit_map;
  for ( SizeType id = 0; id < var_num(); ++ id ) {
    auto as = get_assign(id);
    auto satlit = engine.conv_to_literal(as);
    lit_map.emplace(id, satlit);
  }
  Expr2Cnf conv(engine.solver(), lit_map);

  vector<vector<SatLiteral>> lits_list;
  lits_list.reserve(expr_list.size());
  for ( auto& expr: expr_list ) {
    auto lits = conv.make_cnf(expr);
    lits_list.push_back(lits);
  }
  return lits_list;
}

// @brief Expr のリストから CNF サイズを見積もる．
CnfSize
CnfGenNaive::expr_cnf_size(
  const vector<Expr>& expr_list
)
{
  auto ans = CnfSize::zero();
  for ( auto& expr: expr_list ) {
    ans += Expr2Cnf::calc_cnf_size(expr);
  }
  return ans;
}

END_NAMESPACE_DRUID
