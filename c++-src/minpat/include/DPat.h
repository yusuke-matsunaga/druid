#ifndef DPAT_H
#define DPAT_H

/// @file DPat.h
/// @brief DPat のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "types/PackedVal.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DPat DPat.h "DPat.h"
/// @brief 故障の検出条件を表すビットベクタ
//////////////////////////////////////////////////////////////////////
class DPat
{
public:

  /// @brief コンストラクタ
  DPat(
    SizeType size ///< [in] 要素数
  ) : mSize{size},
      mBody((size + PV_BITLEN - 1) / PV_BITLEN, PV_ALL0)
  {
  }

  /// @brief コンストラクタ
  DPat(
    SizeType size, ///< [in] 要素数
    PackedVal pat0 ///< [in] パタン
  ) : DPat(size)
  {
    if ( size > 0 ) {
      mBody[0] = pat0;
    }
  }

  /// @brief コンストラクタ
  DPat(
    SizeType size,                         ///< [in] 要素数
    const std::vector<PackedVal>& pat_list ///< [in] パタンのリスト
  ) : DPat(size)
  {
    auto nb = mBody.size();
    auto np = pat_list.size();
    auto n = std::min(nb, np);
    for ( SizeType i = 0; i < n; ++ i ) {
      mBody[i] = pat_list[i];
    }
  }

  /// @brief デストラクタ
  ~DPat() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 要素数を返す．
  SizeType
  size() const
  {
    return mSize;
  }

  /// @brief ビットを立てる．
  void
  set(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < size() )
  )
  {
    _check_index(pos);
    auto blk = pos / PV_BITLEN;
    auto sft = pos % PV_BITLEN;
    auto bit = static_cast<PackedVal>(1) << sft;
    mBody[blk] |= bit;
  }

  /// @brief ビットを読み出す．
  bool
  get(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < size() )
  ) const
  {
    _check_index(pos);
    auto blk = pos / PV_BITLEN;
    auto sft = pos % PV_BITLEN;
    auto bit = static_cast<PackedVal>(1) << sft;
    return (mBody[blk] & bit) == bit;
  }

  /// @brief right に包含されていたら true を返す．
  bool
  check_contained(
    const DPat& right
  ) const;

  /// @brief 1のビット数を数える．
  SizeType
  count_ones() const;

  /// @brief ハッシュ値を計算する．
  SizeType
  hash() const;

  /// @brief 内容を表す文字列を返す．
  std::string
  to_str() const;

  /// @brief 等価比較演算子
  bool
  operator==(
    const DPat& right
  ) const
  {
    return mBody == right.mBody;
  }

  /// @brief 非等価比較演算子
  bool
  operator!=(
    const DPat& right
  ) const
  {
    return !operator==(right);
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief インデックスが範囲内かどうかチェックする．
  void
  _check_index(
    SizeType pos
  ) const
  {
    if ( pos >= size() ) {
      throw std::out_of_range{"pos is out of range"};
    }
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // サイズ
  SizeType mSize;

  // 本体
  std::vector<PackedVal> mBody;

};

END_NAMESPACE_DRUID

BEGIN_NAMESPACE_STD

// DPat をキーにしたハッシュ関数のクラスの定義
template <>
struct hash<DRUID_NAMESPACE::DPat>
{
  SizeType
  operator()(
    const DRUID_NAMESPACE::DPat& dpat
  ) const
  {
    return dpat.hash();
  }
};

END_NAMESPACE_STD

#endif // DPAT_H
