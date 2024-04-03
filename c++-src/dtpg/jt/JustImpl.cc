
/// @file JustImpl.cc
/// @brief JustImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "JustImpl.h"
#include "JustData.h"


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
  JustData jd{var_map, model};

  return _justify(jd, assign_list);
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
  JustData jd{var1_map, var2_map, model};

  return _justify(jd, assign_list);
}

// @brief justify の実際の処理
NodeValList
JustImpl::_justify(
  const JustData& jd,
  const NodeValList& assign_list
)
{
  mJustDataPtr = &jd;

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

  NodeValList pi_assign_list;
  for ( SizeType rpos = 0; rpos < mQueue.size(); ++ rpos ) {
    auto& nv = mQueue[rpos];
    auto node = nv.node();
    int time = nv.time();
    just_main(node, time, pi_assign_list);
  }

  just_end();

  mJustDataPtr = nullptr;

  return pi_assign_list;
}

// @brief 正当化に必要な割当を求める．
void
JustImpl::just_main(
  const TpgNode* node,
  int time,
  NodeValList& pi_assign_list
)
{
  if ( node->is_primary_input() ) {
    // 外部入力なら値を記録する．
    just_data().record_value(node, time, pi_assign_list);
    return;
  }

  if ( node->is_dff_output() ) {
    if ( time == 1 && just_data().td_mode() ) {
      // DFF の出力で1時刻目の場合は0時刻目に戻る．
      auto alt_node = node->alt_node();
      put_queue(alt_node, 0);
    }
    else {
      // DFFを擬似入力だと思って値を記録する．
      just_data().record_value(node, time, pi_assign_list);
    }
    return;
  }

  auto oval = just_data().val(node, time);
  if ( oval == node->coval() ) {
    // cval を持つファンインを選ぶ．
    auto inode = select_cval_node(node, time);
    // そのノードに再帰する．
    put_queue(inode, time);
  }
  else {
    // すべてのファンインに再帰する．
    for ( auto inode: node->fanin_list() ) {
      put_queue(inode, time);
    }
  }
}

END_NAMESPACE_DRUID
