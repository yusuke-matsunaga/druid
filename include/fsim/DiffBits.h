#ifndef DIFFBITS_H
#define DIFFBITS_H

/// @file DiffBits.h
/// @brief DiffBits のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DiffBits DiffBits.h "DiffBits.h"
/// @brief 故障の影響が伝搬したかどうかを表すビットベクタ
//////////////////////////////////////////////////////////////////////
class DiffBits
{
public:

  /// @brief 空のコンストラクタ
  DiffBits() = default;

  /// @brief コンストラクタ
  ///
  /// 全て0で初期化される．
  explicit
  DiffBits(
    SizeType output_num ///< [in] 出力数
  ) : mBits(output_num, false)
  {
  }

  /// @brief デストラクタ
  ~DiffBits() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 要素数を返す．
  SizeType
  size() const
  {
    return mBits.size();
  }

  /// @brief 要素を取り出す．
  bool
  operator[](
    SizeType pos ///< [in] 位置 ( 0 <= pos < size() )
  ) const
  {
    return mBits[pos];
  }

  /// @brief 値を true にする．
  void
  set_val(
    SizeType pos ///< [in] 位置 ( 0 <= pos < size() )
  )
  {
    mBits[pos] = true;
  }

  /// @brief 値を false にする．
  void
  clear_val(
    SizeType pos ///< [in] 位置 ( 0 <= pos < size() )
  )
  {
    mBits[pos] = false;
  }

  /// @brief 値を反転する．
  void
  flip_val(
    SizeType pos ///< [in] 位置 ( 0 <= pos < size() )
  )
  {
    mBits[pos] = !mBits[pos];
  }

  /// @brief true のビットがあるか調べる．
  bool
  any() const
  {
    for ( auto v: mBits ) {
      if ( v ) {
	return true;
      }
    }
    return false;
  }

  /// @brief 等価比較演算
  bool
  operator==(
    const DiffBits& right
  ) const
  {
    return mBits == right.mBits;
  }

  /// @brief 非等価比較演算
  bool
  operator!=(
    const DiffBits& right
  ) const
  {
    return !operator==(right);
  }

  /// @brief 内容を出力する．
  void
  print(
    ostream& s
  ) const
  {
    for ( auto v: mBits ) {
      s << v;
    }
  }

  /// @brief ハッシュ関数
  SizeType
  hash() const
  {
    SizeType N = sizeof(SizeType) * 8;
    SizeType ans = 0;
    SizeType n = size();
    SizeType bits = 0;
    for ( SizeType i = 0; i < n; ++ i ) {
      bits <<= 1;
      bits |= mBits[i];
      if ( i % N == N - 1 || i == n - 1) {
	ans ^= bits;
	bits = 0;
      }
    }
    return ans;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 本体
  vector<bool> mBits;

};

/// @brief ストリーム出力演算子
inline
ostream&
operator<<(
  ostream& s,
  const DiffBits& dbits
)
{
  dbits.print(s);
  return s;
}

END_NAMESPACE_DRUID

BEGIN_NAMESPACE_STD

// DiffBits をキーとしたハッシュ関数クラス
template <>
struct hash<DRUID_NAMESPACE::DiffBits>
{
  SizeType
  operator()(
    const DRUID_NAMESPACE::DiffBits& dbits
  ) const
  {
    return dbits.hash();
  }
};

END_NAMESPACE_STD

#endif // DIFFBITS_H
