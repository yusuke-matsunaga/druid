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
  ) : mBody((size + PV_BITLEN - 1) / PV_BITLEN, PV_ALL0)
  {
  }

  /// @brief デストラクタ
  ~DPat() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ビットを立てる．
  void
  set(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < size() )
  )
  {
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
    auto blk = pos / PV_BITLEN;
    auto sft = pos % PV_BITLEN;
    auto bit = static_cast<PackedVal>(1) << sft;
    return (mBody[blk] & bit) == bit;
  }

  /// @brief right に包含されていたら true を返す．
  bool
  check_contain(
    const DPat& right
  ) const
  {
    auto n = mBody.size();
    for ( SizeType i = 0; i < n; ++ i ) {
      auto p1 = mBody[i];
      auto p2 = right.mBody[i];
      if ( (p1 & p2) != p1 ) {
	return false;
      }
    }
    return true;
  }

  /// @brief ハッシュ値を計算する．
  SizeType
  hash() const
  {
    SizeType val = 0;
    for ( auto pat: mBody ) {
      val = val * 13 + pat;
    }
    return (val * val) >> 16;
  }

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


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

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
