
/// @file Just2.cc
/// @brief Just2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "Just2.h"
#include "JustData.h"
#include "TpgDff.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

int debug = 0;

END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// クラス Just2
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
Just2::Just2(
  SizeType max_id
) : JustImpl{max_id},
    mWeightArray(max_id * 2, 0U),
    mTmpArray(max_id * 2, 0.0)
{
  mNodeList[0].reserve(max_id);
  mNodeList[1].reserve(max_id);
}

// @brief デストラクタ
Just2::~Just2()
{
}

// @brief 初期化処理
void
Just2::just_init(
  const NodeValList& assign_list,
  const JustData& jd
)
{
  // ヒューリスティックで用いる重みを計算する．
  for ( auto time: {0, 1} ) {
    mNodeList[time].clear();
  }
  for ( auto nv: assign_list ) {
    add_weight(jd, nv.node(), nv.time());
  }
  for ( auto time: {0, 1} ) {
    for ( auto node: mNodeList[time] ) {
      calc_value(jd, node, time);
    }
  }
}

// @brief 制御値を持つファンインを一つ選ぶ．
const TpgNode*
Just2::select_cval_node(
  const JustData& jd,
  const TpgNode* node,
  int time
)
{
  double min_val = DBL_MAX;
  const TpgNode* min_node = nullptr;
  Val3 cval = node->cval();
  for ( auto inode: node->fanin_list() ) {
    Val3 ival = jd.val(inode, time);
    if ( ival != cval ) {
      continue;
    }
    double val = node_value(inode, time);
    if ( min_val > val ) {
      min_val = val;
      min_node = inode;
    }
  }
  ASSERT_COND ( min_node != nullptr );

  return min_node;
}

// @brief 終了処理
void
Just2::just_end()
{
  // 作業領域をクリアしておく．
  for ( auto time: { 0, 1 } ) {
    for ( auto node: mNodeList[time] ) {
      int index = node->id() * 2 + time;
      mWeightArray[index] = 0;
      mTmpArray[index] = 0.0;
    }
    mNodeList[time].clear();
  }
}

// @brief 重みの計算を行う．
void
Just2::add_weight(
  const JustData& jd,
  const TpgNode* node,
  int time
)
{
  SizeType index = node->id() * 2 + time;
  ++ mWeightArray[index];
  if ( mWeightArray[index] > 1 ) {
    return;
  }

  if ( debug ) {
    cout << "add_weight(Node#" << node->id() << "@" << time
	 << " = " << jd.val(node, time) << ")" << endl;
  }

  if ( node->is_primary_input() ) {
    ;
  }
  else if ( node->is_dff_output() ) {
    if ( time == 1 && jd.td_mode() ) {
      // 1時刻前のタイムフレームに戻る．
      const TpgDff* dff = node->dff();
      const TpgNode* alt_node = dff->input();
      add_weight(jd, alt_node, 0);
    }
  }
  else {
    Val3 oval = jd.val(node, time);
    if ( oval == node->coval() ) {
      // cval をもつノードをたどる．
      Val3 cval = node->cval();
      for ( auto inode: node->fanin_list() ) {
	Val3 ival = jd.val(inode, time);
	if ( ival == cval ) {
	  add_weight(jd, inode, time);
	}
      }
    }
    else {
      // すべてのファンインをたどる．
      for ( auto inode: node->fanin_list() ) {
	add_weight(jd, inode, time);
      }
    }
  }

  // post order で mNodeList に入れる．
  mNodeList[time].push_back(node);
}

// @brief 見積もり値の計算を行う．
void
Just2::calc_value(
  const JustData& jd,
  const TpgNode* node,
  int time
)
{
  if ( mTmpArray[node->id() * 2 + time] != 0.0 ) {
    return;
  }

  double val = 0.0;
  if ( node->is_primary_input() ) {
    val = 1.0;
  }
  else if ( node->is_dff_output() ) {
    if ( time == 1 && jd.td_mode() ) {
      const TpgDff* dff = node->dff();
      const TpgNode* alt_node = dff->input();
      val = node_value(alt_node, 0);
    }
    else {
      val = 1.0;
    }
  }
  else {
    Val3 oval = jd.val(node, time);
    if ( oval == node->coval() ) {
      // cval を持つファンインのうち最小の値を求める．
      double min_val = DBL_MAX;
      Val3 cval = node->cval();
      for ( auto inode: node->fanin_list() ) {
	Val3 ival = jd.val(inode, time);
	if ( ival != cval ) {
	  continue;
	}
	calc_value(jd, inode, time);
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
      for ( auto inode: node->fanin_list() ) {
	calc_value(jd, inode, time);
	val += node_value(inode, time);
      }
    }
  }
  mTmpArray[node->id() * 2 + time] = val;
}

END_NAMESPACE_DRUID
