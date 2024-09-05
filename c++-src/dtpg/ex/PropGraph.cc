
/// @file PropGraph.cc
/// @brief PropGraph の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "PropGraph.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス PropGraph
//////////////////////////////////////////////////////////////////////

PropGraph::PropGraph(
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
  vector<const TpgNode*> tmp_list;
  mFconeMark.emplace(root->id());
  tmp_list.push_back(root);
  for ( SizeType rpos = 0; rpos < tmp_list.size(); ++ rpos ) {
    auto node = tmp_list[rpos];
    if ( node->is_ppo() ) {
      if ( gval(node) != fval(node) ) {
	mSensitizedOutputList.push_back(node);
      }
    }
    for ( auto onode: node->fanout_list() ) {
      if ( mFconeMark.count(onode->id()) == 0 ) {
	mFconeMark.emplace(onode->id());
	tmp_list.push_back(onode);
      }
    }
  }
}

END_NAMESPACE_DRUID
