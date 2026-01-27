
/// @file EventQ.cc
/// @brief EventQ の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "EventQ.h"
#include "SimNode.h"


BEGIN_NAMESPACE_DRUID_FSIM

//////////////////////////////////////////////////////////////////////
// 故障シミュレーション用のイベントキューを表すクラス
//////////////////////////////////////////////////////////////////////

// @brief 初期化を行う．
void
EventQ::init(
  SizeType max_level,
  SizeType output_num,
  SizeType node_num
)
{
  mArray.clear();
  mArray.resize(max_level + 1, nullptr);

  mClearArray.clear();
  mClearArray.reserve(node_num);
  mFlipMaskArray.clear();
  mFlipMaskArray.resize(node_num, PV_ALL0);

  mCurLevel = 0;
  mNum = 0;
}

// @brief イベントドリブンシミュレーションを行う．
DiffBitsArray
EventQ::simulate()
{
  // 結果を格納するオブジェクト
  DiffBitsArray dbits_array;
  for ( ; ; ) {
    auto node = get();
    // イベントが残っていなければ終わる．
    if ( node == nullptr ) break;

    auto old_val = node->val();
    node->calc_val();
    auto new_val = node->val();
    if ( node->has_flip_mask() ) {
      // node に反転イベントがある場合
      auto flip_mask = mFlipMaskArray[node->id()];
      new_val ^= flip_mask;
      node->set_val(new_val);
    }
    if ( new_val != old_val ) {
      add_to_clear_list(node, old_val);
      if ( node->is_output() ) {
	auto dbits = diff(new_val, old_val);
	dbits_array.add_output(node->output_id(), dbits);
      }
      else {
	put_fanouts(node);
      }
    }
  }

  // 今の故障シミュレーションで値の変わったノードを元にもどしておく
  for ( auto& rinfo: mClearArray ) {
    auto node = rinfo.mNode;
    node->set_val(rinfo.mVal);
  }
  mClearArray.clear();

  for ( auto i: Range(0, mMaskPos) ) {
    auto node = mMaskList[i];
    node->clear_flip();
  }
  mMaskPos = 0;

  return dbits_array;
}

END_NAMESPACE_DRUID_FSIM
