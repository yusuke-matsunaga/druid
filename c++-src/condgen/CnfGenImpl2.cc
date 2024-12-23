
/// @file CnfGenImpl2.cc
/// @brief CnfGenImpl2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "CnfGen.h"
#include "CnfGenImpl2.h"
#include "StructEngine.h"
#include "TpgNetwork.h"
#include "ym/BddVar.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス CnfGenImpl2
//////////////////////////////////////////////////////////////////////

// @brief 式を CNF に変換する．
void
CnfGenImpl2::make_cnf(
  const Expr& expr,
  vector<SatLiteral>& assumptions
)
{
  auto bdd = conv_to_bdd(expr);
  bdd_to_cnf(bdd, assumptions);
}

// @brief 式を CNF に変換した際の項数とリテラル数を計算する．
void
CnfGenImpl2::calc_cnf_size(
  const Expr& expr
)
{
  auto bdd = conv_to_bdd(expr);
  calc_cnf_size(bdd);
}

// @brief 論理式を BDD に変換する．
Bdd
CnfGenImpl2::conv_to_bdd(
  const Expr& expr
)
{
  if ( expr.is_zero() ) {
    return mBddMgr.zero();
  }
  if ( expr.is_one() ) {
    return mBddMgr.one();
  }
  if ( expr.is_literal() ) {
    auto lit = expr.literal();
    auto varid = lit.varid();
    auto inv = lit.is_negative();
    auto bdd = static_cast<Bdd>(mBddMgr.variable(varid));
    if ( inv ) {
      bdd = ~bdd;
    }
    return bdd;
  }
  auto n = expr.operand_num();
  if ( expr.is_and() ) {
    auto bdd = mBddMgr.one();
    for ( SizeType i = 0; i < n; ++ i ) {
      bdd &= conv_to_bdd(expr.operand(i));
    }
    return bdd;
  }
  if ( expr.is_or() ) {
    auto bdd = mBddMgr.zero();
    for ( SizeType i = 0; i < n; ++ i ) {
      bdd |= conv_to_bdd(expr.operand(i));
    }
    return bdd;
  }
  if ( expr.is_xor() ) {
    auto bdd = mBddMgr.zero();
    for ( SizeType i = 0; i < n; ++ i ) {
      bdd ^= conv_to_bdd(expr.operand(i));
    }
    return bdd;
  }
  ASSERT_NOT_REACHED;
  return mBddMgr.zero();
}

// @brief BDD を CNF 式に変換する．
void
CnfGenImpl2::bdd_to_cnf(
  const Bdd& bdd,
  vector<SatLiteral>& lit_list
)
{
  if ( mResultDict.count(bdd) > 0 ) {
    lit_list = mResultDict.at(bdd);
    return;
  }
  Bdd bdd0;
  Bdd bdd1;
  auto root_var = bdd.root_decomp(bdd0, bdd1);
  auto lit = conv_to_literal(root_var);
  if ( bdd0.is_zero() ) {
    if ( bdd1.is_one() ) {
      lit_list.push_back(lit);
    }
    else {
      bdd_to_cnf(bdd1, lit_list);
      lit_list.push_back(lit);
    }
  }
  else if ( bdd0.is_one() ) {
    if ( bdd1.is_zero() ) {
      lit_list.push_back(~lit);
    }
    else {
      vector<SatLiteral> lit_list1;
      bdd_to_cnf(bdd1, lit_list1);
      auto new_lit = mEngine.solver().new_variable(true);
      for ( auto lit1: lit_list1 ) {
	mEngine.solver().add_clause(~new_lit, ~lit, lit1);
      }
      lit_list.push_back(new_lit);
    }
  }
  else {
    if ( bdd1.is_zero() ) {
      // bdd0 は定数ではない．
      bdd_to_cnf(bdd0, lit_list);
      lit_list.push_back(~lit);
    }
    else if ( bdd1.is_one() ) {
      vector<SatLiteral> lit_list0;
      bdd_to_cnf(bdd0, lit_list0);
      auto new_lit = mEngine.solver().new_variable(true);
      for ( auto lit1: lit_list0 ) {
	mEngine.solver().add_clause(~new_lit, lit, lit1);
      }
      lit_list.push_back(new_lit);
    }
    else {
      vector<SatLiteral> lit_list0;
      bdd_to_cnf(bdd0, lit_list0);
      vector<SatLiteral> lit_list1;
      bdd_to_cnf(bdd1, lit_list1);
      auto new_lit = mEngine.solver().new_variable(true);
      for ( auto lit1: lit_list0 ) {
	mEngine.solver().add_clause(~new_lit, lit, lit1);
      }
      for ( auto lit1: lit_list1 ) {
	mEngine.solver().add_clause(~new_lit, ~lit, lit1);
      }
      lit_list.push_back(new_lit);
    }
  }
  mResultDict.emplace(bdd, lit_list);
}

// @brief BDD を CNF に変換した際の項数とリテラル数を計算する．
SizeType
CnfGenImpl2::calc_cnf_size(
  const Bdd& bdd
)
{
  if ( mSizeDict.count(bdd) > 0 ) {
    // すでに計算済み
    return mSizeDict.at(bdd);
  }
  Bdd bdd0;
  Bdd bdd1;
  auto root_var = bdd.root_decomp(bdd0, bdd1);
  SizeType result = 0;
  if ( bdd0.is_zero() ) {
    if ( bdd1.is_one() ) {
      ;
    }
    else {
      result = calc_cnf_size(bdd1) + 1;
    }
  }
  if ( bdd0.is_one() ) {
    if ( bdd1.is_zero() ) {
      ;
    }
    else {
      auto result1 = calc_cnf_size(bdd1);
      mCnfSize += CnfSize{result1, result1 * 3};
      result = 1;
    }
  }
  else {
    if ( bdd1.is_zero() ) {
      // bdd0 は定数ではない．
      result = calc_cnf_size(bdd0) + 1;
    }
    else if ( bdd1.is_one() ) {
      auto result0 = calc_cnf_size(bdd0);
      mCnfSize += CnfSize{result0, result0 * 3};
      result = 1;
    }
    else {
      auto result0 = calc_cnf_size(bdd0);
      auto result1 = calc_cnf_size(bdd1);
      mCnfSize += CnfSize{result0, result0 * 3};
      mCnfSize += CnfSize{result1, result1 * 3};
      result = 1;
    }
  }
  mSizeDict.emplace(bdd, result);
  return result;
}

// @brief 論理式のリテラルを SAT ソルバのリテラルに変換する．
SatLiteral
CnfGenImpl2::conv_to_literal(
  const BddVar& var
) const
{
  // 回りくどい変換を行う．
  auto varid = var.id();
  auto node_id = varid / 2;
  auto node = mEngine.network().node(node_id);
  auto time = static_cast<int>(varid % 2);
  auto as = Assign{node, time, true};
  return mEngine.conv_to_literal(as);
}

END_NAMESPACE_DRUID
