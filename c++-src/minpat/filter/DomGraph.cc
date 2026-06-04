
/// @file DomGraph.cc
/// @brief DomGraph の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "DomGraph.h"
#include "EqDomCand.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス DomGraph
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
DomGraph::DomGraph(
  const EqDomCand& cand
)
{
  auto fault_list = cand.fault_list();
  auto nf = fault_list.size();
  mNodeList.reserve(nf);
  for ( auto fault: fault_list ) {
    auto node = new DomNode(fault);
    mNodeList.push_back(std::unique_ptr<DomNode>{node});
    mNodeMap.emplace(fault.id(), node);
  }

  // 支配関係を表すリンクを作る．
  for ( auto fault: fault_list ) {
    auto node = mNodeMap.at(fault.id());
    for ( auto fault1: cand.domcand(fault) ) {
      auto node1 = mNodeMap.at(fault1.id());
      node->mDownLink.push_back(node1);
      node1->mUpLink.push_back(node);
    }
  }

  print(std::cout);

  // ランクを計算する．
  std::unordered_set<SizeType> mark;
  std::vector<DomNode*> node_list;
  for ( auto& node: mNodeList ) {
    if ( node->mUpLink.empty() ) {
      node_list.push_back(node.get());
    }
  }
  for ( auto node: node_list ) {
    node->mRank = 0;
    mark.insert(node->id());
  }
  while ( !node_list.empty() ) {
    {
      std::cout << "node_list: ";
      for ( auto node: node_list ) {
	std::cout << " " << node->fault().str();
      }
      std::cout << std::endl;
    }
    std::vector<DomNode*> new_list;
    for ( auto node: node_list ) {
      for ( auto node1: node->mDownLink ) {
	++ node1->mCount;
	if ( node1->mCount == node1->mUpLink.size() ) {
	  SizeType rank = 0;
	  for ( auto node2: node1->mUpLink ) {
	    rank = std::max(rank, node2->rank());
	  }
	  node1->mRank = rank + 1;
	  new_list.push_back(node1);
	  mark.insert(node1->id());
	}
      }
    }
    std::swap(node_list, new_list);
  }
  for ( auto& node: mNodeList ) {
    if ( mark.count(node->id()) == 0 ) {
      abort();
    }
  }
}

// @brief 故障のランクを返す．
SizeType
DomGraph::rank(
  const TpgFault& fault
) const
{
  auto node = mNodeMap.at(fault.id());
  return node->rank();
}

// @brief 内容を出力する．
void
DomGraph::print(
  std::ostream& s
) const
{
  for ( auto& node: mNodeList ) {
    auto f = node->fault();
    s << f.str() << "[" << node->rank() << "]:" << std::endl;
    s << "  --> ";
    for ( auto node1: node->mDownLink ) {
      auto f1 = node1->fault();
      s << " " << f1.str() << "[" << node1->rank() << "]";
    }
    s << std::endl;
    s << "  <--";
    for ( auto node1: node->mUpLink ) {
      auto f1 = node1->fault();
      s << " " << f1.str() << "[" << node1->rank() << "]";
    }
    s << std::endl;
  }
}

END_NAMESPACE_DRUID
