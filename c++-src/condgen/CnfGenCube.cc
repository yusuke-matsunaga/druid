
/// @file CnfGenCube.cc
/// @brief CnfGenCube の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "CnfGenCube.h"
#include "CubeMgr.h"


BEGIN_NAMESPACE_DRUID

// @brief カバーをCNFに変換する．
SatLiteral
CnfGenCube::cover_to_cnf(
  const vector<AssignList>& cube_list
)
{
  auto n = cube_list.size();
  vector<SatLiteral> tmp_lits;
  tmp_lits.reserve(n + 1);
  auto new_lit = solver().new_variable(false);
  tmp_lits.push_back(~new_lit);
  for ( auto& cube: cube_list ) {
    auto new_lit1 = cube_to_cnf(cube);
    tmp_lits.push_back(new_lit1);
  }
  solver().add_clause(tmp_lits);

  return new_lit;
}

// @brief カバーをCNFに変換した時の CNF のサイズを見積もる．
CnfSize
CnfGenCube::calc_cnf_size(
  const vector<DetCond>& cond_list
)
{
  CubeMgr cube_mgr;
  SizeType nc = 0;
  for ( auto& cond: cond_list ) {
    auto& cube_list = cond.cube_list();
    nc += cube_list.size();
    for ( auto& cube: cube_list ) {
      cube_mgr.reg_cube(cube);
    }
  }

  cout << "Total Cube Num:  " << nc << endl
       << "Unique Cube Num: " << cube_mgr.cube_num() << endl;

  auto ans = CnfSize::zero();
  for ( auto& cube: cube_mgr.cube_list() ) {
    auto n = cube.size();
    // 1つのキューブにつき
    // n 項， n * 2 リテラル
    ans += CnfSize{n, n * 2};
  }
  for ( auto& cond: cond_list ) {
    auto& cube_list = cond.cube_list();
    // キューブ数+1の項を追加
    ans += CnfSize{1, cube_list.size() + 1};
  }

  return ans;
}

END_NAMESPACE_DRUID
