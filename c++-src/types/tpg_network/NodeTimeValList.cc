
/// @file NodeTimeValList.cc
/// @brief NodeTimeValList の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "NodeTimeValList.h"
#include "TpgNode.h"


BEGIN_NAMESPACE_DRUID

// @brief マージする．
void
NodeTimeValList::merge(
  const NodeTimeValList& src_list
)
{
  vector<NodeTimeVal> tmp_list;
  SizeType n1 = mAsList.size();
  SizeType n2 = src_list.mAsList.size();
  tmp_list.reserve(n1 + n2);
  SizeType i1 = 0;
  SizeType i2 = 0;
  while ( i1 < n1 && i2 < n2 ) {
    auto v1 = mAsList[i1];
    auto v2 = src_list.mAsList[i2];
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
    auto v1 = mAsList[i1];
    tmp_list.push_back(v1);
  }
  for ( ; i2 < n2; ++ i2) {
    auto v2 = src_list.mAsList[i2];
    tmp_list.push_back(v2);
  }

  mAsList = tmp_list;
  mDirty = false;
}

// @brief 差分を計算する．
void
NodeTimeValList::diff(
  const NodeTimeValList& src_list
)
{
  vector<NodeTimeVal> tmp_list;
  SizeType n1 = mAsList.size();
  SizeType n2 = src_list.mAsList.size();
  tmp_list.reserve(n1);
  SizeType i1 = 0;
  SizeType i2 = 0;
  while ( i1 < n1 && i2 < n2 ) {
    auto v1 = mAsList[i1];
    auto v2 = src_list.mAsList[i2];
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
    auto v1 = mAsList[i1];
    tmp_list.push_back(v1);
  }

  mAsList = tmp_list;
  mDirty = false;
}

// @brief 矛盾した内容になっていないかチェックする．
bool
NodeTimeValList::sanity_check() const
{
  NodeTimeVal prev{nullptr, 0, false};
  for ( auto nv: mAsList ) {
    if ( prev.node_time() == nv.node_time() && prev.val() != nv.val() ) {
      return false;
    }
    prev = nv;
  }
  return true;
}

// @brief 2つの割当リストを比較する．
int
compare(
  const NodeTimeValList& src_list1,
  const NodeTimeValList& src_list2
)
{
  SizeType n1 = src_list1.size();
  SizeType n2 = src_list2.size();
  SizeType i1 = 0;
  SizeType i2 = 0;
  int ans = 3;
  while ( i1 < n1 && i2 < n2 ) {
    auto nv1 = src_list1[i1];
    auto nv2 = src_list2[i2];
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
operator<<(
  ostream& s,
  NodeTimeVal nv
)
{
  s << "Node#" << nv.node()->id()
    << "@" << nv.time()
    << " = " << nv.val();
  return s;
 }

// @brief 割当リストの内容を出力する．
ostream&
operator<<(
  ostream& s,
  const NodeTimeValList& src_list
)
{
  const char* comma = "";
  for ( auto nv: src_list ) {
    s << comma << nv;
    comma = ", ";
  }
  return s;
}

// @brief 大小関係の比較関数
bool
operator<(
  const NodeTimeVal& left,
  const NodeTimeVal& right
)
{
  auto node1 = left.node();
  auto node2 = right.node();
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
