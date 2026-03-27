
/// @file ExtNaive.cc
/// @brief ExtNaive の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "ExtNaive.h"
#include "PropGraph.h"
#include "dtpg/SuffCond.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス ExtNaive
//////////////////////////////////////////////////////////////////////

// @brief 指定された出力からバックトレースを行う．
SuffCond
ExtNaive::backtrace(
  const PropGraph& data,
  const TpgNode& output
)
{
  std::vector<std::vector<TpgNode>> choice_list;
  std::vector<TpgNode> aux_side_inputs;
  auto node_list = data.backtrace(output, choice_list, aux_side_inputs);
  auto cnode_list = select_cnode(choice_list);
  node_list.insert(node_list.end(), cnode_list.begin(), cnode_list.end());
  // AssignList に変換する．
  AssignList assign_list;
  for ( auto& node: node_list ) {
    auto bval = (data.gval(node) == Val3::_1);
    assign_list.add(node, 1, bval);
  }
  for ( auto& node: aux_side_inputs ) {
    auto bval = (data.gval(node) == Val3::_1);
    assign_list.add(node, 1, bval);
  }
  return SuffCond(assign_list, AssignList());
}

// @brief 制御値を持つ side input を選ぶ．
std::vector<TpgNode>
ExtNaive::select_cnode(
  const std::vector<std::vector<TpgNode>>& choice_list
)
{
  // 出現回数を数える．
  std::unordered_map<SizeType, SizeType> node_count;
  for ( auto& cnode_list: choice_list ) {
    for ( auto& cnode: cnode_list ) {
      if ( node_count.count(cnode.id()) == 0 ) {
	node_count.emplace(cnode.id(), 1);
      }
      else {
	++ node_count.at(cnode.id());
      }
    }
  }

  // 出現回数の多い順に選択する．
  std::vector<TpgNode> ans_list;
  std::unordered_set<SizeType> selected;
  for ( auto& cnode_list: choice_list ) {
    SizeType max_count = 0;
    TpgNode max_node;
    bool done = false;
    for ( auto& cnode: cnode_list ) {
      if ( selected.count(cnode.id()) > 0 ) {
	done = true;
	break;
      }
      SizeType count = node_count.at(cnode.id());
      if ( max_count < count ) {
	max_count = count;
	max_node = cnode;
      }
    }
    if ( !done ) {
      ans_list.push_back(max_node);
      selected.emplace(max_node.id());
    }
  }

  return ans_list;
}

END_NAMESPACE_DRUID
