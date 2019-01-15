
/// @file Isx2.cc
/// @brief Isx2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.


#include "Isx2.h"
#include "MpColGraph.h"
#include "ym/UdGraph.h"
#include "ym/HashSet.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス Isx2
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] graph 対象のグラフ
Isx2::Isx2(MpColGraph& graph) :
  mGraph(graph),
  mCandMark(mGraph.node_num(), false),
  mAdjCount(mGraph.node_num(), 0),
  mCoverFlag(mGraph.fault_num(), false),
  mValue(mGraph.node_num(), 0)
{
  mCandList.reserve(mGraph.node_num());
}

// @brief デストラクタ
Isx2::~Isx2()
{
}

// @brief independent set extraction を用いた coloring を行う．
// @param[in] limit 残りのノード数がこの値を下回ったら処理をやめる．
// @return 彩色数を返す．
//
// ここでは部分的な彩色を行う．
int
Isx2::coloring(int limit)
{
  int remain_num = mGraph.fault_num();
  while ( remain_num > 0 ) {
    vector<int> indep_set;
    indep_set.reserve(mGraph.node_num());
    int max_value = 0;
    for ( auto count: Range(100) ) {
      vector<int> tmp_set;
      tmp_set.reserve(mGraph.node_num());
      get_indep_set(tmp_set);
      //int value = calc_value(tmp_set);
      int value = 0;
      if ( max_value < value ) {
	max_value = value;
	indep_set = tmp_set;
      }
    }

    // indep_set の各ノードに新しい色を割り当てる．
    mGraph.set_color(indep_set, mGraph.new_color());
    for ( auto node_id: indep_set ) {
      for ( auto row_id: mGraph.cover_list(node_id) ) {
	mGraph.set_covered(row_id);
      }
    }

    remain_num = 0;
    for ( auto row_id: Range(mGraph.fault_num()) ) {
      if ( !mGraph.is_covered(row_id) ) {
	++ remain_num;
      }
    }
  }

#if 0
  // 残りは DSATUR で彩色する．
  if ( remain_num > 0 ) {
    UdGraph graph(remain_num);
    vector<int> id_map(mGraph.node_num());
    vector<int> rid_map(remain_num);
    int new_id = 0;
    for ( auto id: Range(mGraph.node_num()) ) {
      if ( mGraph.color(id) == 0 ) {
	id_map[id] = new_id;
	rid_map[new_id] = id;
	++ new_id;
      }
    }
    ASSERT_COND( new_id == remain_num );
    for ( auto id: rid_map ) {
      auto new_id = id_map[id];
      for ( auto id1: mGraph.adj_list(id) ) {
	if ( mGraph.color(id1) == 0 ) {
	  auto new_id1 = id_map[id1];
	  if ( new_id1 > new_id ) {
	    graph.connect(new_id, new_id1);
	  }
	}
      }
    }
    vector<int> color_map1;
    int nc1 = graph.coloring("dsatur", color_map1);
    int c_base = mGraph.color_num();
    for ( auto i: Range(nc1) ) {
      mGraph.new_color();
    }
    for ( auto id: rid_map ) {
      auto new_id = id_map[id];
      int c = color_map1[new_id];
      mGraph.set_color(id, c + c_base);
    }
  }
#endif

  return mGraph.color_num();
}

// @brief maximal independent set を選ぶ．
//
// - 結果は indep_set に格納される．
// - mRandGen を用いてランダムに選ぶ．
void
Isx2::get_indep_set(vector<int>& indep_set)
{
  // 未彩色のノードを cand_list に入れる．
  init_cand_list();

  indep_set.clear();
  int node_id = select_node0();
  while ( node_id != -1 ) {
    indep_set.push_back(node_id);
    for ( auto row_id: mGraph.cover_list(node_id) ) {
      mCoverFlag[row_id] = true;
    }
    update_cand_list(node_id);
    node_id = select_node0();
  }
  //sort(indep_set.begin(), indep_set.end());
}

