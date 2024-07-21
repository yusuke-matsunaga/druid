
/// @file Isx.cc
/// @brief Isx の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "Isx.h"
#include "Dsatur.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス Isx
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
Isx::Isx(
  ColGraph& graph,
  const JsonValue& option
) : mGraph(graph),
    mAdjCount(mGraph.node_num(), 0),
    mValue(mGraph.node_num(), 0)
{
  mCandList.reserve(mGraph.node_num());
  if ( option.is_object() ) {
    if ( option.has_key("skip") ) {
      mSkip = option.get("skip").get_bool();
    }
    if ( option.has_key("debug") ) {
      mDebug = option.get("debug").get_bool();
    }
  }
}

// @brief independent set extraction を用いた coloring を行う．
void
Isx::coloring(
  SizeType limit
)
{
  Timer timer;
  SizeType remain_num = mGraph.node_num();
  while ( remain_num > limit ) {
    timer.reset();
    timer.start();

    // 独立集合を求める．
    auto indep_set = get_indep_set();

    // indep_set の各ノードに新しい色を割り当てる．
    mGraph.set_color(indep_set, mGraph.new_color());

    remain_num -= indep_set.size();

    timer.stop();
    if ( mDebug ) {
      cout << "  " << indep_set.size() << " / " << remain_num << endl
	   << "  CPU time: " << timer.get_time() << endl;
    }
  }
}

// @brief maximal independent set を選ぶ．
vector<SizeType>
Isx::get_indep_set()
{
  // 結果の独立集合
  vector<SizeType> indep_set;
  // indep_set と独立なノードのリスト
  vector<SizeType> cand_list;
  cand_list.reserve(mGraph.node_num());
  for ( auto node_id: Range(mGraph.node_num()) ) {
    if ( mGraph.color(node_id) > 0 ) {
      // 彩色済み
      continue;
    }
    cand_list.push_back(node_id);
  }
  while ( !cand_list.empty() ) {
    // indep_set に加えた時に隣接数の増加が最小
    // になるノードを選ぶ．
    auto node_id = select_node(cand_list, indep_set);
    if ( node_id == std::numeric_limits<SizeType>::max() ) {
      break;
    }
    indep_set.push_back(node_id);

    // 情報を更新する．
    vector<bool> mark(mGraph.network().node_num(), false);
    for ( auto id: mGraph.conflict_list(node_id) ) {
      mark[id] = true;
    }
    vector<SizeType> new_list;
    for ( auto id1: cand_list ) {
      if ( id1 == node_id ) {
	continue;
      }
      if ( mark[id1] ) {
	continue;
      }
      if ( !mSkip && mGraph.is_conflict(id1, indep_set) ) {
	continue;
      }
      new_list.push_back(id1);
    }
    std::swap(cand_list, new_list);
  }
  return indep_set;
}

// @brief 候補集合に加えるノードを選ぶ．
SizeType
Isx::select_node(
  const vector<SizeType>& cand_list,
  const vector<SizeType>& indep_set
)
{
  // indep_set に加えた時に新たに衝突するノード数が
  // 最小となるノードを選ぶ．
  // indep_set と衝突しているノード
  vector<bool> mark(mGraph.network().node_num(), false);
  for ( auto id: indep_set ) {
    for ( auto id1: mGraph.conflict_list(id) ) {
      mark[id1] = true;
    }
  }
  SizeType min_num = mGraph.node_num() + 1;
  vector<pair<SizeType, SizeType>> tmp_list;
  tmp_list.reserve(cand_list.size());
  for ( auto id: cand_list ) {
    SizeType num = 0;
    for ( auto id1: mGraph.conflict_list(id) ) {
      if ( !mark[id1] ) {
	++ num;
      }
    }
    tmp_list.push_back({num, id});
  }
  sort(tmp_list.begin(), tmp_list.end(),
       [](const pair<SizeType, SizeType>& a,
	  const pair<SizeType, SizeType>& b) {
	 return a.first < b.first;
       });
  for ( auto& p: tmp_list ) {
    auto id = p.second;
    if ( !mSkip || !mGraph.is_conflict(id, indep_set) ) {
      return id;
    }
  }
  return std::numeric_limits<SizeType>::max();
}

// @brief ランダムに選択する．
SizeType
Isx::random_select(
  const vector<SizeType>& cand_list
)
{
  SizeType n = cand_list.size();
  std::uniform_int_distribution<SizeType> rd(0, n - 1);
  auto pos = rd(mRandGen);
  return cand_list[pos];
}

END_NAMESPACE_DRUID
