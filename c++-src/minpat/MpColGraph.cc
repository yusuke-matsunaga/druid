
/// @file MpColGraph.cc
/// @brief MpColGraph の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "MpColGraph.h"
#include "TestVector.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
MpColGraph::MpColGraph(
  const vector<TestVector>& tv_list
) : mTvList{tv_list},
    mNodeNum{mTvList.size()},
    mVectorSize{0},
    mOidListArray(mNodeNum),
    mColNum{0},
    mColorMap(mNodeNum, 0),
    mTmpMark(mNodeNum, 0)
{
  if ( mNodeNum > 0 ) {
    TestVector tv0 = mTvList[0];
    mVectorSize = tv0.vector_size();
    mNodeListArray.resize(mVectorSize * 2);

    gen_conflict_list();

    mTmpList.reserve(mNodeNum);
  }
}

// @brief デストラクタ
MpColGraph::~MpColGraph()
{
}

// @brief 衝突リストを作る．
void
MpColGraph::gen_conflict_list()
{
  for ( auto bit: Range(mVectorSize) ) {
    SizeType oid0 = bit * 2 + 0;
    SizeType oid1 = bit * 2 + 1;
    vector<SizeType>& list0 = mNodeListArray[oid0];
    vector<SizeType>& list1 = mNodeListArray[oid1];
    for ( auto id: Range(mNodeNum) ) {
      const TestVector& tv = mTvList[id];
      Val3 val = tv.val(bit);
      if ( val == Val3::_0 ) {
	list0.push_back(id);
      }
      else if ( val == Val3::_1 ) {
	list1.push_back(id);
      }
    }
    if ( !list0.empty() && !list1.empty() ) {
      for ( auto id: list0 ) {
	mOidListArray[id].push_back(oid1);
      }
      for ( auto id: list1 ) {
	mOidListArray[id].push_back(oid0);
      }
    }
  }

  for ( auto id: Range(mNodeNum) ) {
    auto& list = mOidListArray[id];
    sort(list.begin(), list.end());
  }
}

// @brief ノードを削除する．
void
MpColGraph::delete_node(
  SizeType node
)
{
  ASSERT_COND( node >= 0 && node < node_num() );

  for ( auto oid: mOidListArray[node] ) {
    vector<SizeType>& list = mNodeListArray[oid ^ 1];
    // list から node を削除する．
    SizeType rpos = 0;
    SizeType n = list.size();
    for ( ; rpos < n; ++ rpos ) {
      if ( list[rpos] == node ) {
	break;
      }
    }
    ASSERT_COND( rpos < n );

    SizeType wpos = rpos;
    for ( ++ rpos; rpos < n; ++ rpos, ++ wpos ) {
      list[wpos] = list[rpos];
    }
    list.erase(list.end() - 1, list.end());
  }
}

// @brief node が node_list のノード集合と両立する時 true を返す．
bool
MpColGraph::compatible_check(
  SizeType node,
  const vector<SizeType>& node_list
) const
{
  vector<bool> mark(mVectorSize * 2, false);
  for ( auto node1: node_list ) {
    for ( auto oid1: mOidListArray[node1] ) {
      mark[oid1] = true;
    }
  }

  for ( auto oid: mOidListArray[node] ) {
    if ( mark[oid ^ 1] ) {
      return false;
    }
  }
  return true;
}

// @brief node1 の衝突集合が node2 の衝突集合に含まれていたら true を返す．
bool
MpColGraph::containment_check(
  SizeType node1,
  SizeType node2
) const
{
  // まず mOidListArray[node1] と mOidListArray[node2] を比較する．
  // 共通に含まれる oid は削除する．
  const vector<SizeType>& src_list1 = mOidListArray[node1];
  const vector<SizeType>& src_list2 = mOidListArray[node2];
  vector<SizeType> tmp_list1; tmp_list1.reserve(src_list1.size());
  vector<SizeType> tmp_list2; tmp_list2.reserve(src_list2.size());
  SizeType rpos1 = 0;
  SizeType rpos2 = 0;
  SizeType n1 = src_list1.size();
  SizeType n2 = src_list2.size();
  while ( rpos1 < n1 && rpos2 < n2 ) {
    SizeType oid1 = src_list1[rpos1];
    SizeType oid2 = src_list2[rpos2];
    if ( oid1 < oid2 ) {
      tmp_list1.push_back(oid1);
      ++ rpos1;
    }
    else if ( oid1 > oid2 ) {
      tmp_list2.push_back(oid2);
      ++ rpos2;
    }
    else {
      ++ rpos1;
      ++ rpos2;
    }
  }
  for ( ; rpos1 < n1; ++ rpos1 ) {
    SizeType oid1 = src_list1[rpos1];
    tmp_list1.push_back(oid1);
  }
  for ( ; rpos2 < n2; ++ rpos2 ) {
    SizeType oid2 = src_list2[rpos2];
    tmp_list2.push_back(oid2);
  }
  // tmp_list1 に含まれる oid の要素が tmp_list2 に含まれているか調べる．
  for ( auto oid1: tmp_list1 ) {
    for ( auto id1: mNodeListArray[oid1] ) {
      bool found = false;
      for ( auto oid2: tmp_list2 ) {
	for ( auto id2: mNodeListArray[oid2] ) {
	  if ( id2 == id1 ) {
	    found = true;
	    break;
	  }
	  if ( id2 > id1 ) {
	    break;
	  }
	}
	if ( found ) {
	  break;
	}
      }
      if ( !found ) {
	// node1 の衝突集合に含まれていて node2 の衝突集合に含まれない
	// ノードがある．
	return false;
      }
    }
  }

  return true;
}

// @brief ノードの衝突数を返す．
SizeType
MpColGraph::conflict_num(
  SizeType node
) const
{
  get_conflict_list(node, mTmpList);
  SizeType n = mTmpList.size();
  mTmpList.clear();

  return n;
}

// @brief ノードの衝突リストを返す．
void
MpColGraph::get_conflict_list(
  const vector<SizeType>& node_list,
  vector<SizeType>& conflict_list
) const
{
  conflict_list.clear();
  for ( auto node: node_list ) {
    for ( auto oid: mOidListArray[node] ) {
      for ( auto id: mNodeListArray[oid] ) {
	if ( mTmpMark[id] == 0 ) {
	  mTmpMark[id] = 1;
	  conflict_list.push_back(id);
	}
      }
    }
  }
  for ( auto id: conflict_list ) {
    mTmpMark[id] = 0;
  }
}

// @brief color_map を作る．
SizeType
MpColGraph::get_color_map(
  vector<SizeType>& color_map
) const
{
  color_map.clear();
  color_map.resize(node_num());
  for ( auto node: Range(node_num()) ) {
    color_map[node] = mColorMap[node];
  }
  return color_num();
}

END_NAMESPACE_DRUID
