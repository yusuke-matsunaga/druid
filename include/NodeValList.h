#ifndef NODEVALLIST_H
#define NODEVALLIST_H

/// @file NodeValList.h
/// @brief NodeValList のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "satpg.h"
#include "NodeVal.h"


BEGIN_NAMESPACE_SATPG

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
  NodeValList();

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
  ~NodeValList();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容を空にする．
  void
  clear();

  /// @brief 値を追加する．
  /// @param[in] node ノード
  /// @param[in] time 時刻 ( 0 or 1 )
  /// @param[in] val 値
  void
  add(const TpgNode* node,
      int time,
      bool val);

  /// @brief 値を追加する．
  /// @param[in] node_val 値の割り当て情報
  void
  add(NodeVal node_val);

  /// @brief マージする．
  /// @param[in] src_list マージするリスト
  ///
  /// 矛盾する割当があった場合の動作は不定
  void
  merge(const NodeValList& src_list);

  /// @brief 差分を計算する．
  /// @param[in] src_list 差分の対象のリスト
  void
  diff(const NodeValList& src_list);

  /// @brief 要素数を返す．
  int
  size() const;

  /// @brief 要素を返す．
  /// @param[in] pos 位置 ( 0 <= pos < size() )
  NodeVal
  elem(int pos) const;

  /// @brief add(NodeVal) の別名
  /// @param[in] node_val 追加する要素
  NodeValList&
  operator+=(NodeVal node_val);

  /// @brief merge() の別名
  /// @param[in] src_list 追加する要素のリスト
  NodeValList&
  operator+=(const NodeValList& src_list);

  /// @brief diff() の別名
  /// @param[in] src_list 差分の対象のリスト
  NodeValList&
  operator-=(const NodeValList& src_list);

  /// @brief elem() の別名
  /// @param[in] pos 位置 ( 0 <= pos < size() )
  NodeVal
  operator[](int pos) const;

  /// @brief 矛盾した内容になっていないかチェックする．
  /// @return 正しければ true を返す．
  ///
  /// 具体的には同じノードで異なる値がある場合にエラーとなる．
  /// この関数はソートされている前提で動作する．
  bool
  sanity_check() const;

  /// @brief 先頭の反復子を返す．
  vector<NodeVal>::const_iterator
  begin() const;

  /// @brief 末尾の反復子を返す．
  vector<NodeVal>::const_iterator
  end() const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ソートされた状態にする．
  void
  _sort() const;


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

/// @brief 2つの割当リストが矛盾しているか調べる．
bool
check_conflict(const NodeValList& src_list1,
	       const NodeValList& src_list2);

/// @brief 包含関係を調べる．
bool
check_contain(const NodeValList& src_list1,
	      const NodeValList& src_list2);

/// @brief 2つの割当リストを比較する．
/// @retval -1 矛盾した割当がある．
/// @retval  0 無関係
/// @retval  1 src_list1 が src_list2 を含む．
/// @retval  2 src_list2 が src_list1 を含む．
/// @retval  3 等しい
int
compare(const NodeValList& src_list1,
	const NodeValList& src_list2);

/// @brief 2つのリストを結合して新しいリストを返す．
NodeValList
operator+(const NodeValList& src_list1,
	  const NodeValList& src_list2);

/// @brief 2つのリストの差分を計算して新しいリストを返す．
NodeValList
operator-(const NodeValList& src_list1,
	  const NodeValList& src_list2);

/// @brief 割当の内容を出力する．
/// @param[in] s 出力先のストリーム
/// @param[in] nv 値の割り当て
/// @return s を返す．
ostream&
operator<<(ostream& s,
	   NodeVal nv);

/// @brief 割当リストの内容を出力する．
/// @param[in] s 出力先のストリーム
/// @param[in] src_list 値の割り当てリスト
/// @return s を返す．
ostream&
operator<<(ostream& s,
	   const NodeValList& src_list);


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
inline
NodeValList::NodeValList() :
  mDirty(false)
{
}

#if 0
// @brief コピーコンストラクタ
inline
NodeValList::NodeValList(const NodeValList& src) :
  mDirty(src.mDirty),
  mAsList(src.mAsList)
{
}

