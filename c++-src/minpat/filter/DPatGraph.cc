
/// @file DPatGraph.cc
/// @brief DPatGraph の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "DPatGraph.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス DPatGraph
//////////////////////////////////////////////////////////////////////

// @brief 空のコンストラクタ
DPatGraph::DPatGraph()
{
}

// @brief コンストラクタ
DPatGraph::DPatGraph(
  const std::vector<DPat>& pat_list
)
{
  _set(pat_list);
}

// @brief デストラクタ
DPatGraph::~DPatGraph()
{
}

// @brief 内容をセットする．
void
DPatGraph::rebuild(
  const std::vector<DPat>& pat_list
)
{
  mPatList.clear();
  mIdMap.clear();
  mLayeredList.clear();
  _set(pat_list);
}

// @brief 内容をセットする．
void
DPatGraph::_set(
  const std::vector<DPat>& pat_list
)
{
  { // pat_list が重複していることも考慮してユニークな番号を割り当てる．
    for ( auto pat: pat_list ) {
      if ( mIdMap.count(pat) == 0 ) {
	auto id = mPatList.size();
	mPatList.push_back(pat);
	mIdMap.emplace(pat, id);
      }
    }
  }
  // mPatList を作りランクを計算する．
  auto npat = mPatList.size();
  SizeType max_rank = 0;
  std::vector<SizeType> rank_array(npat);
  for ( SizeType id = 0; id < npat; ++ id ) {
    auto& pat = mPatList[id];
    auto rank = pat.count_ones();
    rank_array[id] = rank;
    max_rank = std::max(max_rank, rank);
  }
  // ランクごとのリストを作る．
  ++ max_rank;
  mLayeredList.resize(max_rank);
  for ( SizeType i = 0; i < npat; ++ i ) {
    auto& pat = mPatList[i];
    auto rank = rank_array[i];
    mLayeredList[rank].push_back(pat);
  }
}

// @brief 直接の後続パタンのリストを返す．
std::vector<DPat>
DPatGraph::imm_succ_list(
  const DPat& pat,
  const std::vector<DPat>& block_pats
) const
{
  auto rank0 = pat.count_ones();
  std::vector<DPat> pat_list;
  std::vector<DPat> tmp_list = block_pats;
  auto rank_max = mLayeredList.size();
  for ( auto rank = rank0 + 1; rank < rank_max; ++ rank ) {
    for ( auto& pat1: mLayeredList[rank] ) {
      if ( pat.check_contained(pat1) ) {
	bool blocked = false;
	for ( auto& pat2: tmp_list ) {
	  if ( pat2.check_contained(pat1) ) {
	    // pat1 は pat2 の後続
	    blocked = true;
	    break;
	  }
	}
	if ( !blocked ) {
	  pat_list.push_back(pat1);
	  tmp_list.push_back(pat1);
	}
      }
    }
  }
  return pat_list;
}

// @brief 内容を出力する．
void
DPatGraph::print(
  std::ostream& s
) const
{
  auto rank_size = mLayeredList.size();
  for ( SizeType rank = 0; rank < rank_size; ++ rank ) {
    s << "Rank#" << rank << ":" << std::endl;
    auto& pat_list = mLayeredList[rank];
    for ( auto& pat: pat_list ) {
      s << pat.to_str() << std::endl;
      for ( auto pat1: imm_succ_list(pat) ) {
	s << "  --> "
	  << pat1.to_str() << std::endl;
      }
    }
    s << std::endl;
  }
}

END_NAMESPACE_DRUID
