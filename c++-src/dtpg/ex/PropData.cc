
/// @file PropData.cc
/// @brief PropData の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "PropData.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// @brief 境界ノードを求める．
void
backtrace(
  const TpgNode* node,
  vector<const TpgNode*>& node_list,
  const std::unordered_set<SizeType>& fcone_mark,
  const std::unordered_set<SizeType>& boundary_mark
)
{
  std::unordered_set<SizeType> mark;
  std::deque<const TpgNode*> queue;

  mark.emplace(node->id());
  queue.push_back(node);

  while ( !queue.empty() ) {
    auto node = queue.front();
    queue.pop_front();
    if ( boundary_mark.count(node->id()) > 0 ) {
      node_list.push_back(node);
    }
    for ( auto inode: node->fanin_list() ) {
      if ( fcone_mark.count(inode->id()) > 0 ) {
	// inode をキューに積む．
	if ( mark.count(inode->id()) == 0 ) {
	  mark.emplace(inode->id());
	  queue.push_back(inode);
	}
      }
    }
  }
}

END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// クラス PropData
//////////////////////////////////////////////////////////////////////

PropData::PropData(
  const TpgNode* root,
  const VidMap& gvar_map,
  const VidMap& fvar_map,
  const SatModel& model
) : mRoot{root},
    mGvarMap{gvar_map},
    mFvarMap{fvar_map},
    mSatModel{model}
{
  // root の TFO (fault cone) に印をつける．
  std::unordered_set<SizeType> fcone_mark;
  std::unordered_set<SizeType> boundary_mark;
  std::deque<const TpgNode*> queue;
  queue.push_back(root);
  vector<const TpgNode*> output_list;
  while ( !queue.empty() ) {
    auto node = queue.front();
    queue.pop_front();
    if ( node->is_ppo() ) {
      if ( gval(node) != fval(node) ) {
	output_list.push_back(node);
      }
    }
    for ( auto onode: node->fanout_list() ) {
      if ( fcone_mark.count(onode->id()) == 0 ) {
	fcone_mark.emplace(onode->id());
	queue.push_back(onode);
      }
    }
    // node が境界ノードかどうか調べる．
    // ただし，XOR/XNORノードは境界ノードを持たない．
    if ( node->gate_type() == PrimType::Xor ||
	 node->gate_type() == PrimType::Xnor ) {
      continue;
    }
    bool has_side_input = false;
    for ( auto inode: node->fanin_list() ) {
      if ( fcone_mark.count(inode->id()) == 0 ) {
	has_side_input = true;
	break;
      }
    }
    if ( has_side_input ) {
      // node は境界ノード
      boundary_mark.emplace(node->id());
      vector<const TpgNode*> cnode_list;
      vector<const TpgNode*> side_inputs;
      for ( auto inode: node->fanin_list() ) {
	if ( fcone_mark.count(inode->id()) > 0 ) {
	  continue;
	}
	if ( gval(inode) == node->cval() ) {
	  cnode_list.push_back(inode);
	}
	else {
	  side_inputs.push_back(inode);
	}
      }
      if ( cnode_list.empty() ) {
	vector<Assign> assign_list;
	for ( auto snode: side_inputs ) {
	  auto assign = get_assign(snode);
	  assign_list.push_back(assign);
	}
	mAssignDataDict.emplace(node->id(), AssignData{true, assign_list});
      }
      else {
	vector<Assign> assign_list;
	for ( auto cnode: cnode_list ) {
	  auto assign = get_assign(cnode);
	  assign_list.push_back(assign);
	}
	mAssignDataDict.emplace(node->id(), AssignData{false, assign_list});
      }
    }
  }

  for ( auto node: output_list ) {
    vector<const TpgNode*> node_list;
    backtrace(node, node_list, fcone_mark, boundary_mark);
    mBoundaryDataList.push_back({node, std::move(node_list)});
  }
}

END_NAMESPACE_DRUID
