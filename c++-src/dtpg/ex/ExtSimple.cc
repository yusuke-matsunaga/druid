
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
    if ( debug ) {
      std::cout << " reached to the root" << std::endl;
    }
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
    if ( debug ) {
      std::cout << "  Node#" << node.id()
		<< " => " << bval << std::endl;
    }
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

END_NAMESPACE_DRUID
