#ifndef DFFVECTOR_H
#define DFFVECTOR_H

/// @file DffVector.h
/// @brief DffVector のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "BitVector.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DffVector DffVector.h "DffVector.h"
/// @brief FF用のビットベクタ
/// @ingroup TypesGroup
///
/// 実は BitVector の別名だが他のクラスと区別するために typedef は使わない．
//////////////////////////////////////////////////////////////////////
class DffVector :
  public BitVector
{
public:

  /// @brief コンストラクタ
  ///
  /// 内容は X で初期化される．
  explicit
  DffVector(
    SizeType len = 0  ///< [in] ベクタ長
  ) : BitVector{len}
  {
  }

  /// @brief コピーコンストラクタ
  DffVector(
    const DffVector& src  ///< [in] コピー元のソース
  ) : BitVector{src}
  {
  }

  /// @brief コピーコンストラクタ
  explicit
  DffVector(
    const BitVector& src  ///< [in] コピー元のソース
  ) : BitVector{src}
  {
  }

  /// @brief コピー代入演算子
  DffVector&
  operator=(
    const DffVector& src  ///< [in] コピー元のソース
  )
  {
    BitVector::operator=(src);

    return *this;
  }

  /// @brief デストラクタ
  ~DffVector() = default;


public:

  /// @brief マージする．
  /// @return マージ結果を返す．
  ///
  /// left と right がコンフリクトしている時の結果は不定
  DffVector
  operator&(
    const DffVector& right ///< [in] オペランド2
  ) const
  {
    return DffVector(BitVector(*this).operator&=(right));
  }

};

END_NAMESPACE_DRUID

#endif // DFFVECTOR_H