// @brief ムーブコンストラクタ
inline
NodeValList::NodeValList(NodeValList&& src) :
  mDirty(src.mDirty),
  mAsList(src.mAsList)
{
  // vector<> はムーブコンストラクタを持っている．
}

// @brief コピー代入演算子
inline
NodeValList&
NodeValList::operator=(const NodeValList& src)
{
  mDirty = src.mDirty;
  mAsList = src.mAsList;

  return *this;
}

// @brief ムーブ代入演算子
inline
NodeValList&
NodeValList::operator=(NodeValList&& src)
{
  mDirty = src.mDirty;
  // vector<> はムーブ代入演算子を持っている．
  mAsList = src.mAsList;

  return *this;
}
#endif

// @brief デストラクタ
inline
NodeValList::~NodeValList()
{
}

// @brief 内容を空にする．
inline
void
NodeValList::clear()
{
  mDirty = true;
  mAsList.clear();
}

// @brief 要素数を返す．
inline
int
NodeValList::size() const
{
  return mAsList.size();
}

// @brief ソートする．
inline
void
NodeValList::_sort() const
{
  if ( mDirty ) {
    std::sort(mAsList.begin(), mAsList.end());
    mDirty = false;
  }
}

// @brief 値を追加する．
// @param[in] node ノード
// @param[in] time 時刻(0 or 1)
// @param[in] val 値
inline
void
NodeValList::add(const TpgNode* node,
		 int time,
		 bool val)
{
  add(NodeVal(node, time, val));
}

// @brief 値を追加する．
// @param[in] node_val 値の割り当て情報
inline
void
NodeValList::add(NodeVal node_val)
{
  mAsList.push_back(node_val);
  mDirty = true;
}

// @brief 要素を返す．
// @param[in] pos 位置 ( 0 <= pos < size() )
inline
NodeVal
NodeValList::elem(int pos) const
{
  ASSERT_COND( pos >= 0 && pos < size() );

  _sort();
  return mAsList[pos];
}

// @brief add(NodeVal) の別名
// @param[in] node_val 追加する要素
inline
NodeValList&
NodeValList::operator+=(NodeVal node_val)
{
  add(node_val);

  return *this;
}

// @brief merge() の別名
// @param[in] src_list 追加する要素のリスト
inline
NodeValList&
NodeValList::operator+=(const NodeValList& src_list)
{
  merge(src_list);

  return *this;
}

// @brief diff() の別名
// @param[in] src_list 差分の対象のリスト
inline
NodeValList&
NodeValList::operator-=(const NodeValList& src_list)
{
  diff(src_list);

  return *this;
}

// @brief elem() の別名
// @param[in] pos 位置 ( 0 <= pos < size() )
inline
NodeVal
NodeValList::operator[](int pos) const
{
  return elem(pos);
}

// @brief 先頭の反復子を返す．
inline
vector<NodeVal>::const_iterator
NodeValList::begin() const
{
  _sort();
  return mAsList.begin();
}

// @brief 末尾の反復子を返す．
inline
vector<NodeVal>::const_iterator
NodeValList::end() const
{
  _sort();
  return mAsList.end();
}

// @brief 2つの割当リストが矛盾しているか調べる．
inline
bool
check_conflict(const NodeValList& src_list1,
	       const NodeValList& src_list2)
{
  return compare(src_list1, src_list2) == -1;
}

// @brief 包含関係を調べる．
inline
bool
check_contain(const NodeValList& src_list1,
	      const NodeValList& src_list2)
{
  return (compare(src_list1, src_list2) & 1) == 1;
}

// @brief 2つのリストを結合して新しいリストを返す．
inline
NodeValList
operator+(const NodeValList& src_list1,
	  const NodeValList& src_list2)
{
  NodeValList tmp(src_list1);
  return tmp.operator+=(src_list2);
}

// @brief 2つのリストの差分を計算して新しいリストを返す．
inline
NodeValList
operator-(const NodeValList& src_list1,
	  const NodeValList& src_list2)
{
  NodeValList tmp(src_list1);
  return tmp.operator-=(src_list2);
}

END_NAMESPACE_SATPG

#endif // NODEVALLIST_H
