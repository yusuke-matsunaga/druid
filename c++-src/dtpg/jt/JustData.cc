
/// @file JustData.cc
/// @brief JustData の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "JustData.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス JustData
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ(遷移故障用)
JustData::JustData(
  const TpgNetwork& network,    ///< [in] 対象のネットワーク
  const VidMap& var1_map,       ///< [in] 1時刻目の変数番号のマップ
  const VidMap& var2_map,       ///< [in] 2時刻目の変数番号のマップ
  const SatModel& model,        ///< [in] SATソルバの作ったモデル
  const AssignList& assign_list ///< [in] 事前の値割り当て
) : mVar1Map{var1_map},
    mVar2Map{var2_map},
    mSatModel{model},
    mFixedMark(network.node_num() * 2, false)
{
  // assign_list に関係するノードに fixed マークを付ける．
  // ただし，含意関係を調べないといけないのでまずトポロジカル順の
  // リストを作る．
  TpgNodeList start_list[2];
  for ( auto as: assign_list ) {
    auto node = as.node();
    auto time = as.time();
    set_fixed(node, time);
    start_list[time].push_back(node);
  }
  std::vector<TpgNode> node_list[2];
  network.get_tfo_list(start_list[0],
		       [&](const TpgNode& node)->bool {
			 node_list[0].push_back(node);
			 if ( node.is_dff_input() ) {
			   auto alt_node = node.alt_node();
			   start_list[1].push_back(alt_node);
			 }
			 return true;
		       });
  network.get_tfo_list(start_list[1],
		       [&](const TpgNode& node)->bool {
			 node_list[1].push_back(node);
			 return true;
		       });
  // 実は TpgNode のノード番号はトポロジカル順になっている．
  for ( auto time: {0, 1} ) {
    std::sort(node_list[time].begin(), node_list[time].end(),
	      [](const TpgNode& a, const TpgNode& b) -> bool {
		return a.id() < b.id();
	      });
    for ( auto node: node_list[time] ) {
      if ( !fixed_mark(node, time) ) {
	if ( check_fixed(node, time) ) {
	  set_fixed(node, time);
	}
      }
    }
  }
}

// @brief 値が確定しているか調べる．
bool
JustData::check_fixed(
  const TpgNode& node,
  int time
)
{
  if ( node.is_ppi() ) {
    return false;
  }
  if ( node.fanin_num() == 1 ) {
    // BUF か NOT
    auto inode = node.fanin(0);
    return fixed_mark(inode, time);
  }

  if ( val(node, time) == node.coval() ) {
    // ファンインの中に確定している制御値を持つノードがあるか調べる．
    for ( auto inode: node.fanin_list() ) {
      if ( val(inode, time) == node.cval() &&
	   fixed_mark(inode, time) ) {
	return true;
      }
    }
    return false;
  }
  else {
    // すべてのファンインの値が確定しているか調べる．
    for ( auto inode: node.fanin_list() ) {
      if ( !fixed_mark(inode, time) ) {
	return false;
      }
    }
    return true;
  }
}

END_NAMESPACE_DRUID
