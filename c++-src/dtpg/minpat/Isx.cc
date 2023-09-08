
/// @file Isx.cc
/// @brief Isx の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.


#include "Isx.h"
#include "Dsatur.h"
#include "MpColGraph.h"
#include "ym/UdGraph.h"
#include "ym/HashSet.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス Isx
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] graph 対象のグラフ
Isx::Isx(MpColGraph& graph) :
  mGraph(graph),
  mCandMark(mGraph.node_num(), false),
  mAdjCount(mGraph.node_num(), 0),
  mValue(mGraph.node_num(), 0)
{
  mCandList.reserve(mGraph.node_num());
}

// @brief デストラクタ
Isx::~Isx()
{
}

// @brief independent set extraction を用いた coloring を行う．
// @param[in] limit 残りのノード数がこの値を下回ったら処理をやめる．
// @return 彩色数を返す．
//
// ここでは部分的な彩色を行う．
int
Isx::coloring(int limit)
{
  int remain_col = mGraph.node_num();
  int remain_row = mGraph.fault_num();
  while ( remain_row > limit && remain_row > 0 ) {
    // 未彩色のノードを cand_list に入れる．
    init_cand_list();

    vector<int> indep_set;
    indep_set.reserve(remain_col);
    get_indep_set(indep_set);

    // indep_set の各ノードに新しい色を割り当てる．
    mGraph.set_color(indep_set, mGraph.new_color());

    remain_col -= indep_set.size();
    remain_row = 0;
    for ( auto row_id: Range(mGraph.fault_num()) ) {
      if ( !mGraph.is_covered(row_id) ) {
	++ remain_row;
      }
    }
  }

  // 残りは DSATUR で彩色する．
  if ( remain_row > 0 ) {
    Dsatur dsat(mGraph);
    dsat.coloring();
  }

  return mGraph.color_num();
}

// @brief maximal independent set を選ぶ．
//
// - 結果は indep_set に格納される．
// - mRandGen を用いてランダムに選ぶ．
void
Isx::get_indep_set(vector<int>& indep_set)
{
  vector<int> tmp_list(mCandList);

  indep_set.clear();
  for ( ; ; ) {
    int node_id = select_node0();
    if ( node_id == -1 ) {
      break;
    }
    indep_set.push_back(node_id);
    for ( auto row_id: mGraph.cover_list(node_id) ) {
      mGraph.set_covered(row_id);
    }
    update_cand_list(node_id);
  }
  //sort(indep_set.begin(), indep_set.end());
}

// @brief mCandList, mCandMark を初期化する．
void
Isx::init_cand_list()
{
  mCandList.clear();
  for ( auto node_id: Range(mGraph.node_num()) ) {
    if ( mGraph.color(node_id) == 0 ) {
      int row_num = 0;
      for ( auto row_id: mGraph.cover_list(node_id) ) {
	if ( !mGraph.is_covered(row_id) ) {
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

#if 0
  for ( auto node_id: mCandList ) {
    HashSet<int> node_set;
    for ( auto node1_id: mGraph.adj_list(node_id) ) {
      if ( !node_set.check(node1_id) ) {
	node_set.add(node1_id);
	++ mAdjCount[node1_id];
      }
    }
  }
#endif
}

// @brief 候補集合に加えるノードを選ぶ．
//
// - 現在の候補集合に隣接していないノードの内，隣接ノード数の少ないものを選ぶ．
// - 追加できるノードがない場合は -1 を返す．
int
Isx::select_node()
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
Isx::select_node0()
{
  vector<int> tmp_list;
  tmp_list.reserve(mCandList.size());
  int max_num = 0;
  for ( auto node_id: mCandList ) {
    int v = mValue[node_id];
    if ( max_num <= v ) {
      if ( max_num < v ) {
	max_num = v;
	tmp_list.clear();
      }
      tmp_list.push_back(node_id);
    }
  }

  return random_select(tmp_list);
}

// @brief 候補リストを更新する．
// @param[in] node_id 新たに加わったノード
void
Isx::update_cand_list(int node_id)
{
  // node_id と隣接するノードの cand_mark をはずす．
  mCandMark[node_id] = false;
  for ( auto node1_id: mGraph.adj_list(node_id) ) {
    if ( mCandMark[node1_id] ) {
      mCandMark[node1_id] = false;
#if 0
      HashSet<int> node_set;
      for ( auto node2_id: mGraph.adj_list(node1_id) ) {
	if ( !node_set.check(node2_id) ) {
	  node_set.add(node2_id);
	  -- mAdjCount[node2_id];
	}
      }
#endif
    }
  }

  // cand_mark に従って mCandList を更新する．
  int n = mCandList.size();
  int rpos = 0;
  int wpos = 0;
  for ( rpos = 0; rpos < n; ++ rpos ) {
    auto node1_id = mCandList[rpos];
    if ( mCandMark[node1_id] ) {
      int row_num = 0;
      for ( auto row_id: mGraph.cover_list(node1_id) ) {
	if ( !mGraph.is_covered(row_id) ) {
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
