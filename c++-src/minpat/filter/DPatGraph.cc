
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
) : mPatList{pat_list}
{
  auto npat = pat_list.size();
  mNodeList.reserve(npat);
  for ( SizeType id = 0; id < npat; ++ id ) {
    auto pat = pat_list[id];
    auto node = new DPatNode(id, pat);
    mNodeList.push_back(std::unique_ptr<DPatNode>{node});
    mNodeMap.emplace(pat, node);
  }

  // 支配関係を表すリンクを作る．
  for ( SizeType i1 = 0; i1 < npat - 1; ++ i1 ) {
    auto pat1 = pat_list[i1];
    auto node1 = mNodeMap.at(pat1);
    for ( SizeType i2 = i1 + 1; i2 < npat; ++ i2 ) {
      auto pat2 = pat_list[i2];
      auto node2 = mNodeMap.at(pat2);
      auto cap = pat1 & pat2;
      if ( cap == pat1 ) {
	node1->mDownLink.push_back(node2);
	node2->mUpLink.push_back(node1);
	++ node2->mCount;
      }
      else if ( cap == pat2 ) {
	node2->mDownLink.push_back(node1);
	node1->mUpLink.push_back(node2);
	++ node1->mCount;
      }
    }
  }

  // ランクを計算する．
  for ( auto& node: mNodeList ) {
    if ( node->mCount == 0 ) {
      mRank0List.push_back(node.get());
    }
  }
  auto node_list = mRank0List;
  for ( SizeType rank = 0; !node_list.empty(); ++ rank ) {
    for ( auto node: node_list ) {
      node->mRank = rank;
      node->mHasRank = true;
    }
    std::vector<DPatNode*> new_list;
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
#if 0
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
#endif
}

// @brief 直接の支配関係にあるパタンのリストを返す．
std::vector<PackedVal>
DPatGraph::dom_list(
  PackedVal pat
) const
{
  if ( mNodeMap.count(pat) > 0 ) {
    auto node = mNodeMap.at(pat);
    return node->dom_list();
  }

  std::vector<PackedVal> ans_list;
  std::unordered_set<PackedVal> mark;
  for ( auto node: mRank0List ) {
    auto pat1 = node->pat();
    if ( (pat1 & pat) == pat ) {
      ans_list.push_back(pat1);
    }
    else {
      dfs(node, pat, mark, ans_list);
    }
  }
  return ans_list;
}

/// @brief dom_list() 用の下請け関数
void
DPatGraph::dfs(
  const DPatNode* node,
  PackedVal pat,
  std::unordered_set<PackedVal>& mark,
  std::vector<PackedVal>& ans_list
) const
{
  auto pat1 = node->pat();
  if ( mark.count(pat1) > 0 ) {
    return;
  }
  mark.insert(pat1);
  if ( (pat1 & pat) == pat ) {
    ans_list.push_back(pat1);
    return;
  }
  if ( (pat1 & pat) == pat1 ) {
    for ( auto node1: node->mDownLink ) {
      dfs(node1, pat, mark, ans_list);
    }
  }
}

// @brief 内容を出力する．
void
DPatGraph::print(
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


//////////////////////////////////////////////////////////////////////
// クラス DPatNode
//////////////////////////////////////////////////////////////////////

// @brief 直接支配するパタンのリストを返す．
std::vector<PackedVal>
DPatNode::dom_list() const
{
  std::vector<PackedVal> pat_list;
  pat_list.reserve(mDownLink.size());
  for ( auto node: mDownLink ) {
    if ( node->mRank == mRank + 1 ) {
      // ランクが 1 違いのノードが直接支配しているノード
      pat_list.push_back(node->pat());
    }
  }
  return pat_list;
}

// @grief 内容を出力する．
void
DPatNode::print(
  std::ostream& s
) const
{
  s << "Node#" << id();
  if ( mHasRank ) {
    s << "@" << mRank;
  }
  else {
    s << "[" << mCount << "]";
  }
}

END_NAMESPACE_DRUID
