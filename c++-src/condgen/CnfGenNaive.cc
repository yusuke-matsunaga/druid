
/// @file CnfGenNaive.cc
/// @brief CnfGenNaive の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "CnfGenNaive.h"


BEGIN_NAMESPACE_DRUID

// @brief カバーをCNFに変換する．
SatLiteral
CnfGenNaive::cover_to_cnf(
  const vector<AssignList>& cube_list
)
{
  auto n = cube_list.size();
  vector<SatLiteral> tmp_lits;
  tmp_lits.reserve(n + 1);
  auto new_lit = solver().new_variable(false);
  tmp_lits.push_back(~new_lit);
  for ( auto& cube: cube_list ) {
    auto new_lit1 = solver().new_variable(false);
    auto cube_lits = cube_to_literals(cube);
    for ( auto lit: cube_lits ) {
      solver().add_clause(~new_lit1, lit);
    }
    tmp_lits.push_back(new_lit1);
  }
  solver().add_clause(tmp_lits);

  return new_lit;
}

// @brief カバーをCNFに変換した時の CNF のサイズを見積もる．
CnfSize
CnfGenNaive::calc_cover_size(
  const vector<AssignList>& cube_list
)
{
  auto ans = CnfSize::zero();
  for ( auto& cube: cube_list ) {
    auto n = cube.size();
    // 1つのキューブにつき
    // n 項， n * 2 リテラル
    ans += CnfSize{n, n * 2};
  }
  // 最後にキューブ数+1の項を追加
  ans += CnfSize{1, cube_list.size() + 1};

  return ans;
}

END_NAMESPACE_DRUID
