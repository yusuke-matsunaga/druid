
/// @file CnfGenFactor.cc
/// @brief CnfGenFactor の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "CnfGenFactor.h"


BEGIN_NAMESPACE_DRUID

// @brief 条件を CNF に変換する．
vector<vector<SatLiteral>>
CnfGenFactor::make_cnf(
  StructEngine& engine,
  const vector<DetCond>& cond_list
)
{
  vector<vector<SatLiteral>> ans_list;
  ans_list.reserve(cond_list.size());
  for ( auto& cond: cond_list ) {
    vector<SatLiteral> assumptions;
    assumptions.reserve(cond.mandatory_condition().size() + 1);
    for ( auto as: cond.mandatory_condition() ) {
      auto lit = engine.conv_to_literal(as);
      assumptions.push_back(lit);
    }
    auto lit = cover_to_cnf(engine, cond.cube_list());
    assumptions.push_back(lit);
    ans_list.push_back(assumptions);
  }
  return ans_list;
}

// @brief カバーをCNFに変換する．
SatLiteral
CnfGenFactor::cover_to_cnf(
  StructEngine& engine,
  const vector<AssignList>& cube_list
)
{
  // Assign のノードと変数番号の対応表
  VarMgr var_mgr;

  // cube_list を SopCover に変換する．
  vector<vector<Literal>> literal_list;
  literal_list.reserve(cube_list.size());
  for ( auto& cube: cube_list ) {
    vector<Literal> cube_lits;
    cube_lits.reserve(cube.size());
    for ( auto& as: cube ) {
      var_mgr.reg(as);
      auto lit = var_mgr.to_literal(as);
      cube_lits.push_back(lit);
    }
    literal_list.push_back(cube_lits);
  }
  auto nv = var_mgr.var_num();
  auto cover = SopCover(nv, literal_list);

  // ファクタリングを行う．
  auto expr = cover.bool_factor();

  auto n = cube_list.size();
  vector<SatLiteral> tmp_lits;
  tmp_lits.reserve(n + 1);
  auto new_lit = engine.solver().new_variable(false);
  tmp_lits.push_back(~new_lit);
  for ( auto& cube: cube_list ) {
    auto new_lit1 = cube_to_cnf(engine, cube);
    tmp_lits.push_back(new_lit1);
  }
  engine.solver().add_clause(tmp_lits);

  return new_lit;
}

// @brief カバーをCNFに変換した時の CNF のサイズを見積もる．
CnfSize
CnfGenFactor::calc_cnf_size(
  const vector<DetCond>& cond_list
)
{
  auto ans = CnfSize::zero();
  for ( auto& cond: cond_list ) {
    auto& cube_list = cond.cube_list();
    for ( auto& cube: cube_list ) {
      auto n = cube.size();
      // 1つのキューブにつき
      // n 項， n * 2 リテラル
      ans += CnfSize{n, n * 2};
    }
    // 最後にキューブ数+1の項を追加
    ans += CnfSize{1, cube_list.size() + 1};
  }

  return ans;
}

END_NAMESPACE_DRUID
