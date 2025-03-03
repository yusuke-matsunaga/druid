
/// @file CnfGenFactor.cc
/// @brief CnfGenFactor の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "CnfGenFactor.h"
#include "ym/SopCover.h"
#include "ym/Expr.h"
#include "AssignVarDict.h"
#include "Expr2Aig.h"
#include "Expr2Cnf.h"



BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

SopCover
to_cover(
  StructEngine& engine,
  const vector<AssignList>& cube_list,
  std::unordered_map<SizeType, SatLiteral>& lit_map
)
{
  // Assign のノードと変数番号の対応表
  AssignVarDict as_dict;

  // cube_list を SopCover に変換する．
  vector<vector<Literal>> literal_list;
  literal_list.reserve(cube_list.size());
  for ( auto& cube: cube_list ) {
    vector<Literal> cube_lits;
    cube_lits.reserve(cube.size());
    for ( auto& as: cube ) {
      auto id = as_dict.reg_assign(as);
      auto satlit = engine.conv_to_literal(as);
      lit_map.emplace(id, satlit);
      auto lit = as_dict.to_literal(as);
      cube_lits.push_back(lit);
    }
    literal_list.push_back(cube_lits);
  }
  auto nv = as_dict.var_num();
  auto cover = SopCover(nv, literal_list);
  return cover;
}

// vector<AssignList> を Expr に変換する．
Expr
to_expr(
  const SopCover& cover
)
{
  // ファクタリングを行う．
  auto expr = cover.bool_factor();
  return expr;
}

// @brief カバーをCNFに変換する．
vector<SatLiteral>
cond_to_cnf(
  StructEngine& engine,
  const vector<AssignList>& cube_list
)
{
  if ( cube_list.empty() ) {
    return {};
  }

  AssignVarDict as_dict;
  auto cover = as_dict.to_cover(cube_list);

  // Expr に変換する．
  auto expr = to_expr(cover);

  // 変数番号とSATリテラルの対応表を作る．
  std::unordered_map<SizeType, SatLiteral> lit_map;
  for ( SizeType id = 0; id < as_dict.var_num(); ++ id ) {
    auto as = as_dict.to_assign(id);
    auto satlit = engine.conv_to_literal(as);
    lit_map.emplace(id, satlit);
  }

  // CNF に変換する．
  Expr2Cnf conv(engine.solver(), lit_map);
  auto lits = conv.make_cnf(expr);
  return lits;
}

// @brief カバーをCNFに変換した時の CNF のサイズを見積もる．
CnfSize
calc_size(
  const vector<AssignList>& cube_list
)
{
  if ( cube_list.empty() ) {
    return CnfSize::zero();
  }

  {
    SizeType n = 0;
    for ( auto& cube: cube_list ) {
      n += cube.size();
    }
    cout << "Initial: " << cube_list.size()
	 << " cubes, " << n << " literals" << endl;
  }

  AssignVarDict as_dict;
  auto cover = as_dict.to_cover(cube_list);

  // Expr に変換する．
  auto expr = to_expr(cover);
  auto size = Expr2Cnf::calc_cnf_size(expr);
  return size;
}

END_NONAMESPACE

// @brief 条件を CNF に変換する．
vector<vector<SatLiteral>>
CnfGenFactor::make_cnf(
  StructEngine& engine,
  const vector<DetCond>& cond_list
)
{
  vector<Expr> expr_list;
  expr_list.reserve(cond_list.size());
  for ( auto& cond: cond_list ) {
    if ( cond.type() == DetCond::Detected ) {
      AssignVarDict as_dict;
      auto cover = as_dict.to_cover(cond.cube_list());
      // Expr に変換する．
      auto expr = to_expr(cover);
      expr_list.push_back(expr);
    }
    else {
      expr_list.push_back(Expr::make_zero());
    }
  }

  Expr2Aig expr2aig(mgr);
  auto aig_list = expr2aig.conv_to_aig(expr_list);
  auto lits_list = aig_to_cnf(engine, aig_list, lit_map);
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

// @brief カバーをCNFに変換した時の CNF のサイズを見積もる．
CnfSize
CnfGenFactor::calc_cnf_size(
  const vector<DetCond>& cond_list
)
{
  auto ans = CnfSize::zero();
  for ( auto& cond: cond_list ) {
    if ( cond.type() == DetCond::Detected ) {
      // mandatory_condition() に対応するリテラルは CNF にならない．
      auto& cube_list = cond.cube_list();
      ans += calc_size(cube_list);
    }
  }

  return ans;
}

END_NAMESPACE_DRUID
