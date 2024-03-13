#ifndef LFSR_H
#define LFSR_H

/// @file LFSR.h
/// @brief LFSR のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "BitVector.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class LFSR LFSR.h "LFSR.h"
/// @brief LFSR(Linear Feedback Shift Register) を表すクラス
///
/// x^n + x^i1 + x^i2 + ... + 1 = 0 の形の原始方程式を利用した LFSR
/// 原始方程式の性質上，常に x^n と x^0 = 1 の係数は1である．
//////////////////////////////////////////////////////////////////////
class LFSR
{
public:

  /// @brief コンストラクタ
  ///
  /// タップ位置のリストに最上位ビットは含まない．
  LFSR(
    SizeType bitlen,                 ///< [in] ビット長
    const vector<SizeType>& tap_list ///< [in] タップ位置のリスト
  ) : mTapList{tap_list},
      mBits(bitlen)
  {
    // mBits の内容は不定
  }

  /// @brief デストラクタ
  ~LFSR() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief タップ位置のリストを返す．
  ///
  /// タップ位置のリストに最上位ビットは含まれない．
  const vector<SizeType>&
  tap_list() const
  {
    return mTapList;
  }

  /// @brief ビット長を返す．
  SizeType
  bitlen() const
  {
    return mBits.len();
  }

  /// @brief 内容を返す．
  const BitVector&
  bits() const
  {
    return mBits;
  }

  /// @brief 内容をセットする．
  void
  set_bits(
    const BitVector& src ///< [in] 元の内容
  )
  {
    mBits = src;
  }

  /// @brief シフト動作を行う．
  void
  shift()
  {
    auto v = mBits.val(mBits.len() - 1);
    for ( auto p: mTapList ) {
      auto v1 = mBits.val(p);
      v = v ^ v1;
    }
    mBits.lshift(v);
  }

  /// @brief 逆方向にシフト動作を行う．
  void
  rshift()
  {
    auto v = mBits.val(0);
    for ( auto p: mTapList ) {
      auto v1 = mBits.val(p + 1);
      v = v ^ v1;
    }
    mBits.rshift(v);
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // タップ位置のリスト
  vector<SizeType> mTapList;

  // 内容を表すビットベクタ
  BitVector mBits;

};

END_NAMESPACE_DRUID

#endif // LFSR_H
