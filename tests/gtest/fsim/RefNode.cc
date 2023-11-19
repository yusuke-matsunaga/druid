
/// @file RefNode.cc
/// @brief RefNode の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "RefNode.h"


BEGIN_NAMESPACE_DRUID

// @brief 出力値を計算する．
Val3
RefNode::calc_val() const
{
  switch ( mGateType ) {
  case PrimType::None: // 入力
    // なにもしない．
    break;
  case PrimType::C0: // 定数0
    mVal = Val3::_0;
    break;
  case PrimType::C1: // 定数1
    mVal = Val3::_1;
    break;
  case PrimType::Buff: // バッファ
    mVal = mFaninList[0]->get_val();
    break;
  case PrimType::Not: // インバーター
    mVal = ~mFaninList[0]->get_val();
  case PrimType::And: // AND
    mVal = Val3::_1;
    for ( auto inode: mFaninList ) {
      mVal &= inode->get_val();
    }
    break;
  case PrimType::Nand: // NAND
    mVal = Val3::_1;
    for ( auto inode: mFaninList ) {
      mVal &= inode->get_val();
    }
    mVal = ~mVal;
    break;
  case PrimType::Or: // OR
    mVal = Val3::_0;
    for ( auto inode: mFaninList ) {
      mVal |= inode->get_val();
    }
    break;
  case PrimType::Nor: // NOR
    mVal = Val3::_0;
    for ( auto inode: mFaninList ) {
      mVal |= inode->get_val();
    }
    mVal = ~mVal;
    break;
  case PrimType::Xor: // XOR
    mVal = Val3::_0;
    for ( auto inode: mFaninList ) {
      mVal ^= inode->get_val();
    }
    break;
  case PrimType::Xnor: // XNOR
    mVal = Val3::_1;
    for ( auto inode: mFaninList ) {
      mVal ^= inode->get_val();
    }
    break;
  }
  return mVal;
}

END_NAMESPACE_DRUID
