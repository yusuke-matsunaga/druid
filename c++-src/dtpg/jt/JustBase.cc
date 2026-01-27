
/// @file JustBase.cc
/// @brief JustBase の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "JustBase.h"
#include "JustData.h"

#define DEBUG_OUT std::cout


BEGIN_NONAMESPACE

int debug_jt = 0;

END_NONAMESPACE


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス JustBase
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
JustBase::JustBase(
  const TpgNetwork& network
) : Justifier{network},
    mMarkArray(network.node_num(), 0U)
{
}

// @brief デストラクタ
JustBase::~JustBase()
{
}

// @brief justify の実際の処理
AssignList
JustBase::_justify(
  const AssignList& assign_list
)
{
  // マークをクリアする．
  for ( auto& m: mMarkArray ) {
    m = 0U;
  }

  just_init(assign_list);

  mQueue.clear();
  for ( auto nv: assign_list ) {
    auto node = nv.node();
    int time = nv.time();
    put_queue(node, time);
  }

  AssignList pi_assign_list;
  for ( SizeType rpos = 0; rpos < mQueue.size(); ++ rpos ) {
    auto& p = mQueue[rpos];
    auto& node = p.first;
    int time = p.second;
    just_main(node, time, pi_assign_list);
  }

  just_end();

  return pi_assign_list;
}

// @brief 正当化に必要な割当を求める．
void
JustBase::just_main(
  const TpgNode& node,
  int time,
  AssignList& pi_assign_list
)
{
  if ( debug_jt ) {
    auto oval = just_data().val(node, time);
    DEBUG_OUT << node << "@" << time << ": " << oval
	      << std::endl;
  }

  if ( node.is_primary_input() ) {
    // 外部入力なら値を記録する．
    just_data().record_value(node, time, pi_assign_list);
    return;
  }

  if ( node.is_dff_output() ) {
    if ( time == 1 && has_prev_state() ) {
      // DFF の出力で1時刻目の場合は0時刻目に戻る．
      auto alt_node = node.alt_node();
      put_queue(alt_node, 0);
    }
    else {
      // DFFを擬似入力だと思って値を記録する．
      just_data().record_value(node, time, pi_assign_list);
    }
    return;
  }

  auto oval = just_data().val(node, time);
  if ( oval == node.coval() ) {
    // cval を持つファンインを選ぶ．
    auto inode = select_cval_node(node, time);
    // そのノードに再帰する．
    put_queue(inode, time);
  }
  else {
    // すべてのファンインに再帰する．
    for ( auto inode: node.fanin_list() ) {
      put_queue(inode, time);
    }
  }
}

END_NAMESPACE_DRUID
