
/// @file CnfGenFactor.cc
/// @brief CnfGenFactor の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "CnfGenFactor.h"
#include "VarMgr.h"
#include "ym/SopCover.h"



BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// vector<AssignList> を Expr に変換する．
Expr
to_expr(
  const vector<AssignList>& cube_list,
  VarMgr& var_mgr
)
{
  // cube_list を SopCover に変換する．
  vector<vector<Literal>> literal_list;
  literal_list.reserve(cube_list.size());
  for ( auto& cube: cube_list ) {
    vector<Literal> cube_lits;
    cube_lits.reserve(cube.size());
    for ( auto& as: cube ) {
      var_mgr.reg_var(as);
      auto lit = var_mgr.to_literal(as);
      cube_lits.push_back(lit);
    }
    literal_list.push_back(cube_lits);
  }
  auto nv = var_mgr.var_num();
  {
    for ( auto& cube: literal_list ) {
      for ( auto lit: cube ) {
	if ( lit.varid() >= nv ) {
	  throw std::logic_error{"lit is out of range"};
	}
      }
    }
  }
  auto cover = SopCover(nv, literal_list);
  {
    auto literal_list = cover.literal_list();
    for ( auto& cube: literal_list ) {
      for ( auto lit: cube ) {
	if ( lit.varid() >= cover.variable_num() ) {
	  throw std::logic_error{"lit is out of range"};
	}
      }
    }
  }
  // ファクタリングを行う．
  auto expr = cover.bool_factor();
  return expr;
}

vector<SatLiteral>
expr_to_cnf(
  StructEngine& engine,
  const Expr& expr,
  const VarMgr& var_mgr
)
{
  if ( expr.is_zero() ) {
    // これは充足不可
    throw std::logic_error{"expr is zero"};
  }
  if ( expr.is_one() ) {
    // これは常に充足している．
    return {};
  }
  if ( expr.is_literal() ) {
    auto vid = expr.varid();
    auto as = var_mgr.to_assign(vid);
    auto lit = engine.conv_to_literal(as);
    if ( expr.is_nega_literal() ) {
      lit = ~lit;
    }
    return {lit};
  }
  if ( expr.is_and() ) {
    vector<SatLiteral> ans_lits;
    for ( auto& expr1: expr.operand_list() ) {
      auto lits1 = expr_to_cnf(engine, expr1, var_mgr);
      ans_lits.insert(ans_lits.end(), lits1.begin(), lits1.end());
    }
    return ans_lits;
  }
  if ( expr.is_or() ) {
    auto new_lit = engine.solver().new_variable(true);
    vector<SatLiteral> lit_list;
    lit_list.reserve(expr.operand_num() + 1);
    lit_list.push_back(~new_lit);
    for ( auto& expr1: expr.operand_list() ) {
      auto lit1 = engine.solver().new_variable(false);
      lit_list.push_back(lit1);
      auto lit_list1 = expr_to_cnf(engine, expr1, var_mgr);
      for ( auto lit: lit_list1 ) {
	engine.solver().add_clause(~lit1, lit);
      }
    }
    return {new_lit};
  }
  if ( expr.is_xor() ) {
    throw std::logic_error{"EXOR cannot be handled"};
  }
  return {};
}

// @brief カバーをCNFに変換する．
vector<SatLiteral>
cover_to_cnf(
  StructEngine& engine,
  const vector<AssignList>& cube_list
)
{
  // Assign のノードと変数番号の対応表
  VarMgr var_mgr;

  // Expr に変換する．
  auto expr = to_expr(cube_list, var_mgr);
  // CNF に変換する．
  auto lits = expr_to_cnf(engine, expr, var_mgr);
  return lits;
}

// Expr を CNF に変換した際のサイズを計算する．
// 結果は size に格納される．
// この関数自身は expr に対応するリテラル数を返す．
SizeType
calc_expr_size(
  const Expr& expr,
  CnfSize& size
)
{
  if ( expr.is_zero() ) {
    throw std::logic_error{"expr is zero"};
  }
  if ( expr.is_one() ) {
    return 0;
  }
  if ( expr.is_literal() ) {
    return 1;
  }
  if ( expr.is_and() ) {
    auto n = 0;
    for ( auto& expr1: expr.operand_list() ) {
      n += calc_expr_size(expr1, size);
    }
    return n;
  }
  if ( expr.is_or() ) {
    for ( auto& expr1: expr.operand_list() ) {
      auto n1 = calc_expr_size(expr1, size);
      size += CnfSize{n1, n1 * 2};
    }
    auto n = expr.operand_num();
    size += CnfSize{1, n + 1};
    return 1;
  }
  if ( expr.is_xor() ) {
    throw std::logic_error{"EXOR cannot be handled"};
  }
  return 0;
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

  // Assign のノードと変数番号の対応表
  VarMgr var_mgr;

  // Expr に変換する．
  auto expr = to_expr(cube_list, var_mgr);

  {
    cout << " => " << expr.literal_num() << " literals" << endl;
  }

  auto size = CnfSize::zero();
  auto n = calc_expr_size(expr, size);

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
  vector<vector<SatLiteral>> ans_list;
  ans_list.reserve(cond_list.size());
  for ( auto& cond: cond_list ) {
    vector<SatLiteral> assumptions;
    assumptions.reserve(cond.mandatory_condition().size() + 1);
    for ( auto as: cond.mandatory_condition() ) {
      auto lit = engine.conv_to_literal(as);
      assumptions.push_back(lit);
    }
    auto lits = cover_to_cnf(engine, cond.cube_list());
    for ( auto lit: lits ) {
      assumptions.push_back(lit);
    }
    ans_list.push_back(assumptions);
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
    // mandatory_condition() に対応するリテラルは CNF にならない．
    auto& cube_list = cond.cube_list();
    ans += calc_size(cube_list);

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
