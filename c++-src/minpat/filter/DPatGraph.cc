
/// @file DPatGraph.cc
/// @brief DPatGraph の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "DPatGraph.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE


END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// クラス DPatGraph
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
DPatGraph::DPatGraph(
  const std::vector<PackedVal>& pat_list
) : mPatList{pat_list}
{
  auto npat = pat_list.size();
  for ( SizeType id = 0; id < npat; ++ id ) {
    auto pat = pat_list[id];
    mIdMap.emplace(pat, id);
  }

  POSet::Builder builder;
  builder.mElemList.reserve(npat);
  for ( SizeType id1 = 0; id1 < npat - 1; ++ id1 ) {
    auto pat1 = pat_list[id1];
    for ( SizeType id2 = id1 + 1; id2 < npat; ++ id2 ) {
      auto pat2 = pat_list[id2];
      auto pat3 = pat1 & pat2;
      if ( pat3 == pat1 ) {
	builder.add(id1, id2);
      }
      else if ( pat3 == pat2 ) {
	builder.add(id2, id1);
      }
    }
  }

  mPOSet.rebuild(builder);
}

// @brief 直接の支配関係にあるパタンのリストを返す．
std::vector<PackedVal>
DPatGraph::dom_list(
  PackedVal root_pat,
  const std::vector<PackedVal>& target_pats,
  const std::vector<PackedVal>& block_pats
) const
{
  // マーク
  // 0: なし
  // 1: target
  // 2: block
  std::vector<int> mark(mPatList.size(), 0);
  for ( auto pat: block_pats) {
    auto id = mIdMap.at(pat);
    mark[id] = 2;
  }
  // target は block より優先する．
  for ( auto pat: target_pats ) {
    auto id = mIdMap.at(pat);
    mark[id] = 1;
  }
  auto id = mIdMap.at(root_pat);
  if ( mark[id] == 1 ) {
    // root_pat が target だった場合はそれだけ
    return {root_pat};
  }
  std::vector<PackedVal> pat_list;
  dfs(id, mark, pat_list);
  return pat_list;
}

/// @brief dom_list() 用の下請け関数
void
DPatGraph::dfs(
  SizeType id,
  std::vector<int>& mark,
  std::vector<PackedVal>& ans_list
) const
{
  if ( mark[id] > 0 ) {
    if ( mark[id] == 1 ) {
      auto pat = mPatList[id];
      ans_list.push_back(pat);
    }
    return;
  }
  mark[id] = 2;
  for ( auto id1: mPOSet.imm_succ_list(id) ) {
    dfs(id1, mark, ans_list);
  }
}

// @brief 内容を出力する．
void
DPatGraph::print(
  std::ostream& s
) const
{
  auto rank_size = mPOSet.rank_size();
  for ( SizeType rank = 0; rank < rank_size; ++ rank ) {
    s << "Rank#" << rank << ":" << std::endl;
    auto& id_list = mPOSet.rank_list(rank);
    for ( auto id: id_list ) {
      auto pat = mPatList[id];
      s << id << "[" << std::hex << std::setw(16) << pat << std::dec << "]";
      s << "  --> ";
      for ( auto id1: mPOSet.imm_succ_list(id) ) {
	s << " " << id1;
      }
      s << std::endl;
    }
    s << std::endl;
  }
}

END_NAMESPACE_DRUID
