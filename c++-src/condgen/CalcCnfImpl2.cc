
/// @file CalcCnfImpl2.cc
/// @brief CalcCnfImpl2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "CalcCnfImpl2.h"
#include "CnfGenImpl.h"
#include "ym/BddVar.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス CalcCnfImpl2
//////////////////////////////////////////////////////////////////////

// @brief 式を CNF に変換した際の項数とリテラル数を計算する．
void
CalcCnfImpl2::run(
  const Expr& expr
)
{
  auto bdd = mBddMgr.from_expr(expr);
  if ( bdd.is_zero() ) {
    mCnfSize = CnfGenImpl::calc_cnf_size(expr);
  }
  else {
    calc_cnf_size(bdd);
    auto cnf_size1 = CnfGenImpl::calc_cnf_size(expr);
    if ( cnf_size1.clause_num < mCnfSize.clause_num ) {
      mCnfSize = cnf_size1;
    }
  }
}

// @brief BDD を CNF に変換した際の項数とリテラル数を計算する．
SizeType
CalcCnfImpl2::calc_cnf_size(
  const Bdd& bdd
)
{
  if ( bdd.is_zero() ) {
    abort();
  }
  if ( bdd.is_one() ) {
    return 0;
  }
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
  else if ( bdd0.is_one() ) {
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

END_NAMESPACE_DRUID
