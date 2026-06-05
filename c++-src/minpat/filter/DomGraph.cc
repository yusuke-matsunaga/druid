
/// @file DomGraph.cc
/// @brief DomGraph の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "DomGraph.h"
#include "DichoCandMgr.h"
#include "DiGroup.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス DomGraph
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
DomGraph::DomGraph(
  const DichoCandMgr& candmgr
)
{
  auto ng = candmgr.group_num();
  mNodeList.reserve(ng);
  for ( SizeType id = 0; id < ng; ++ id ) {
    auto node = new DomNode(id);
    mNodeList.push_back(std::unique_ptr<DomNode>{node});
    mNodeMap.emplace(id, node);
  }

  // 支配関係を表すリンクを作る．
  for ( SizeType id = 0; id < ng; ++ id ) {
    auto group = candmgr.group(id);
    auto node = mNodeMap.at(group->id());
    for ( auto group1: group->dominance_list() ) {
      auto node1 = mNodeMap.at(group1->id());
      if ( node1 == node ) {
	continue;
      }
      node->mDownLink.push_back(node1);
      node1->mUpLink.push_back(node);
      ++ node1->mCount;
    }
  }

  // ランクを計算する．
  std::vector<DomNode*> node_list;
  for ( auto& node: mNodeList ) {
    if ( node->mCount == 0 ) {
      node_list.push_back(node.get());
    }
  }
  for ( SizeType rank = 0; !node_list.empty(); ++ rank ) {
    for ( auto node: node_list ) {
      node->mRank = rank;
      node->mHasRank = true;
    }
    std::vector<DomNode*> new_list;
    for ( auto node: node_list ) {
      for ( auto node1: node->mDownLink ) {
	-- node1->mCount;
	if ( node1->mCount == 0 ) {
	  new_list.push_back(node1);
	}
      }
    }
    std::swap(node_list, new_list);
  }
  SizeType total_num = 0;
  SizeType imm_num = 0;
  for ( auto& node: mNodeList ) {
    total_num += node->mDownLink.size();
    auto rank = node->rank();
    for ( auto node1: node->mDownLink ) {
      auto rank1 = node1->rank();
      if ( rank1 == rank + 1 ) {
	++ imm_num;
      }
    }
  }
  std::cout << "Total # of domcands:           " << total_num << std::endl
	    << "Total # of immediate comcands: " << imm_num << std::endl;
}

// @brief 故障のランクを返す．
SizeType
DomGraph::rank(
  SizeType id
) const
{
  auto node = mNodeMap.at(id);
  return node->rank();
}

// @brief 内容を出力する．
void
DomGraph::print(
  std::ostream& s
) const
{
  for ( auto& node: mNodeList ) {
    node->print(s);
    s << std::endl;
    s << "  --> ";
    for ( auto node1: node->mDownLink ) {
      s << " ";
      node1->print(s);
    }
    s << std::endl;
    s << "  <--";
    for ( auto node1: node->mUpLink ) {
      s << " ";
      node1->print(s);
    }
    s << std::endl
      << std::endl;
  }
}

END_NAMESPACE_DRUID
