#ifndef DIFFBITSARRAY_H
#define DIFFBITSARRAY_H

/// @file DiffBitsArray.h
/// @brief DiffBitsArray のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "PackedVal.h"
#include "DiffBits.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DiffBitsArray DiffBitsArray.h "DiffBitsArray.h"
/// @brief DiffBits の配列
///
/// 実際には非ゼロの出力要素ごとに PackedVal を持つ．
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

  /// @brief 全ての出力のビットパタンの OR を返す．
  PackedVal
  dbits_union() const
  {
    return mUnion;
  }

  /// @brief ビットスライスを得る．
  void
  get_slice(
    DiffBits& dbits, ///< [out] 結果の格納先
    SizeType pos     ///< [in] ビット位置
  ) const
  {
    dbits.clear();
    PackedVal mask = 1UL << pos;
    SizeType N = mPosList.size();
    for ( SizeType i = 0; i < N; ++ i ) {
      if ( mBody[i] & mask ) {
	SizeType pos = mPosList[i];
	dbits.add_output(pos);
      }
    }
  }

  /// @brief 内容をクリアする．
  void
  clear()
  {
    mPosList.clear();
    mBody.clear();
    mUnion = 0UL;
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
    mUnion |= dbits;
  }

  /// @brief 1ビット分のパタンを追加する．
  void
  add_pat(
    const DiffBits& dbits, ///< [in] ビットパタン
    SizeType pos           ///< [in] ビット位置
  )
  {
    PackedVal mask = 1UL << pos;
    unordered_map<SizeType, SizeType> pos_map;
    for ( SizeType i = 0; i < mPosList.size(); ++ i ) {
      pos_map.emplace(mPosList[i], i);
    }
    for ( SizeType i = 0; i < dbits.elem_num(); ++ i ) {
      auto oid = dbits.output(i);
      if ( pos_map.count(oid) ) {
	SizeType j = pos_map.at(oid);
	mBody[j] |= mask;
      }
      else {
	mPosList.push_back(oid);
	mBody.push_back(mask);
      }
    }
  }

  /// @brief 等価比較演算
  bool
  operator==(
    const DiffBitsArray& right
  ) const
  {
    return mPosList == right.mPosList && mBody == right.mBody;
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
    SizeType N = mPosList.size();
    SizeType ans = 0;
    for ( SizeType i = 0; i < N; ++ i ) {
      ans = (ans * 1021) + mPosList[i];
      ans = (ans * 2017) + mBody[i];
    }
    return ans;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 非ゼロ要素の出力番号のリスト
  vector<SizeType> mPosList;

  // 本体
  // サイズは mPosList と等しい
  vector<PackedVal> mBody;

  // mBody の各要素のビットワイズOR
  PackedVal mUnion{0UL};

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

#endif // DIFFBITSARRAY_H
