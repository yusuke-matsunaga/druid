#ifndef NODEVALLIST_H
#define NODEVALLIST_H

/// @file NodeValList.h
/// @brief NodeValList のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "NodeVal.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class NodeValList NodeValList.h "td/NodeValList.h"
/// @brief ノードに対する値の割当を記録するクラス
///
/// * このクラスのメソッドはすべてソートされていると仮定している．
/// * 実際には内部で mDirty というフラグを用意して内容が変更されたら
///   mDirty を true にしておく．
///   内容を参照する際に mDirty が true なら _sort() を呼ぶ．
//////////////////////////////////////////////////////////////////////
class NodeValList
{
public:

  /// @brief コンストラクタ
  ///
  /// 空のリストが生成される．
  NodeValList(
  ) : mDirty{false}
  {
  }

  /// @brief コピーコンストラクタ
  NodeValList(const NodeValList& src) = default;

  /// @brief ムーブコンストラクタ
  NodeValList(NodeValList&& src) = default;

  /// @brief コピー代入演算子
  NodeValList&
  operator=(const NodeValList& src) = default;

  /// @brief ムーブ代入演算子
  NodeValList&
  operator=(NodeValList&& src) = default;

  /// @brief デストラクタ
  ~NodeValList() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容を空にする．
  void
  clear()
  {
    mDirty = true;
    mAsList.clear();
  }

  /// @brief 値を追加する．
  void
  add(
    const TpgNode* node, ///< [in] ノード
    int time,		 ///< [in] 時刻 ( 0 or 1 )
    bool val		 ///< [in] 値
  )
  {
    add(NodeVal(node, time, val));
  }

  /// @brief 値を追加する．
  void
  add(
    NodeVal node_val  ///< [in] 値の割り当て情報
  )
  {
    mAsList.push_back(node_val);
    mDirty = true;
  }

  /// @brief マージする．
  ///
  /// 矛盾する割当があった場合の動作は不定
  void
  merge(
    const NodeValList& src_list  ///< [in] マージするリスト
  );

  /// @brief 差分を計算する．
  void
  diff(
    const NodeValList& src_list  ///< [in] 差分の対象のリスト
  );

  /// @brief 要素数を返す．
  SizeType
  size() const
  {
    return mAsList.size();
  }

  /// @brief 要素を返す．
  NodeVal
  elem(
    SizeType pos  ///< [in] 位置 ( 0 <= pos < size() )
  ) const
  {
    ASSERT_COND( pos >= 0 && pos < size() );

    _sort();
    return mAsList[pos];
  }

  /// @brief add(NodeVal) の別名
  NodeValList&
  operator+=(
    NodeVal node_val  ///< [in] 追加する要素
  )
  {
    add(node_val);

    return *this;
  }

  /// @brief merge() の別名
  NodeValList&
  operator+=(
    const NodeValList& src_list  ///< [in] 追加する要素のリスト
  )
  {
    merge(src_list);

    return *this;
  }

  /// @brief diff() の別名
  NodeValList&
  operator-=(
    const NodeValList& src_list  ///< [in] 差分の対象のリスト
  )
  {
    diff(src_list);

    return *this;
  }

  /// @brief elem() の別名
  NodeVal
  operator[](
    int pos  ///< [in] 位置 ( 0 <= pos < size() )
  ) const
  {
    return elem(pos);
  }

  /// @brief 矛盾した内容になっていないかチェックする．
  /// @return 正しければ true を返す．
  ///
  /// 具体的には同じノードで異なる値がある場合にエラーとなる．
  /// この関数はソートされている前提で動作する．
  bool
  sanity_check() const;

  /// @brief 先頭の反復子を返す．
  vector<NodeVal>::const_iterator
  begin() const
  {
    _sort();
    return mAsList.begin();
  }

  /// @brief 末尾の反復子を返す．
  vector<NodeVal>::const_iterator
  end() const
  {
    _sort();
    return mAsList.end();
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ソートされた状態にする．
  void
  _sort() const
  {
    if ( mDirty ) {
      std::sort(mAsList.begin(), mAsList.end());
      mDirty = false;
    }
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ソートが必要かを表すフラグ
  mutable
  bool mDirty;

  // 値割り当てのリスト
  mutable
  vector<NodeVal> mAsList;

};

/// @brief 2つの割当リストを比較する．
/// @retval -1 矛盾した割当がある．
/// @retval  0 無関係
/// @retval  1 src_list1 が src_list2 を含む．
/// @retval  2 src_list2 が src_list1 を含む．
/// @retval  3 等しい
int
compare(
  const NodeValList& src_list1,
  const NodeValList& src_list2
);

/// @brief 2つの割当リストが矛盾しているか調べる．
inline
bool
check_conflict(
  const NodeValList& src_list1,
  const NodeValList& src_list2
)
{
  return compare(src_list1, src_list2) == -1;
}

/// @brief 包含関係を調べる．
inline
bool
check_contain(
  const NodeValList& src_list1,
  const NodeValList& src_list2
)
{
  return (compare(src_list1, src_list2) & 1) == 1;
}

/// @brief 2つのリストを結合して新しいリストを返す．
inline
NodeValList
operator+(
  const NodeValList& src_list1,
  const NodeValList& src_list2
)
{
  NodeValList tmp(src_list1);
  return tmp.operator+=(src_list2);
}

/// @brief 2つのリストの差分を計算して新しいリストを返す．
inline
NodeValList
operator-(
  const NodeValList& src_list1,
  const NodeValList& src_list2
)
{
  NodeValList tmp(src_list1);
  return tmp.operator-=(src_list2);
}

/// @brief 割当の内容を出力する．
/// @return s を返す．
ostream&
operator<<(
  ostream& s, ///< [in] 出力先のストリーム
  NodeVal nv  ///< [in] 値の割り当て
);

/// @brief 割当リストの内容を出力する．
/// @return s を返す．
ostream&
operator<<(
  ostream& s,                 ///< [in] 出力先のストリーム
  const NodeValList& src_list ///< [in] 値の割り当てリスト
);

END_NAMESPACE_DRUID

#endif // NODEVALLIST_H
