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
//////////////////////////////////////////////////////////////////////
class LFSR
{
public:

  /// @brief コンストラクタ
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
    auto v = Val3::_0;
    for ( auto p: mTapList ) {
      auto v1 = mBits.val(p);
      v = v ^ v1;
    }
    mBits.lshift(v);
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
