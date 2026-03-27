
/// @file ExtSimple.cc
/// @brief ExtSimple の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "ExtSimple.h"
#include "PropGraph.h"
#include "dtpg/SuffCond.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE
static int debug = 0;
END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// クラス ExtSimple
//////////////////////////////////////////////////////////////////////

// @brief 指定された出力からバックトレースを行う．
SuffCond
ExtSimple::backtrace(
  const PropGraph& data,
  const TpgNode& output
)
{
  mPropGraph = &data;

  mAssignList.clear();

  // output から入力側にDFSを行う．
  // fcone 内のノードは正常値と故障値を区別する．
  std::unordered_set<SizeType> mark;
  dfs(output, true, mark);
  dfs(output, false, mark);

  return SuffCond(mAssignList, {});
#if 0
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
#endif
}

// @brief 深さ優先順で探索を行う．
void
ExtSimple::dfs(
  const TpgNode& node,
  bool gf,
  std::unordered_set<SizeType>& mark
)
{
  if ( debug ) {
    std::cout << "dfs(Node#" << node.id()
	      << ", ";
    if ( gf ) {
      std::cout << "gval";
    }
    else {
      std::cout << "fval";
    }
    std::cout << ")" << std::endl;
  }

  if ( node == mPropGraph->root() ) {
    // 起点に到達した．
    return;
  }

  if ( gf ) {
    if ( mark.count(node.id() * 2 + 1) > 0 ) {
      return;
    }
  }
  else {
    if ( mark.count(node.id() * 2 + 0) > 0 ) {
      return;
    }
  }

  auto pg_node = get_node(node.id());
  if ( pg_node->is_in_fcone() ) {
    if ( gf ) {
      mark.insert(node.id() * 2 + 1);
      if ( pg_node->gval3() == node.coval() ) {
	// ファンインのなかで制御値を持つノードを探す．
	TpgNodeList cval_inode_list;
	for ( auto inode: node.fanin_list() ) {
	  auto pg_inode = get_node(inode.id());
	  if ( pg_inode->gval3() == node.cval() ) {
	    cval_inode_list.push_back(inode);
	  }
	}
	// その中で値が確定しているノードを優先的に選ぶ．
	bool found = false;
	for ( auto inode: cval_inode_list ) {
	  auto pg_inode = get_node(inode.id());
	  if ( pg_inode->is_gval_fixed() ) {
	    dfs(inode, true, mark);
	    found = true;
	    break;
	  }
	}
	if ( !found ) {
	  auto inode = cval_inode_list[0];
	  dfs(inode, true, mark);
	}
      }
      else {
	// 全てのファンインに再帰する．
	for ( auto inode: node.fanin_list() ) {
	  dfs(inode, true, mark);
	}
      }
    }
    else {
      mark.insert(node.id() * 2 + 0);
      if ( pg_node->fval3() == node.coval() ) {
	// ファンインのなかで制御値を持つノードを探す．
	TpgNodeList cval_inode_list;
	for ( auto inode: node.fanin_list() ) {
	  auto pg_inode = get_node(inode.id());
	  if ( pg_inode->fval3() == node.cval() ) {
	    cval_inode_list.push_back(inode);
	  }
	}
	// その中で値が確定しているノードを優先的に選ぶ．
	bool found = false;
	for ( auto inode: cval_inode_list ) {
	  auto pg_inode = get_node(inode.id());
	  if ( pg_inode->is_fval_fixed() ) {
	    dfs(inode, false, mark);
	    found = true;
	    break;
	  }
	}
	if ( !found ) {
	  auto inode = cval_inode_list[0];
	  dfs(inode, false, mark);
	}
      }
      else {
	// 全てのファンインに再帰する．
	for ( auto inode: node.fanin_list() ) {
	  dfs(inode, false, mark);
	}
      }
    }
  }
  else {
    // 値を記録する．
    mark.insert(node.id() * 2 + 1);
    mark.insert(node.id() * 2 + 0);
    auto bval = pg_node->gval();
    mAssignList.add(node, 1, bval);
  }
}

// @brief PgNode を取り出す．
PgNode*
ExtSimple::get_node(
  SizeType id
)
{
  return mPropGraph->get_node(id);
}

#if 0
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
#endif

END_NAMESPACE_DRUID