// @brief mCandList, mCandMark を初期化する．
void
Isx2::init_cand_list()
{
  for ( auto row_id: Range(mGraph.fault_num()) ) {
    mCoverFlag[row_id] = mGraph.is_covered(row_id);
  }
  mCandList.clear();
  for ( auto node_id: Range(mGraph.node_num()) ) {
    if ( mGraph.color(node_id) == 0 ) {
      int row_num = 0;
      for ( auto row_id: mGraph.cover_list(node_id) ) {
	if ( !mCoverFlag[row_id] ) {
	  ++ row_num;
	}
      }
      mValue[node_id] = row_num;
      if ( row_num > 0 ) {
	mCandList.push_back(node_id);
	mCandMark[node_id] = true;
	mAdjCount[node_id] = 0;
      }
    }
  }

  for ( auto node_id: mCandList ) {
    for ( auto node1_id: mGraph.adj_list(node_id) ) {
      ++ mAdjCount[node1_id];
    }
  }
}

// @brief 候補集合に加えるノードを選ぶ．
//
// - 現在の候補集合に隣接していないノードの内，隣接ノード数の少ないものを選ぶ．
// - 追加できるノードがない場合は -1 を返す．
int
Isx2::select_node()
{
  ASSERT_COND( mCandList.size() > 0 );

  vector<int> tmp_list;
  tmp_list.reserve(mCandList.size());
  int min_num = mGraph.node_num();
  int max_num = 0;
  for ( auto node_id: mCandList ) {
    int c = mAdjCount[node_id];
    if ( c > min_num ) {
      continue;
    }
    if ( min_num > c ) {
      min_num = c;
      tmp_list.clear();
      int row_num = 0;
      for ( auto row_id: mGraph.cover_list(node_id) ) {
	if ( !mGraph.is_covered(row_id) ) {
	  ++ row_num;
	}
      }
      max_num = row_num;
    }
    else { // min_num == c
      int row_num = 0;
      for ( auto row_id: mGraph.cover_list(node_id) ) {
	if ( !mGraph.is_covered(row_id) ) {
	  ++ row_num;
	}
      }
      if ( row_num < max_num ) {
	continue;
      }
      if ( max_num < row_num ) {
	max_num = row_num;
	tmp_list.clear();
      }
    }
    tmp_list.push_back(node_id);
  }

  return random_select(tmp_list);
}

// @brief 候補集合に加えるノードを選ぶ．
//
// - 現在の候補集合に隣接していないノードの内，隣接ノード数の少ないものを選ぶ．
// - 追加できるノードがない場合は -1 を返す．
int
Isx2::select_node0()
{
  ASSERT_COND( mCandList.size() > 0 );

  vector<int> tmp_list;
  tmp_list.reserve(mCandList.size());
  int max_num = 0;
  for ( auto node_id: mCandList ) {
    int v = mValue[node_id];
    if ( max_num > v ) {
      continue;
    }
    if ( max_num < v ) {
      max_num = v;
      tmp_list.clear();
    }
    tmp_list.push_back(node_id);
  }

  return random_select(tmp_list);
}

// @brief 候補リストを更新する．
// @param[in] node_id 新たに加わったノード
void
Isx2::update_cand_list(int node_id)
{
  // node_id と隣接するノードの cand_mark をはずす．
  mCandMark[node_id] = false;
  for ( auto node1_id: mGraph.adj_list(node_id) ) {
    if ( mCandMark[node1_id] ) {
      mCandMark[node1_id] = false;
      for ( auto node2_id: mGraph.adj_list(node1_id) ) {
	-- mAdjCount[node2_id];
      }
    }
  }

  // cand_mark に従って cand_list を更新する．
  int n = mCandList.size();
  int rpos = 0;
  int wpos = 0;
  for ( rpos = 0; rpos < n; ++ rpos ) {
    auto node1_id = mCandList[rpos];
    if ( mCandMark[node1_id] ) {
      int row_num = 0;
      for ( auto row_id: mGraph.cover_list(node1_id) ) {
	if ( !mCoverFlag[row_id] ) {
	  ++ row_num;
	}
      }
      mValue[node1_id] = row_num;
      if ( row_num > 0 ) {
	mCandList[wpos] = node1_id;
	++ wpos;
      }
    }
  }
  if ( wpos < n ) {
    mCandList.erase(mCandList.begin() + wpos, mCandList.end());
  }
}

END_NAMESPACE_DRUID
