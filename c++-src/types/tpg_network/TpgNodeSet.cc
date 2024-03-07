
/// @file TpgNodeSet.cc
/// @brief TpgNodeSet の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "TpgNodeSet.h"


BEGIN_NAMESPACE_DRUID

// @brief TFO のノードを求める．
vector<const TpgNode*>
TpgNodeSet::get_tfo_list(
  SizeType max_size,
  const TpgNode* root,
  const TpgNode* bnode
)
{
  vector<const TpgNode*> node_list;
  node_list.reserve(max_size);
  vector<bool> mark_array(max_size, false);

  set_mark(root, node_list, mark_array);
  for ( SizeType rpos = 0; rpos < node_list.size(); ++ rpos ) {
    // set_tfo_mark() 中で node_list に要素を追加しているので
    // 古いタイプの for 文を用いている．
    auto node = node_list[rpos];
    if ( node != bnode ) {
      for ( auto onode: node->fanout_list() ) {
	set_mark(onode, node_list, mark_array);
      }
    }
  }
  return node_list;
}

// @brief TFO のノードを求める．
vector<const TpgNode*>
TpgNodeSet::get_tfo_list(
  SizeType max_size,
  const TpgNode* root,
  std::function<void(const TpgNode*)> op
)
{
  vector<const TpgNode*> node_list;
  node_list.reserve(max_size);
  vector<bool> mark_array(max_size, false);

  set_mark(root, node_list, mark_array);
  for ( SizeType rpos = 0; rpos < node_list.size(); ++ rpos ) {
    // set_tfo_mark() 中で node_list に要素を追加しているので
    // 古いタイプの for 文を用いている．
    auto node = node_list[rpos];
    op(node);
    for ( auto onode: node->fanout_list() ) {
      set_mark(onode, node_list, mark_array);
    }
  }
  return node_list;
}

// @brief TFI のノードを求める．
vector<const TpgNode*>
TpgNodeSet::get_tfi_list(
  SizeType max_size,
  const vector<const TpgNode*>& root_list
)
{
  return get_tfi_list(max_size, root_list, [](const TpgNode*){});
}

// @brief TFI のノードを求める．
vector<const TpgNode*>
TpgNodeSet::get_tfi_list(
  SizeType max_size,
  const vector<const TpgNode*>& root_list,
  std::function<void(const TpgNode*)> op
)
{
  vector<const TpgNode*> node_list;
  node_list.reserve(max_size);
  vector<bool> mark_array(max_size, false);

  for ( auto node: root_list ) {
    set_mark(node, node_list, mark_array);
  }
  for ( SizeType rpos = 0; rpos < node_list.size(); ++ rpos ) {
    // set_tfo_mark() 中で node_list に要素を追加しているので
    // 古いタイプの for 文を用いている．
    auto node = node_list[rpos];
    op(node);
    for ( auto onode: node->fanin_list() ) {
      set_mark(onode, node_list, mark_array);
    }
  }
  return node_list;
}

BEGIN_NONAMESPACE

void
dfs_sub(
  const TpgNode* node,
  vector<bool>& mark_array,
  std::function<void(const TpgNode*)> pre_func,
  std::function<void(const TpgNode*)> post_func
)
{
  if ( !mark_array[node->id()] ) {
    mark_array[node->id()] = true;

    pre_func(node);

    for ( auto inode: node->fanin_list() ) {
      dfs_sub(inode, mark_array, pre_func, post_func);
    }

    post_func(node);
  }
}

END_NONAMESPACE

// @brief 出力からの DFS を行う．
void
TpgNodeSet::dfs(
  SizeType max_size,
  const vector<const TpgNode*>& root_list,
  std::function<void(const TpgNode*)> pre_func,
  std::function<void(const TpgNode*)> post_func
)
{
  vector<bool> mark_array(max_size, false);
  for ( auto node: root_list ) {
    dfs_sub(node, mark_array, pre_func, post_func);
  }
}

END_NAMESPACE_DRUID
