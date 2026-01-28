#ifndef DIFFBITS_H
#define DIFFBITS_H

/// @file DiffBits.h
/// @brief DiffBits のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DiffBits DiffBits.h "DiffBits.h"
/// @brief 故障の影響が伝搬したかどうかを表すビットベクタ
/// @ingroup FsimGroup
///
/// 意味的には出力数ぶんのビットベクタだが実際にはほぼゼロなので
/// 非ゼロ要素の番号のリストで表す．
//////////////////////////////////////////////////////////////////////
class DiffBits
{
public:

  /// @brief 空のコンストラクタ
  DiffBits() = default;

  /// @brief デストラクタ
  ~DiffBits() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 非ゼロの出力数を返す．
  SizeType
  elem_num() const
  {
    return mPosList.size();
  }

  /// @brief 非ゼロの出力番号を返す．
  SizeType
  output(
    SizeType pos ///< [in] 位置 ( 0 <= pos < elem_num() )
  ) const
  {
    return mPosList[pos];
  }

  /// @brief 内容をクリアする．
  void
  clear()
  {
    mPosList.clear();
  }

  /// @brief 出力番号を追加する．
  void
  add_output(
    SizeType output ///< [in] 出力番号
  )
  {
    mPosList.push_back(output);
  }

  /// @brief 内容をソートする．
  void
  sort()
  {
    std::sort(mPosList.begin(), mPosList.end());
  }

  /// @brief ソートした内容を返す．
  DiffBits
  sorted() const
  {
    DiffBits ans{*this};
    ans.sort();
    return ans;
  }

  /// @brief 等価比較演算
  bool
  operator==(
    const DiffBits& right
  ) const
  {
    return mPosList == right.mPosList;
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
    std::ostream& s
  ) const
  {
    for ( auto pos: mPosList ) {
      s << " " << pos;
    }
  }

  /// @brief ハッシュ関数
  SizeType
  hash() const
  {
    SizeType ans = 0;
    for ( auto pos: mPosList ) {
      ans = (ans * 1021) + pos;
    }
    return ans;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 本体
  std::vector<SizeType> mPosList;

};

/// @brief ストリーム出力演算子
inline
std::ostream&
operator<<(
  std::ostream& s,
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
