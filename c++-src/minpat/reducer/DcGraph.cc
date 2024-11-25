
/// @file DcGraph.cc
/// @brief DcGraph の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "DcGraph.h"
#include "TpgFault.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

SizeType
get_max_id(
  const vector<const TpgFault*>& fault_list
)
{
  SizeType max_id = 0;
  for ( auto f: fault_list ) {
    max_id = std::max(max_id, f->id());
  }
  ++ max_id;
  return max_id;
}

END_NONAMESPACE

// @brief コンストラクタ
DcGraph::DcGraph(
  const vector<const TpgFault*>& fault_list,
  const vector<vector<const TpgFault*>>& dom_cand_list
) : mNodeArray(get_max_id(fault_list), nullptr)
{
  for ( auto f: fault_list ) {
    auto node = new DcNode{f};
    mNodeArray[f->id()] = node;
  }
  for ( auto f1: fault_list ) {
    auto node1 = mNodeArray[f1->id()];
    for ( auto f2: dom_cand_list[f1->id()] ) {
      auto node2 = mNodeArray[f2->id()];
      auto edge = new DcEdge{node1, node2};
      mEdgeList.push_back(edge);
      node1->mOutEdgeList.push_back(edge);
      node2->mInEdgeList.push_back(edge);
    }
  }
}

// @brief デストラクタ
DcGraph::~DcGraph()
{
  for ( auto node: mNodeArray ) {
    delete node;
  }
  for ( auto edge: mEdgeList ) {
    delete edge;
  }
}

// @brief 故障に対応したノードを返す．
DcNode*
DcGraph::node(
  const TpgFault* f
) const
{
  return mNodeArray[f->id()];
}

// @brief トポロジカル順にソートする．
vector<DcNode*>
DcGraph::sorted() const
{
  // 結果を格納するリスト
  vector<DcNode*> ans_list;
  // 作業用のキュー
  std::deque<DcNode*> queue;
  // 処理済みの印
  unordered_set<SizeType> mark;
  // 入枝のない節点を探す．
  for ( auto node: mNodeArray ) {
    if ( node != nullptr && node->inedge_list().empty() ) {
      queue.push_back(node);
    }
  }
  while ( !queue.empty() ) {
    auto node = queue.front();
    queue.pop_front();
    ans_list.push_back(node);
    mark.emplace(node->fault()->id());
    // node の出枝の先のノードで処理可能なノードを探す．
    for ( auto e: node->outedge_list() ) {
      auto node2 = e->to_node();
      bool found = true;
      for ( auto e2: node2->inedge_list() ) {
	auto node1 = e2->from_node();
	if ( mark.count(node1->fault()->id()) == 0 ) {
	  found = false;
	  break;
	}
      }
      if ( found ) {
	queue.push_back(node2);
      }
    }
  }
  return ans_list;
}

END_NAMESPACE_DRUID
