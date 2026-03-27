
/// @file PropGraph.cc
/// @brief PropGraph の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "PropGraph.h"
#include "types/TpgNetwork.h"
#include "types/TpgNodeList.h"
#include "types/AssignList.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE
static int debug = 0;
END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// クラス PropGraph
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
PropGraph::PropGraph(
  const TpgNode& root,
  const VidMap& gvar_map,
  const VidMap& fvar_map,
  const SatModel& model,
  const AssignList& assign_list
) : mRoot{root}
{
  // root の TFO (fault cone) に印をつける．
  // 同時に故障差の伝搬している外部出力のリストを作る．
  // TpgNodeList をキューとして用いる．
  // ただし，あとでもう一回使うので中身は残したまま
  TpgNodeList queue;

  // キューに入っていることを表すマーク
  std::unordered_set<SizeType> qmark;
  // キューの読み出し位置
  SizeType rpos = 0;
  queue.push_back(root);
  qmark.insert(root.id());
  TpgNodeList output_list;
  while ( rpos < queue.size() ) {
    auto node = queue[rpos];
    ++ rpos;

    auto gval = model[gvar_map(node)] == SatBool3::True;
    auto fval = model[fvar_map(node)] == SatBool3::True;
    auto pg_node = new PgNode(node.id(), gval, fval);
    put_node(pg_node);
    if ( node.is_ppo() ) {
      if ( gval != fval ) {
	output_list.push_back(node);
      }
    }
    // node のファンアウトをキューに積む．
    for ( auto onode: node.fanout_list() ) {
      if ( qmark.count(onode.id()) == 0 ) {
	queue.push_back(onode);
	qmark.insert(onode.id());
      }
    }
  }

  // assign_list に関係するノードとそのTFOを登録する．
  for ( auto as: assign_list ) {
    auto node = as.node();
    auto time = as.time();
    if ( time == 1 ) {
      get_tfo(node, gvar_map, model);
    }
  }

  // assign_list で値が正当化されるノードに印を付ける．
  // TpgNode::id() は入力からのトポロジカル順になっている．
  std::vector<PgNode*> tmp_list;
  for ( auto& pg_node: mNodePool ) {
    tmp_list.push_back(pg_node.get());
  }
  std::sort(tmp_list.begin(), tmp_list.end(),
	    [&](const PgNode* a, const PgNode* b) -> bool {
	      return a->id() < b->id();
	    });
  // assign_list に関連するノードのマーク
  std::unordered_set<SizeType> mark;
  for ( auto as: assign_list ) {
    auto node = as.node();
    auto time = as.time();
    if ( time == 1 ) {
      mark.insert(node.id());
    }
  }
  auto network = root.network();
  for ( auto pg_node: tmp_list ) {
    if ( pg_node->id() == root.id() ) {
      // root の gval/fval は確定している．
      pg_node->set_gval_fixed();
      pg_node->set_fval_fixed();
    }
    else if ( mark.count(pg_node->id()) > 0 ) {
      pg_node->set_gval_fixed();
      if ( !pg_node->is_in_fcone() ) {
	pg_node->set_fval_fixed();
      }
    }
    else {
      auto node = network.node(pg_node->id());
      // 正常値が確定しているか調べる．
      if ( check_fixed(pg_node, node, true) ) {
	pg_node->set_gval_fixed();
      }
      if ( pg_node->is_in_fcone() ) {
	// 故障値が確定しているか調べる．
	if ( check_fixed(pg_node, node, false) ) {
	  pg_node->set_fval_fixed();
	}
      }
      else {
	if ( pg_node->is_gval_fixed() ) {
	  pg_node->set_fval_fixed();
	}
      }
    }
  }

  // 出力のリストを作る．
  for ( auto node: output_list ) {
    auto pg_node = get_node(node.id());
    if ( pg_node->is_gval_fixed() && pg_node->is_fval_fixed() ) {
      mJustifiedSensitizedOutputList.push_back(node);
    }
    else {
      mSensitizedOutputList.push_back(node);
    }
  }

  // fcone 外の side input を登録する．
  for ( auto node: queue ) {
    for ( auto inode: node.fanin_list() ) {
      if ( mNodeDict.count(inode.id()) == 0 ) {
	auto gval = model[gvar_map(inode)] == SatBool3::True;
	auto pg_node = new PgNode(inode.id(), gval);
	put_node(pg_node);
      }
    }
  }

  if ( debug ) {
    std::cout << "PropGraph(root = Node#" << root.id() << ")" << std::endl;
    std::vector<PgNode*> tmp_list;
    for ( auto& pg_node: mNodePool ) {
      tmp_list.push_back(pg_node.get());
    }
    std::sort(tmp_list.begin(), tmp_list.end(),
	      [&](const PgNode* a, const PgNode* b) -> bool {
		return a->id() < b->id();
	      });
    for ( auto pg_node: tmp_list ) {
      auto node = network.node(pg_node->id());
      std::cout << "Node#" << pg_node->id()
		<< " [";
      if ( node.is_logic() ) {
	std::cout << node.gate_type();
      }
      else if ( node.is_ppi() ) {
	std::cout << "Input#" << node.input_id();
      }
      else if ( node.is_ppo() ) {
	std::cout << "Output#" << node.output_id();
      }
      std::cout << "] ";
      if ( pg_node->is_in_fcone() ) {
	std::cout << value_name1(pg_node->gval3());
	if ( pg_node->is_gval_fixed() ) {
	  std::cout << "*";
	}
	std::cout << "/" << value_name1(pg_node->fval3());
	if ( pg_node->is_fval_fixed() ) {
	  std::cout << "*";
	}
      }
      else {
	std::cout << value_name1(pg_node->gval3());
	if ( pg_node->is_gval_fixed() ) {
	  std::cout << "*";
	}
      }
      std::cout << std::endl;
      if ( node.is_logic() ) {
	for ( auto inode: node.fanin_list() ) {
	  std::cout << "  Node#" << inode.id();
	  if ( mNodeDict.count(inode.id()) > 0 ) {
	    auto pg_inode = get_node(inode.id());
	    std::cout << " "
		      << value_name1(pg_inode->gval3());
	    if ( pg_inode->is_gval_fixed() ) {
	      std::cout << "*";
	    }
	    if ( pg_inode->is_in_fcone() ) {
	      std::cout << "/"
			<< value_name1(pg_inode->fval3());
	      if ( pg_inode->is_fval_fixed() ) {
		std::cout << "*";
	      }
	    }
	  }
	  std::cout << std::endl;
	}
      }
    }
  }
}

