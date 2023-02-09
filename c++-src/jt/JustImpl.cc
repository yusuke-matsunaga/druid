
/// @file JustImpl.cc
/// @brief JustImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "JustImpl.h"
#include "JustData.h"
#include "TpgDFF.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス JustImpl
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
JustImpl::JustImpl(
  SizeType max_id
) : mMarkArray(max_id, 0U)
{
}

// @brief デストラクタ
JustImpl::~JustImpl()
{
}

// @brief 正当化に必要な割当を求める(縮退故障用)．
NodeValList
JustImpl::justify(
  const NodeValList& assign_list,
  const VidMap& var_map,
  const SatModel& model
)
{
  clear_mark();

  JustData jd{var_map, model};

  just_init(assign_list, jd);

  NodeValList pi_assign_list;
  for ( auto nv: assign_list ) {
    auto node = nv.node();
    int time = nv.time();
    just_main(jd, node, time, pi_assign_list);
  }

  just_end();

  return pi_assign_list;
}

// @brief 正当化に必要な割当を求める(遷移故障用)．
NodeValList
JustImpl::justify(
  const NodeValList& assign_list,
  const VidMap& var1_map,
  const VidMap& var2_map,
  const SatModel& model
)
{
  clear_mark();

  JustData jd{var1_map, var2_map, model};

  just_init(assign_list, jd);

  NodeValList pi_assign_list;
  for ( auto nv: assign_list ) {
    auto node = nv.node();
    int time = nv.time();
    just_main(jd, node, time, pi_assign_list);
  }

  just_end();

  return pi_assign_list;
}

// @brief 正当化に必要な割当を求める．
void
JustImpl::just_main(
  const JustData& jd,
  const TpgNode* node,
  int time,
  NodeValList& pi_assign_list
)
{
  if ( mark(node, time) ) {
    // 処理済みならなにもしない．
    return;
  }
  // 処理済みの印を付ける．
  set_mark(node, time);

  if ( node->is_primary_input() ) {
    // 外部入力なら値を記録する．
    jd.record_value(node, time, pi_assign_list);
    return;
  }

  if ( node->is_dff_output() ) {
    if ( time == 1 && jd.td_mode() ) {
      // DFF の出力で1時刻目の場合は0時刻目に戻る．
      auto dff = node->dff();
      auto alt_node = dff.input();
      just_main(jd, alt_node, 0, pi_assign_list);
    }
    else {
      // DFFを擬似入力だと思って値を記録する．
      jd.record_value(node, time, pi_assign_list);
    }
    return;
  }

  auto oval = jd.val(node, time);
  if ( oval == node->coval() ) {
    // cval を持つファンインを選ぶ．
    auto inode = select_cval_node(jd, node, time);
    // そのノードに再帰する．
    just_main(jd, inode, time, pi_assign_list);
  }
  else {
    // すべてのファンインに再帰する．
    for ( auto inode: node->fanin_list() ) {
      just_main(jd, inode, time, pi_assign_list);
    }
  }
}

// @brief 全てのマークを消す．
void
JustImpl::clear_mark()
{
  for ( auto& m: mMarkArray ) {
    m = 0U;
  }
}

END_NAMESPACE_DRUID
