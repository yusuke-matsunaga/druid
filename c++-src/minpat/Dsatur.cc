
/// @file Dsatur.cc
/// @brief Dsatur の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2013, 2015, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "Dsatur.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス Dsatur
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] graph 対象のグラフ
Dsatur::Dsatur(MpColGraph& graph) :
  mGraph(graph)
{
  init();
}

// @brief 初期化する．
void
Dsatur::init()
{
  int n = mGraph.node_num();
  mCandList.reserve(n);
  if ( n > 0 ) {
    mSatDegree = new int[n];
    mAdjDegree = new int[n];
    mCovDegree = new int[n];
    for ( auto node_id: Range(n) ) {
      int c = mGraph.color(node_id);
      if ( c != 0 ) {
	continue;
      }
      int row_num = 0;
      for ( auto row_id: mGraph.cover_list(node_id) ) {
	if ( !mGraph.is_covered(row_id) ) {
	  ++ row_num;
	}
      }
      if ( row_num == 0 ) {
	continue;
      }
      mCandList.push_back(node_id);
      vector<bool> used_color(mGraph.color_num() + 1, false);
      int adj_num = 0;
      HashSet<int> node_set;
      for ( auto node1_id: mGraph.adj_list(node_id) ) {
	if ( !node_set.check(node1_id) ) {
	  node_set.add(node1_id);
	  ++ adj_num;
	  int c1 = mGraph.color(node1_id);
	  used_color[c1] = true;
	}
      }
      int sat = 0;
      for ( auto c1: Range(1, mGraph.color_num()) ) {
	if ( used_color[c1] ) {
	  ++ sat;
	}
      }
      mSatDegree[node_id] = sat;
      mAdjDegree[node_id] = adj_num;
      mCovDegree[node_id] = row_num;
    }
  }
  else {
    mSatDegree = nullptr;
    mAdjDegree = nullptr;
    mCovDegree = nullptr;
  }
}

// @brief デストラクタ
Dsatur::~Dsatur()
{
  delete [] mSatDegree;
  delete [] mAdjDegree;
  delete [] mCovDegree;
}

// @brief 彩色する．
// @return 彩色数を返す．
void
Dsatur::coloring()
{
  // dsatur アルゴリズムを用いる．

  // 1: 隣接するノード数が最大のノードを選び彩色する．
  int max_node = get_max_node();
  update(max_node, mGraph.new_color());

  // 2: saturation degree が最大の未彩色ノードを選び最小の色番号で彩色する．
  for ( ; ; ) {
    cout << "Dsatur: mUncolNum = " << mCandList.size()
	 << ", color_num() = " << mGraph.color_num() << endl;
    int max_node = get_max_node();
    if ( max_node == -1 ) {
      break;
    }
    // max_node に隣接している未彩色の色のリストを作る．
    vector<int> free_list;
    free_list.reserve(mGraph.adj_list(max_node).num());
    vector<bool> used_color(mGraph.color_num() + 1, false);
    for ( auto node1_id: mGraph.adj_list(max_node) ) {
      int c = mGraph.color(node1_id);
      if ( c == 0 ) {
	free_list.push_back(node1_id);
      }
      else {
	used_color[c] = true;
      }
    }
    vector<int> color_list;
    color_list.reserve(mGraph.color_num());
    for ( auto c: Range(1, mGraph.color_num() + 1) ) {
      if ( !used_color[c] ) {
	color_list.push_back(c);
      }
    }
    int sel_col = 0;
    if ( color_list.empty() ) {
      // 可能な色がなかったので新しい色を割り当てる．
      sel_col = mGraph.new_color();
    }
    else {
      // color_list に含まれる色のなかで隣接するノードの sat_degree の増加が
      // 最小となるものを選ぶ．
      vector<int> color_count(mGraph.color_num() + 1, 0);
      for ( auto node1_id: free_list ) {
	vector<bool> used_color(mGraph.color_num() + 1, false);
	for ( auto node2_id: mGraph.adj_list(node1_id) ) {
	  int c = mGraph.color(node2_id);
	  used_color[c] = true;
	}
	for ( auto col: color_list ) {
	  if ( used_color[col] ) {
	    ++ color_count[col];
	  }
	}
      }
      int min_count = free_list.size() + 1;
      for ( auto col: color_list ) {
	int n = color_count[col];
	if ( min_count > n ) {
	  min_count = n;
	  sel_col = col;
	}
      }
    }
    update(max_node, sel_col);
  }

  // 検証
  // もちろん最小色数の保証はないが，同じ色が隣接していないことを確認する．
  // また，未彩色のノードがないことも確認する．
  // 違反が見つかったら例外を送出する．
  if ( true ) {
    ASSERT_COND( mGraph.is_covered() );
    ASSERT_COND( mGraph.verify() );
  }
  if ( !mGraph.is_covered() || !mGraph.verify() ) {
    cout << "Error" << endl;
  }
}

// @brief (sat_degree, adj_degree) の辞書順で最大のノードを取ってくる．
int
Dsatur::get_max_node()
{
  int max_row = -1;
  int max_sat = -1;
  int max_adj = -1;
  int max_node = -1;
  int rpos = 0;
  int wpos = 0;
  for ( ; rpos < mCandList.size(); ++ rpos ) {
    int node_id = mCandList[rpos];
    if ( mGraph.color(node_id) != 0 ) {
      continue;
    }

    int num_rows = 0;
    for ( auto row_id: mGraph.cover_list(node_id) ) {
      if ( !mGraph.is_covered(row_id) ) {
	++ num_rows;
      }
    }
    if ( num_rows > 0 ) {
      mCandList[wpos] = node_id;
      ++ wpos;

      int sat = mSatDegree[node_id];
      int adj = mAdjDegree[node_id];
      if ( max_row < num_rows ) {
	max_row = num_rows;
	max_sat = sat;
	max_adj = adj;
	max_node = node_id;
      }
      else if ( max_row == num_rows ) {
	if ( max_sat < sat ) {
	  max_sat = sat;
	  max_adj = adj;
	  max_node = node_id;
	}
	else if ( max_sat == sat ) {
	  if ( max_adj < adj ) {
	    max_adj = adj;
	    max_node = node_id;
	  }
	}
      }
    }
  }
  if ( wpos < rpos ) {
    mCandList.erase(mCandList.begin() + wpos, mCandList.end());
  }
  return max_node;
}

// @brief node_id に color の色を割り当て情報を更新する．
// @param[in] node_id ノード番号
// @param[in] color 色
void
Dsatur::update(int node_id,
	       int color)
{
  for ( auto node1_id: mGraph.adj_list(node_id) ) {
    bool already_colored = false;
    for ( auto node2_id: mGraph.adj_list(node1_id) ) {
      if ( mGraph.color(node2_id) == color ) {
	already_colored = true;
	break;
      }
    }
    if ( !already_colored ) {
      ++ mSatDegree[node1_id];
    }
  }
  mGraph.set_color(node_id, color);
  for ( auto row_id: mGraph.cover_list(node_id) ) {
    mGraph.set_covered(row_id);
  }
}

END_NAMESPACE_DRUID
