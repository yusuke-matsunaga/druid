
/// @file ExData.cc
/// @brief ExData の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "ExData.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス ExData
//////////////////////////////////////////////////////////////////////

ExData::ExData(
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
  // 同時に故障差の伝搬している外部出力のリストを作る．
  mFconeMark.emplace(root->id());
  mFconeNodeList.push_back(root);
  for ( SizeType rpos = 0; rpos < mFconeNodeList.size(); ++ rpos ) {
    auto node = mFconeNodeList[rpos];
    if ( node->is_ppo() ) {
      if ( gval(node) != fval(node) ) {
	mSensitizedOutputList.push_back(node);
      }
    }
    for ( auto onode: node->fanout_list() ) {
      if ( mFconeMark.count(onode->id()) == 0 ) {
	mFconeMark.emplace(onode->id());
	mFconeNodeList.push_back(onode);
      }
    }
  }

  for ( auto node: mSensitizedOutputList ) {
    vector<const TpgNode*> node_list;
    backtrace(node, node_list);
    mBoundaryNodeListDict.emplace(node->id(), std::move(node_list));
  }
}

// @brief ノードの side input を求める．
void
ExData::get_side_inputs(
  const TpgNode* node, ///< [in] 対象のノード
  vector<const TpgNode*>& side_inputs,
  vector<const TpgNode*>& cnode_list
) const
{
  for ( auto inode: node->fanin_list() ) {
    if ( is_in_fcone(inode) ) {
      continue;
    }
    if ( gval(inode) == node->cval() ) {
      cnode_list.push_back(inode);
    }
    else {
      side_inputs.push_back(inode);
    }
  }
}

// @brief 境界ノードを求める．
void
ExData::backtrace(
  const TpgNode* onode,
  vector<const TpgNode*>& node_list
)
{
  std::unordered_set<SizeType> mark;
  std::deque<const TpgNode*> queue;

  mark.emplace(onode->id());
  queue.push_back(onode);

  while ( !queue.empty() ) {
    auto node = queue.front();
    queue.pop_front();
    bool has_side_input = false;
    for ( auto inode: node->fanin_list() ) {
      if ( is_in_fcone(inode) ) {
	// inode をキューに積む．
	if ( mark.count(inode->id()) == 0 ) {
	  mark.emplace(inode->id());
	  queue.push_back(inode);
	}
      }
      else {
	if ( node->gate_type() != PrimType::Xor &&
	     node->gate_type() != PrimType::Xnor ) {
	  has_side_input = true;
	}
      }
    }
    if ( has_side_input ) {
      node_list.push_back(node);
    }
  }
}

END_NAMESPACE_DRUID
