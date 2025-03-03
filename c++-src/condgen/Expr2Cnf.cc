
/// @file Expr2Cnf.cc
/// @brief Expr2Cnf の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "Expr2Cnf.h"


BEGIN_NAMESPACE_DRUID

vector<SatLiteral>
Expr2Cnf::make_cnf(
  const Expr& expr
)
{
  if ( expr.is_zero() ) {
    // これは充足不可
    abort();
    throw std::logic_error{"expr is zero"};
  }
  if ( expr.is_one() ) {
    // これは常に充足している．
    return {};
  }
  if ( expr.is_literal() ) {
    auto vid = expr.varid();
    auto lit = mLitMap.at(vid);
    if ( expr.is_nega_literal() ) {
      lit = ~lit;
    }
    return {lit};
  }
  if ( expr.is_and() ) {
    vector<SatLiteral> ans_lits;
    for ( auto& expr1: expr.operand_list() ) {
      auto lits1 = make_cnf(expr1);
      ans_lits.insert(ans_lits.end(), lits1.begin(), lits1.end());
    }
    return ans_lits;
  }
  if ( expr.is_or() ) {
    auto new_lit = mSolver.new_variable(true);
    vector<SatLiteral> lit_list;
    lit_list.reserve(expr.operand_num() + 1);
    lit_list.push_back(~new_lit);
    for ( auto& expr1: expr.operand_list() ) {
      auto lit_list1 = make_cnf(expr1);
      if ( lit_list1.empty() ) {
	continue;
      }
      if ( lit_list1.size() == 1 ) {
	auto lit1 = lit_list1.front();
	lit_list.push_back(lit1);
      }
      else {
	auto lit1 = mSolver.new_variable(false);
	lit_list.push_back(lit1);
	for ( auto lit: lit_list1 ) {
	  mSolver.add_clause(~lit1, lit);
	}
      }
    }
    mSolver.add_clause(lit_list);
    return {new_lit};
  }
  if ( expr.is_xor() ) {
    throw std::logic_error{"EXOR cannot be handled"};
  }
  return {};
}

BEGIN_NONAMESPACE

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
      if ( n1 > 1 ) {
	size += CnfSize{n1, n1 * 2};
      }
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

END_NONAMESPACE

// @brief Expr を CNF 式に変換したときのサイズを計算する．
CnfSize
Expr2Cnf::calc_cnf_size(
  const Expr& expr
)
{
  CnfSize size;
  calc_expr_size(expr, size);
  return size;
}

END_NAMESPACE_DRUID
