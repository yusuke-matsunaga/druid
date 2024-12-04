
/// @file CnfGen.cc
/// @brief CnfGen の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "CnfGen.h"
#include "CnfGenImpl.h"
#include "StructEngine.h"
#include "AssignMap.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス CnfGen
//////////////////////////////////////////////////////////////////////

// @brief AssignExpr を CNF に変換する．
vector<SatLiteral>
CnfGen::make_cnf(
  StructEngine& engine,
  const AssignExpr& expr,
  const JsonValue& option
)
{
  CnfGenImpl gen{engine, expr.assign_map()};
  vector<SatLiteral> assumptions;
  gen.make_cnf(expr.expr(), assumptions);
  return assumptions;
}

// @brief AssignExpr を CNF に変換した際の項数とリテラル数を数える．
CnfSize
CnfGen::calc_cnf_size(
  const AssignExpr& expr,
  const JsonValue& option
)
{
  return CnfGenImpl::calc_cnf_size(expr.expr());
}


//////////////////////////////////////////////////////////////////////
// クラス CnfGenImpl
//////////////////////////////////////////////////////////////////////

// @brief 式を CNF に変換する．
void
CnfGenImpl::make_cnf(
  const Expr& expr,
  vector<SatLiteral>& assumptions
)
{
  if ( expr.is_one() ) {
    return;
  }
  if ( expr.is_zero() ) {
    // 充足不能
    // 正確には空節を追加すべき
    throw std::invalid_argument{"expr is 0"};
  }
  if ( expr.is_literal() ) {
    // リテラルの場合
    auto as = mMap.assign(expr.literal());
    auto lit = mEngine.conv_to_literal(as);
    assumptions.push_back(lit);
  }
  if ( expr.is_and() ) {
    // AND の場合
    // すべてのオペランドが成り立つ必要がある．
    auto n = expr.operand_num();
    for ( SizeType i = 0; i < n; ++ i ) {
      auto opr = expr.operand(i);
      make_cnf(opr, assumptions);
    }
  }
  // それ以外の場合は expr が成り立つ条件を表すリテラルを作る．
  auto lit = make_cnf_sub(expr);
  assumptions.push_back(lit);
}

// @brief make_cnf() の下請け関数
SatLiteral
CnfGenImpl::make_cnf_sub(
  const Expr& expr
)
{
  if ( expr.is_constant() ) {
    throw std::invalid_argument{"expr is constant"};
  }
  if ( expr.is_literal() ) {
    auto as = mMap.assign(expr.literal());
    auto lit = mEngine.conv_to_literal(as);
    return lit;
  }
  auto lit = mEngine.solver().new_variable();
  if ( expr.is_and() ) {
    auto n = expr.operand_num();
    for ( SizeType i = 0; i < n; ++ i ) {
      auto opr = expr.operand(i);
      auto lit1 = make_cnf_sub(opr);
      mEngine.solver().add_clause(~lit, lit1);
    }
  }
  if ( expr.is_or() ) {
    auto n = expr.operand_num();
    vector<SatLiteral> tmp_lits(n + 1);
    tmp_lits[0] = ~lit;
    for ( SizeType i = 0; i < n; ++ i ) {
      auto opr = expr.operand(i);
      tmp_lits[i + 1] = make_cnf_sub(opr);
    }
    mEngine.solver().add_clause(tmp_lits);
  }
  if ( expr.is_xor() ) {
    auto n = expr.operand_num();
    vector<SatLiteral> opr_lits(n);
    for ( SizeType i = 0; i < n; ++ i ) {
      auto opr = expr.operand(i);
      opr_lits[i] = make_cnf_sub(opr);
    }
    make_xor_cnf(lit, opr_lits, 0, n);
  }
  return lit;
}

