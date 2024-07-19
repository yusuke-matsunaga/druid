
/// @file Dsatur2.cc
/// @brief Dsatur2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "Dsatur2.h"
#include "TpgNetwork.h"
#include "TpgFault.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス Dsatur2
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
Dsatur2::Dsatur2(
  ColGraph2& graph
) : mGraph{graph}
{
  SizeType n = mGraph.node_num();
  mCandList.reserve(n);
  for ( auto node_id: Range(n) ) {
    SizeType c = mGraph.color(node_id);
    if ( c != 0 ) {
      // 既に彩色済みの場合はスキップする．
      continue;
    }
    mCandList.push_back(node_id);
  }
  mFaultNum = mGraph.node_num();
}

// @brief デストラクタ
Dsatur2::~Dsatur2()
{
}

// @brief 彩色する．
void
Dsatur2::coloring()
{
  // dsatur アルゴリズムを用いる．

  // 1: 隣接するノード数が最大のノードを選び彩色する．
  SizeType max_node = get_max_node();
  update(max_node, mGraph.new_color());

  // 2: saturation degree が最大の未彩色ノードを選び最小の色番号で彩色する．
  while ( !mCandList.empty() ) {
    auto max_node = get_max_node();
    // max_node に隣接している未彩色の色のリストを作る．
    auto& col_list = mGraph.conflict_color_list(max_node);
    vector<bool> color_set(mGraph.color_num() + 1, false);
    vector<SizeType> free_col_list;
    free_col_list.reserve(mGraph.color_num());
    for ( auto col: col_list ) {
      color_set[col] = true;
    }
    for ( auto col: Range(1, mGraph.color_num() + 1) ) {
      if ( !color_set[col] ) {
	free_col_list.push_back(col);
      }
    }
    if ( free_col_list.empty() ) {
      // 可能な色がなかったので新しい色を割り当てる．
      auto col = mGraph.new_color();
      update(max_node, col);
    }
    else {
      // color_list に含まれる色のなかで隣接するノードの sat_degree の増加が
      // 最小となるものを選ぶ．
      vector<SizeType> color_count(mGraph.color_num() + 1, 0);
      auto& adj_list = mGraph.conflict_list(max_node);
      for ( auto node1_id: adj_list ) {
	if ( mGraph.color(node1_id) > 0 ) {
	  continue;
	}
	auto adj_list1 = mGraph.conflict_list(node1_id);
	vector<bool> color_set(mGraph.color_num() + 1, false);
	for ( auto node2_id: adj_list1 ) {
	  SizeType col = mGraph.color(node2_id);
	  if ( !color_set[col] ) {
	    color_set[col] = true;
	    ++ color_count[col];
	  }
	}
      }
      vector<pair<SizeType, SizeType>> color_count_list;
      color_count_list.reserve(free_col_list.size());
      for ( auto col: free_col_list ) {
	SizeType n = color_count[col];
	color_count_list.push_back({n, col});
      }
      sort(color_count_list.begin(), color_count_list.end(),
	   [](const pair<SizeType, SizeType>& a,
	      const pair<SizeType, SizeType>& b){
	     return a.first < b.first;
	   });
      auto col = color_count_list.front().second;
      update(max_node, col);
    }
  }
}

// @brief (sat_degree, adj_degree) の辞書順で最大のノードを取ってくる．
SizeType
Dsatur2::get_max_node()
{
  int max_sat = -1;
  int max_adj = -1;
  SizeType max_node;
  for ( auto node_id: mCandList ) {
    int sat = mGraph.saturation_degree(node_id);
    int adj = mGraph.adjacent_degree(node_id);
    if ( max_sat <= sat ) {
      if ( max_sat < sat ) {
	max_sat = sat;
	max_adj = adj;
	max_node = node_id;
      }
      else if ( max_adj < adj ) {
	max_adj = adj;
	max_node = node_id;
      }
    }
  }
  ASSERT_COND( max_sat >= 0 );
  cout << mFaultNum << ": Choose Node#" << max_node << " (" << max_sat << ", " << max_adj << ")"
       << " / " << mGraph.color_num()
       << endl;
  return max_node;
}

// @brief node_id に color の色を割り当て情報を更新する．
void
Dsatur2::update(
  SizeType node_id,
  SizeType color
)
{
  mGraph.set_color(node_id, color);
  -- mFaultNum;

  SizeType rpos = 0;
  SizeType wpos = 0;
  for ( ; rpos < mCandList.size(); ++ rpos ) {
    SizeType node_id = mCandList[rpos];
    if ( mGraph.color(node_id) == 0 ) {
      mCandList[wpos] = node_id;
      ++ wpos;
    }
  }
  if ( wpos < rpos ) {
    mCandList.erase(mCandList.begin() + wpos, mCandList.end());
  }
}

END_NAMESPACE_DRUID
