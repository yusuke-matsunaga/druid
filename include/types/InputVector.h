#ifndef INPUTVECTOR_H
#define INPUTVECTOR_H

/// @file InputVector.h
/// @brief InputVector のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "BitVector.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class InputVector InputVector.h "InputVector.h"
/// @brief 入力用のビットベクタ
/// @ingroup TypesGroup
///
/// 実は BitVector の別名だが他のクラスと区別するために typedef は使わない．
//////////////////////////////////////////////////////////////////////
class InputVector :
  public BitVector
{
public:

  /// @brief コンストラクタ
  ///
  /// 内容は X で初期化される．
  explicit
  InputVector(
    SizeType len = 0  ///< [in] ベクタ長
  ) : BitVector(len)
  {
  }

  /// @brief コピーコンストラクタ
  InputVector(
    const InputVector& src  ///< [in] コピー元のソース
  ) : BitVector{src}
  {
  }

  /// @brief コピーコンストラクタ
  explicit
  InputVector(
    const BitVector& src  ///< [in] コピー元のソース
  ) : BitVector{src}
  {
  }

  /// @brief コピー代入演算子
  InputVector&
  operator=(
    const InputVector& src  ///< [in] コピー元のソース
  )
  {
    BitVector::operator=(src);

    return *this;
  }

  /// @brief デストラクタ
  ~InputVector() = default;


public:

  /// @brief マージする．
  /// @return マージ結果を返す．
  ///
  /// left と right がコンフリクトしている時の結果は不定
  InputVector
  operator&(
    const InputVector& right ///< [in] オペランド2
  ) const
  {
    return InputVector(BitVector(*this).operator&=(right));
  }

};

END_NAMESPACE_DRUID

#endif // INPUTVECTOR_H
