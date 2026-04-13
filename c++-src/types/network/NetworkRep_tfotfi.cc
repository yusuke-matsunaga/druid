
/// @File NetworkRep_tfitfo.cc
/// @brief NetworkRep::get_tfo_list(), get_tfi_list() の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "NetworkRep.h"
#include "NodeRep.h"
#include "NodeQueue.h"
#include "DfsDriver.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス NetworkRep
//////////////////////////////////////////////////////////////////////

// @brief TFO のノードを求める．
std::vector<const NodeRep*>
NetworkRep::get_tfo_list(
  const std::vector<const NodeRep*>& root_list,
  const NodeRep* block,
  std::function<bool(const NodeRep*)> op
) const
{
  std::vector<const NodeRep*> node_list;
  NodeQueue queue(node_num());
  for ( auto root: root_list ) {
    queue.put(root);
  }
  while ( !queue.empty() ) {
    auto node = queue.get();
    node_list.push_back(node);
    if ( op(node) ) {
      if ( node != block ) {
	for ( auto onode: node->fanout_list() ) {
	  queue.put(onode);
	}
      }
    }
  }
  return node_list;
}

// @brief TFI のノードを求める．
std::vector<const NodeRep*>
NetworkRep::get_tfi_list(
  const std::vector<const NodeRep*>& root_list,
  std::function<bool(const NodeRep*)> op
) const
{
  std::vector<const NodeRep*> node_list;
  NodeQueue queue(node_num());
  for ( auto root: root_list ) {
    queue.put(root);
  }
  while ( !queue.empty() ) {
    auto node = queue.get();
    node_list.push_back(node);
    if ( op(node) ) {
      for ( auto inode: node->fanin_list() ) {
	queue.put(inode);
      }
    }
  }
  return node_list;
}

// @brief 出力からの DFS を行う．
void
NetworkRep::dfs(
  const std::vector<const NodeRep*>& root_list,
  std::function<void(const NodeRep*)> pre_func,
  std::function<void(const NodeRep*)> post_func
)
{
  DfsDriver dfs(node_num(), pre_func, post_func);
  for ( auto root: root_list ) {
    dfs(root);
  }
}

END_NAMESPACE_DRUID
