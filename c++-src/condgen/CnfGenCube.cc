
/// @file CnfGenCube.cc
/// @brief CnfGenCube の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "CnfGenCube.h"
#include "ym/SopCover.h"
#include "ym/SopCube.h"
#include "VarMgr.h"


BEGIN_NAMESPACE_DRUID

// @brief 条件を CNF に変換する．
vector<vector<SatLiteral>>
CnfGenCube::make_cnf(
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
CnfGenCube::cover_to_cnf(
  StructEngine& engine,
  const vector<AssignList>& cube_list
)
{
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

BEGIN_NONAMESPACE

SopCover
gen_cover(
  VarMgr& var_mgr,
  vector<AssignList>& src_cube_list
)
{
  SizeType nv = var_mgr.var_num();
  vector<SopCube> cube_list;
  for ( auto& src_cube: src_cube_list ) {
    vector<Literal> lit_list;
    for ( auto& as: src_cube ) {
      auto lit = var_mgr.to_literal(as);
      lit_list.push_back(lit);
    }
    SopCube cube{nv, lit_list};
    cube_list.push_back(cube);
  }
  return SopCover{nv, cube_list};
}

#if 0
CnfSize
_calc_size(
  const Expr& expr
)
{
  ASSERT_COND( expr.is_valid() ) ;
  ASSERT_COND( !expr.is_constant() );

  if ( expr.is_literal() ) {
  }
}

CnfSize
calc_size(
  const Expr& expr
)
{
  ASSERT_COND( expr.is_valid() ) ;
  ASSERT_COND( !expr.is_constant() );

  // 特例: 単純なキューブなら CNF を作る必要はない．
  if ( expr.is_literal() ) {
    return CnfSize::zero();
  }
  auto size = CnfSize::zero();
  if ( expr.is_and ) {
    for ( auto& expr1: expr.operand_list() ) {
      size += _calc_size(expr1);
    }
  }
  else if ( expr.is_or() ) {
    size += expr.operand_num();
    for ( auto& expr1: expr.operand_list() ) {
      size += _calc_size(expr1);
    }
  }
}
#endif

struct Elem {
  SizeType row;
  SizeType col;
  SizeType cube_id;
};

#if 0
void
make_matrix(
  const vector<SopCover>& cover_list
)
{
  // カーネルキューブを管理するクラス
  CubeMgr kernel_cube_mgr;

  // キューブを管理するクラス
  CubeMgr cube_mgr;
  // 行の情報のリスト
  vector<pair<SizeType, SopCube>> row_list;
  // 要素のリスト
  vector<Elem> elem_list;
  SizeType n = cover_list.size();
  for ( SizeType i = 0; i < n; ++ i ) {
    auto& cover = cover_list[i];
    auto kc_list = cover.all_kernels();
    for ( auto& kc: kc_list ) {
      auto& kernel = kc.first;
      auto nk = kernel.cube_num();
      vector<SizeType> col_list;
      col_list.reseve(nk);
      for ( auto& kernel_cube: kernel ) {
	SizeType col = kernel_cube_mgr.reg_cube(kernel_cube);
	col_list.push_back(col);
      }
      auto& ckcube_list = kc.second;
      for ( auto& ckcube: ckcube_lsit ) {
	auto row = row_list.size();
	row_list.push_back({i, ckcube});
	for ( SizeType i = 0; i < nk; ++ i ) {
	  auto col = col_list[i];
	  auto kernel_cube = kernel.get_cube(i);
	  auto tmp_cube = ckcube & kernel_cube;
	  auto cube_id = cube_mgr.reg_cube(tmp_cube);
	  elem_list.push_back(Elem{row, col, cube_id});
	}
      }
    }
  }
}
#endif

END_NONAMESPACE

// @brief カバーをCNFに変換した時の CNF のサイズを見積もる．
CnfSize
CnfGenCube::calc_cnf_size(
  const vector<DetCond>& cond_list
)
{
  // Assign のノードと変数番号の対応表
  VarMgr var_mgr;

  for ( auto& cond: cond_list ) {
    auto& cube_list = cond.cube_list();
    for ( auto& cube: cube_list ) {
      for ( auto as: cube ) {
	var_mgr.reg_var(as);
      }
    }
  }
  auto nv = var_mgr.var_num();

#if 0
  vector<SopCover> cover_list;
  cover_list.reserve(cond_list.size());
  for ( auto& cond: cond_list ) {
    auto cover = gen_cover(var_mgr, cond.cube_list());
    cover_list.push_back(cover);
  }
#endif

  auto ans = CnfSize::zero();

  return ans;
}

END_NAMESPACE_DRUID