// @brief node の TFO を登録する．
void
PropGraph::get_tfo(
  const TpgNode& node,
  const VidMap& gvar_map,
  const SatModel& model
)
{
  if ( mNodeDict.count(node.id()) > 0 ) {
    // 登録済み
    return;
  }

  // fcone 外であるはず
  auto glit = gvar_map(node);
  if ( glit == SatLiteral::X ) {
    // 完全な範囲外
    return;
  }
  bool gval = model[glit] == SatBool3::True;
  auto pg_node = new PgNode(node.id(), gval);
  put_node(pg_node);

  for ( auto onode: node.fanout_list() ) {
    get_tfo(onode, gvar_map, model);
  }
}

bool
PropGraph::check_fixed(
  PgNode* pg_node,
  const TpgNode& node,
  bool gval
)
{
  if ( node.is_ppi() ) {
    return false;
  }
  if ( node.fanin_num() == 1 ) {
    auto inode = node.fanin(0);
    if ( mNodeDict.count(inode.id())== 0 ) {
      return false;
    }
    auto pg_inode = get_node(inode.id());
    return pg_inode->is_val_fixed(gval);
  }
  if ( pg_node->val3(gval) == node.coval() ) {
    // ファンインの中に確定している制御値を持つノードがあるか調べる．
    for ( auto inode: node.fanin_list() ) {
      if ( mNodeDict.count(inode.id()) == 0 ) {
	continue;
      }
      auto pg_inode = get_node(inode.id());
      if ( pg_inode->val3(gval) == node.cval() &&
	   pg_inode->is_val_fixed(gval) ) {
	return true;
      }
    }
  }
  else {
    // ファンインが全て確定しているか調べる．
    for ( auto inode: node.fanin_list() ) {
      if ( mNodeDict.count(inode.id()) == 0 ) {
	return false;
      }
      auto pg_inode = get_node(inode.id());
      if ( !pg_inode->is_val_fixed(gval) ) {
	return false;
      }
    }
    return true;
  }
  return false;
}

// @brief PgNode を登録する．
void
PropGraph::put_node(
  PgNode* pg_node
)
{
  mNodePool.push_back(std::unique_ptr<PgNode>{pg_node});
  mNodeDict.emplace(pg_node->id(), pg_node);
}

// @brief PgNode を取り出す．
PgNode*
PropGraph::get_node(
  SizeType id
) const
{
  if ( mNodeDict.count(id) == 0 ) {
    throw std::out_of_range{"id is not registered"};
  }
  return mNodeDict.at(id);
}

END_NAMESPACE_DRUID
