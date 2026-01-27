
/// @file Just2.cc
/// @brief Just2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "Just2.h"
#include "JustData.h"

#define DEBUG_OUT std::cout


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

int debug = 0;

END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// クラス Just2
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
Just2::Just2(
  const TpgNetwork& network
) : JustBase{network},
    mWeightArray(network.node_num() * 2, 0U),
    mTmpArray(network.node_num() * 2, 0.0)
{
  for ( auto i: {0, 1} ) {
    mNodeList[i].reserve(network.node_num());
  }
}

// @brief デストラクタ
Just2::~Just2()
{
}

// @brief 初期化処理
void
Just2::just_init(
  const AssignList& assign_list
)
{
  // ヒューリスティックで用いる重みを計算する．
  for ( auto time: {0, 1} ) {
    mNodeList[time].clear();
  }
  for ( auto nv: assign_list ) {
    add_weight(nv.node(), nv.time());
  }
  for ( auto time: {0, 1} ) {
    for ( auto node: mNodeList[time] ) {
      calc_value(node, time);
    }
  }
}

// @brief 制御値を持つファンインを一つ選ぶ．
TpgNode
Just2::select_cval_node(
  const TpgNode& node,
  int time
)
{
  double min_val = DBL_MAX;
  TpgNode min_node;
  auto cval = node.cval();
  for ( auto inode: node.fanin_list() ) {
    auto ival = just_data().val(inode, time);
    if ( ival == cval ) {
      auto val = node_value(inode, time);
      if ( min_val > val ) {
	min_val = val;
	min_node = inode;
      }
    }
  }
  if ( !min_node.is_valid() ) {
    throw std::logic_error{"min_node not found"};
  }

  return min_node;
}

// @brief 終了処理
void
Just2::just_end()
{
  // 作業領域をクリアしておく．
  for ( auto time: { 0, 1 } ) {
    for ( auto& node: mNodeList[time] ) {
      SizeType index = node.id() * 2 + time;
      mWeightArray[index] = 0;
      mTmpArray[index] = 0.0;
    }
    mNodeList[time].clear();
  }
}

// @brief 重みの計算を行う．
void
Just2::add_weight(
  const TpgNode& node,
  int time
)
{
  SizeType index = node.id() * 2 + time;
  ++ mWeightArray[index];
  if ( mWeightArray[index] > 1 ) {
    return;
  }

  if ( debug ) {
    DEBUG_OUT << "add_weight(" << node << "@" << time
	      << " = " << just_data().val(node, time) << ")"
	      << std::endl;
  }

  if ( node.is_primary_input() ) {
    ;
  }
  else if ( node.is_dff_output() ) {
    if ( time == 1 && has_prev_state() ) {
      // 1時刻前のタイムフレームに戻る．
      auto alt_node = node.alt_node();
      add_weight(alt_node, 0);
    }
  }
  else {
    auto oval = just_data().val(node, time);
    if ( oval == node.coval() ) {
      // cval をもつノードをたどる．
      auto cval = node.cval();
      for ( auto inode: node.fanin_list() ) {
	auto ival = just_data().val(inode, time);
	if ( ival == cval ) {
	  add_weight(inode, time);
	}
      }
    }
    else {
      // すべてのファンインをたどる．
      for ( auto inode: node.fanin_list() ) {
	add_weight(inode, time);
      }
    }
  }

  // post order で mNodeList に入れる．
  mNodeList[time].push_back(node);
}

// @brief 見積もり値の計算を行う．
void
Just2::calc_value(
  const TpgNode& node,
  int time
)
{
  if ( mTmpArray[node.id() * 2 + time] != 0.0 ) {
    return;
  }

  double val = 0.0;
  if ( node.is_primary_input() ) {
    val = 1.0;
  }
  else if ( node.is_dff_output() ) {
    if ( time == 1 && has_prev_state() ) {
      auto alt_node = node.alt_node();
      val = node_value(alt_node, 0);
    }
    else {
      val = 1.0;
    }
  }
  else {
    auto oval = just_data().val(node, time);
    if ( oval == node.coval() ) {
      // cval を持つファンインのうち最小の値を求める．
      double min_val = DBL_MAX;
      auto cval = node.cval();
      for ( auto inode: node.fanin_list() ) {
	auto ival = just_data().val(inode, time);
	if ( ival != cval ) {
	  continue;
	}
	calc_value(inode, time);
	double val1 = node_value(inode, time);
	if ( min_val > val1 ) {
	  min_val = val1;
	}
      }
      ASSERT_COND ( min_val < DBL_MAX );

      val = min_val;
    }
    else {
      // すべてのファンインノードをたどる．
      for ( auto inode: node.fanin_list() ) {
	calc_value(inode, time);
	val += node_value(inode, time);
      }
    }
  }
  mTmpArray[node.id() * 2 + time] = val;
}

END_NAMESPACE_DRUID
