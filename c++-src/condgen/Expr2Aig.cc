
/// @file Expr2Aig.cc
/// @brief Expr2Aig の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "Expr2Aig.h"


BEGIN_NAMESPACE_DRUID

// @brief 複数の Expr を AIG に変換する
AigHandle
Expr2Aig::_conv_to_aig(
  const Expr& expr
)
{
  if ( expr.is_zero() ) {
    return mMgr.make_zero();
  }
  if ( expr.is_one() ) {
    return mMgr.make_one();
  }
  if ( expr.is_literal() ) {
    auto vid = expr.varid();
    while ( mMgr.input_num() <= vid ) {
      mMgr.make_input();
    }
    auto aig = mMgr.input(vid);
    if ( expr.is_nega_literal() ) {
      return ~aig;
    }
    else {
      return aig;
    }
  }
  auto nop = expr.operand_num();
  auto aig_list = conv_to_aig(expr.operand_list());
  if ( expr.is_and() ) {
    return make_and_tree(aig_list, 0, nop);
  }
  if ( expr.is_or() ) {
    return make_or_tree(aig_list, 0, nop);
  }
  if ( expr.is_xor() ) {
    return make_xor_tree(aig_list, 0, nop);
  }
  throw std::logic_error{"never happens"};
  mMgr.make_zero();
}

// @brief AND木を作る．
AigHandle
Expr2Aig::make_and_tree(
  const vector<AigHandle>& aig_list,
  SizeType begin,
  SizeType end
)
{
  auto n = end - begin;
  if ( n == 1 ) {
    return aig_list[begin];
  }
  if ( n == 2 ) {
    auto aig0 = aig_list[begin + 0];
    auto aig1 = aig_list[begin + 1];
    return make_and(aig0, aig1);
  }
  auto nh = (n + 1) / 2;
  auto aig0 = make_and_tree(aig_list, begin, begin + nh);
  auto aig1 = make_and_tree(aig_list, begin + nh, end);
  return make_and(aig0, aig1);
}

// @brief OR木を作る．
AigHandle
Expr2Aig::make_or_tree(
  const vector<AigHandle>& aig_list,
  SizeType begin,
  SizeType end
)
{
  auto n = end - begin;
  if ( n == 1 ) {
    return aig_list[begin];
  }
  if ( n == 2 ) {
    auto aig0 = aig_list[begin + 0];
    auto aig1 = aig_list[begin + 1];
    return make_or(aig0, aig1);
  }
  auto nh = (n + 1) / 2;
  auto aig0 = make_or_tree(aig_list, begin, begin + nh);
  auto aig1 = make_or_tree(aig_list, begin + nh, end);
  return make_or(aig0, aig1);
}

// @brief XOR木を作る．
AigHandle
Expr2Aig::make_xor_tree(
  const vector<AigHandle>& aig_list,
  SizeType begin,
  SizeType end
)
{
  auto n = end - begin;
  if ( n == 1 ) {
    return aig_list[begin];
  }
  if ( n == 2 ) {
    auto aig0 = aig_list[begin + 0];
    auto aig1 = aig_list[begin + 1];
    return make_xor(aig0, aig1);
  }
  auto nh = (n + 1) / 2;
  auto aig0 = make_or_tree(aig_list, begin, begin + nh);
  auto aig1 = make_or_tree(aig_list, begin + nh, end);
  return make_xor(aig0, aig1);
}

END_NAMESPACE_DRUID
