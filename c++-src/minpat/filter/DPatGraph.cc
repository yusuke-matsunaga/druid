
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

// @brief コンストラクタ
DPatGraph::DPatGraph(
  const std::vector<PackedVal>& pat_list
)
{
  SizeType id = 0;
  for ( auto pat: pat_list ) {
    if ( mIdMap.count(pat) == 0 ) {
      mIdMap.emplace(pat, id);
      ++ id;
    }
  }
  mPatList.resize(id);
  for ( auto pat: pat_list ) {
    auto id = mIdMap.at(pat);
    mPatList[id] = pat;
  }

  POSet::Builder builder;
  auto npat = mPatList.size();
  builder.mElemList.reserve(npat);
  for ( SizeType id1 = 0; id1 < npat - 1; ++ id1 ) {
    auto pat1 = mPatList[id1];
    for ( SizeType id2 = id1 + 1; id2 < npat; ++ id2 ) {
      auto pat2 = mPatList[id2];
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

// @brief 直接の後続パタンのリストを返す．
std::vector<PackedVal>
DPatGraph::imm_succ_list(
  PackedVal pat
) const
{
  auto id = mIdMap.at(pat);
  auto tmp_list = mPOSet.imm_succ_list(id);
  std::vector<PackedVal> pat_list;
  pat_list.reserve(tmp_list.size());
  for ( auto id1: tmp_list ) {
    auto pat = mPatList[id1];
    pat_list.push_back(pat);
  }
  return pat_list;
}

// @brief ブロックされたノードまでたどる．
void
DPatGraph::traverse(
  PackedVal start_pat,
  const std::vector<PackedVal>& block_pats,
  std::vector<PackedVal>& medial_pats,
  std::vector<PackedVal>& boundary_pats
) const
{
  auto start_id = mIdMap.at(start_pat);
  std::vector<SizeType> block_list;
  block_list.reserve(block_pats.size());
  for ( auto pat: block_pats ) {
    auto id = mIdMap.at(pat);
    block_list.push_back(id);
  }
  std::vector<SizeType> medial_list;
  std::vector<SizeType> boundary_list;
  mPOSet.traverse(start_id, block_list, medial_list, boundary_list);
  medial_pats.reserve(medial_list.size());
  for ( auto id: medial_list ) {
    auto pat = mPatList[id];
    medial_pats.push_back(pat);
  }
  boundary_pats.reserve(boundary_list.size());
  for ( auto id: boundary_list ) {
    auto pat = mPatList[id];
    boundary_pats.push_back(pat);
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
