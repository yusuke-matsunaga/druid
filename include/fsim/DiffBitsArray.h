#ifndef DIFFBITSARRAY_H
#define DIFFBITSARRAY_H

/// @file DiffBitsArray.h
/// @brief DiffBitsArray のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"


BEGIN_NAMESPACE_DRUID

class DiffBits;

//////////////////////////////////////////////////////////////////////
/// @class DiffBitsArray DiffBitsArray.h "DiffBitsArray.h"
/// @brief DiffBits の排列
///
/// 実際にはゼロ要素を持たない．
//////////////////////////////////////////////////////////////////////
class DiffBitsArray
{
public:

  /// @brief コンストラクタ
  DiffBitsArray() = default;

  /// @brief デストラクタ
  ~DiffBitsArray() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 非ゼロの要素数を返す．
  SizeType
  elem_num() const
  {
    return mPosList.size();
  }

  /// @brief 非ゼロの出力番号を返す．
  SizeType
  output(
    SizeType pos ///< [in] 位置番号( 0 <= pos < elem_num() )
  ) const
  {
    return mPosList[pos];
  }

  /// @brief 故障伝搬ビットパタンを返す．
  PackedVal
  dbits(
    SizeType pos ///< [in] 位置番号( 0 <= pos < elem_num() )
  ) const
  {
    return mBody[pos];
  }

  /// @brief 内容をクリアする．
  void
  clear()
  {
    mPosList.clear();
    mBody.clear();
  }

  /// @brief 出力のビットパタンを追加する．
  void
  add_output(
    SizeType output, ///< [in] 出力番号
    PackedVal dbits  ///< [in] 故障伝搬ビットパタン
  )
  {
    mPosList.push_back(output);
    mBody.push_back(dbits);
  }

  /// @brief 等価比較演算
  bool
  operator==(
    const DiffBitsArray& right
  ) const
  {
    if ( mOutputNum != right.mOutputNum ) {
      return false;
    }
    if ( mPatNum != right.mPatNum ) {
      return false;
    }
    if ( mPosList != right.mPosList ) {
      return false;
    }
    return mBody != right.mBody;
  }

  /// @brief 非等価比較演算
  bool
  operator!=(
    const DiffBitsArray& right
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
    SizeType N = mPosList.size();
    for ( SizeType i = 0; i < N; ++ i ) {
      s << " " << mPosList[i] << ": " << hex << setw(16) << mBody[i] << dec;
    }
  }

  /// @brief ハッシュ関数
  SizeType
  hash() const
  {
    SizeType N = sizeof(SizeType) * 8;
    SizeType ans = 0;
#if 0
    SizeType n = mBits.size();
    SizeType bits = 0;
    for ( SizeType i = 0; i < n; ++ i ) {
      bits <<= 1;
      bits |= mBits[i];
      if ( i % N == N - 1 || i == n - 1) {
	ans ^= bits;
	bits = 0;
      }
    }
#endif
    return ans;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 出力数
  SizeType mOutputNum;

  // 非ゼロ要素の出力番号のリスト
  vector<SizeType> mPosList;

  // 本体
  // サイズは mPosList と等しい
  vector<PackedVal> mBody;

};

/// @brief ストリーム出力演算子
inline
ostream&
operator<<(
  ostream& s,
  const DiffBitsArray& dbits
)
{
  dbits.print(s);
  return s;
}

END_NAMESPACE_DRUID

BEGIN_NAMESPACE_STD

// DiffBitsArray をキーとしたハッシュ関数クラス
template <>
struct hash<DRUID_NAMESPACE::DiffBitsArray>
{
  SizeType
  operator()(
    const DRUID_NAMESPACE::DiffBitsArray& dbits
  ) const
  {
    return dbits.hash();
  }
};

END_NAMESPACE_STD

#else
#include "DiffBitsArrayNew.h"

BEGIN_NAMESPACE_DRUID

using DiffBitsArray = DiffBitsArrayNew;

END_NAMESPACE_DRUID
#endif

#endif // DIFFBITSARRAY_H
