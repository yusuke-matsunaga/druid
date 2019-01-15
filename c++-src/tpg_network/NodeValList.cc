
/// @file NodeValList.cc
/// @brief NodeValList の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "NodeValList.h"
#include "TpgNode.h"


BEGIN_NAMESPACE_DRUID

// @brief マージする．
// @param[in] src_list マージするリスト
//
// 矛盾する割当があった場合の動作は不定
void
NodeValList::merge(const NodeValList& src_list)
{
  vector<NodeVal> tmp_list;
  int n1 = mAsList.size();
  int n2 = src_list.mAsList.size();
  tmp_list.reserve(n1 + n2);
  int i1 = 0;
  int i2 = 0;
  while ( i1 < n1 && i2 < n2 ) {
    NodeVal v1 = mAsList[i1];
    NodeVal v2 = src_list.mAsList[i2];
    if ( v1 < v2 ) {
      tmp_list.push_back(v1);
      ++ i1;
    }
    else if ( v1 > v2 ) {
      tmp_list.push_back(v2);
      ++ i2;
    }
    else {
      tmp_list.push_back(v1);
      ++ i1;
      ++ i2;
    }
  }
  for ( ; i1 < n1; ++ i1) {
    NodeVal v1 = mAsList[i1];
    tmp_list.push_back(v1);
  }
  for ( ; i2 < n2; ++ i2) {
    NodeVal v2 = src_list.mAsList[i2];
    tmp_list.push_back(v2);
  }

  mAsList = tmp_list;
  mDirty = false;
}

// @brief 差分を計算する．
// @param[in] src_list 差分の対象のリスト
void
NodeValList::diff(const NodeValList& src_list)
{
  vector<NodeVal> tmp_list;
  int n1 = mAsList.size();
  int n2 = src_list.mAsList.size();
  tmp_list.reserve(n1);
  int i1 = 0;
  int i2 = 0;
  while ( i1 < n1 && i2 < n2 ) {
    NodeVal v1 = mAsList[i1];
    NodeVal v2 = src_list.mAsList[i2];
    if ( v1 < v2 ) {
      tmp_list.push_back(v1);
      ++ i1;
    }
    else if ( v1 > v2 ) {
      ++ i2;
    }
    else {
      ++ i1;
      ++ i2;
    }
  }
  for ( ; i1 < n1; ++ i1) {
    NodeVal v1 = mAsList[i1];
    tmp_list.push_back(v1);
  }

  mAsList = tmp_list;
  mDirty = false;
}

// @brief 矛盾した内容になっていないかチェックする．
// @return 正しければ true を返す．
//
// 具体的には同じノードで異なる値がある場合にエラーとなる．
// この関数はソートされている前提で動作する．
bool
NodeValList::sanity_check() const
{
  NodeVal prev(nullptr, 0, false);
  for (int i = 0; i < mAsList.size(); ++ i) {
    NodeVal nv = mAsList[i];
    if ( prev.node_time() == nv.node_time() && prev.val() != nv.val() ) {
      return false;
    }
    prev = nv;
  }
  return true;
}

// @brief 2つの割当リストを比較する．
// @retval -1 矛盾した割当がある．
// @retval  0 無関係
// @retval  1 src_list1 が src_list2 を含む．
// @retval  2 src_list2 が src_list1 を含む．
// @retval  3 等しい
int
compare(const NodeValList& src_list1,
	const NodeValList& src_list2)
{
  int n1 = src_list1.size();
  int n2 = src_list2.size();
  int i1 = 0;
  int i2 = 0;
  int ans = 3;
  while ( i1 < n1 && i2 < n2 ) {
    NodeVal nv1 = src_list1[i1];
    NodeVal nv2 = src_list2[i2];
    if ( nv1.node_time() == nv2.node_time() ) {
      if ( nv1.val() != nv2.val() ) {
	// 矛盾している．
	return -1;
      }
      else {
	// 同一
	++ i1;
	++ i2;
      }
    }
    else if ( nv1 < nv2 ) {
      // src_list1 に含まれていて src_list2 に含まれていない要素がある．
      ans &= 1;
      ++ i1;
    }
    else { //  nv1 > nv2
      // src_list2 に含まれていて src_list1 に含まれていない要素がある．
      ans &= 2;
      ++ i2;
    }
  }
  if ( i1 < n1 ) {
    // src_list1 に含まれていて src_list2 に含まれていない要素がある．
    ans &= 1;
  }
  if ( i2 < n2 ) {
    // src_list2 に含まれていて src_list1 に含まれていない要素がある．
    ans &= 2;
  }
  return ans;
}

// @brief 割当の内容を出力する．
ostream&
operator<<(ostream& s,
	   NodeVal nv)
{
  s << "Node#" << nv.node()->id()
    << "@" << nv.time()
    << " = " << nv.val();
  return s;
 }

// @brief 割当リストの内容を出力する．
ostream&
operator<<(ostream& s,
	   const NodeValList& src_list)
{
  int n = src_list.size();
  const char* comma = "";
  for (int i = 0; i < n; ++ i) {
    NodeVal nv = src_list[i];
    s << comma << nv;
    comma = ", ";
  }
  return s;
}

// @brief 大小関係の比較関数
bool
operator<(const NodeVal& left,
	  const NodeVal& right)
{
  const TpgNode* node1 = left.node();
  const TpgNode* node2 = right.node();
  if ( node1->id() < node2->id() ) {
    return true;
  }
  if ( node1->id() > node2->id() ) {
    return false;
  }
  if ( left.time() < right.time() ) {
    return true;
  }
  if ( left.time() > right.time() ) {
    return false;
  }
  return left.val() < right.val();
}

END_NAMESPACE_DRUID