// @brief XOR を表す CNF を生成する．
void
CnfGenImpl::make_xor_cnf(
  SatLiteral lit,
  const vector<SatLiteral>& opr_lits,
  SizeType begin,
  SizeType end
)
{
  auto n = end - begin;
  if ( n < 2 ) {
    throw std::invalid_argument{"n < 2"};
  }
  if ( n == 2 ) {
    auto lit1 = opr_lits[begin + 0];
    auto lit2 = opr_lits[begin + 1];
    mEngine.solver().add_clause(~lit,  lit1,  lit2);
    mEngine.solver().add_clause(~lit, ~lit1, ~lit2);
  }
  else if ( n == 3 ) {
    auto lit1 = opr_lits[begin + 0];
    auto lit2 = opr_lits[begin + 1];
    auto lit3 = opr_lits[begin + 2];
    mEngine.solver().add_clause(~lit,  lit1,  lit2,  lit3);
    mEngine.solver().add_clause(~lit, ~lit1,  lit2,  lit3);
    mEngine.solver().add_clause(~lit,  lit1, ~lit2,  lit3);
    mEngine.solver().add_clause(~lit,  lit1,  lit2, ~lit3);
  }
  else { // n >= 4
    auto nh = n / 2;
    auto lit1 = mEngine.solver().new_variable();
    auto lit2 = mEngine.solver().new_variable();
    make_xor_cnf(lit1, opr_lits, begin, nh);
    make_xor_cnf(lit2, opr_lits, nh, end);
    mEngine.solver().add_clause(~lit,  lit1,  lit2);
    mEngine.solver().add_clause(~lit, ~lit1, ~lit2);
  }
}

// @brief 式を CNF に変換した際の項数とリテラル数を計算する．
CnfSize
CnfGenImpl::calc_cnf_size(
  const Expr& expr
)
{
  if ( expr.is_one() ) {
    return CnfSize{0, 0};
  }
  if ( expr.is_zero() ) {
    // 充足不能
    // 正確には空節を追加すべき
    throw std::invalid_argument{"expr is 0"};
  }
  if ( expr.is_literal() ) {
    // リテラルの場合
    return CnfSize{0, 0};
  }
  if ( expr.is_and() ) {
    // AND の場合
    auto n = expr.operand_num();
    CnfSize ans{0, 0};
    for ( SizeType i = 0; i < n; ++ i ) {
      auto opr = expr.operand(i);
      ans += calc_cnf_size(opr);
    }
    return ans;
  }
  // それ以外の場合
  return calc_sub(expr);
}

// @brief calc_cnf_size() の下請け関数
CnfSize
CnfGenImpl::calc_sub(
  const Expr& expr
)
{
  if ( expr.is_constant() ) {
    throw std::invalid_argument{"expr is constant"};
  }
  if ( expr.is_literal() ) {
    return CnfSize{0, 0};
  }
  // 以降は多項演算
  CnfSize ans{0, 0};
  auto n = expr.operand_num();
  for ( SizeType i = 0; i < n; ++ i ) {
    auto opr = expr.operand(i);
    ans += calc_sub(opr);
  }
  if ( expr.is_and() ) {
    ans += CnfSize{n, n * 2};
  }
  if ( expr.is_or() ) {
    ans += CnfSize{1, n + 1};
  }
  if ( expr.is_xor() ) {
    auto n = expr.operand_num();
    ans += calc_xor(n);
  }
  return ans;
}

// @brief XOR 用のCNFサイズ計算関数
CnfSize
CnfGenImpl::calc_xor(
  SizeType n
)
{
  if ( n < 2 ) {
    throw std::invalid_argument{"n < 2"};
  }
  if ( n == 2 ) {
    return CnfSize{2, 6};
  }
  if ( n == 3 ) {
    return CnfSize{4, 16};
  }
  auto nh = n / 2;
  CnfSize ans{2, 6};
  ans += calc_xor(nh);
  ans += calc_xor(n - nh);
  return ans;
}

END_NAMESPACE_DRUID
