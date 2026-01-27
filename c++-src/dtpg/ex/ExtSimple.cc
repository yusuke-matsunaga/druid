
/// @file ExtSimple.cc
/// @brief ExtSimple の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "ExtSimple.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス ExtSimple
//////////////////////////////////////////////////////////////////////

// @brief 制御値を持つ side input を選ぶ．
std::vector<TpgNode>
ExtSimple::select_cnode(
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
