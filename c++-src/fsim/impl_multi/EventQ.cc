
/// @file EventQ.cc
/// @brief EventQ の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018, 2022, 2024 Yusuke Matsunaga
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
  mOutputNum = output_num;
  mPropArray.clear();
  mPropArray.resize(mOutputNum, PV_ALL0);

  mArray.clear();
  mArray.resize(max_level + 1, nullptr);

  mEvNodeMap.clear();
  mEvNodeMap.resize(node_num, nullptr);

  mClearArray.clear();
  mClearArray.reserve(node_num);
  mFlipMaskArray.clear();
  mFlipMaskArray.resize(node_num, PV_ALL0);

  mValArray.clear();
  mValArray.resize(node_num);

  mCurLevel = 0;
  mNum = 0;
}

// @brief イベントドリブンシミュレーションを行う．
PackedVal
EventQ::simulate()
{
  clear_prop_val();

  // どこかの外部出力で検出されたことを表すビット
  auto obs = PV_ALL0;
  for ( ; ; ) {
    auto node = get();
    // イベントが残っていなければ終わる．
    if ( node == nullptr ) break;

    auto old_val = get_val(node);
    auto new_val = node->calc_val(mValArray);
    // 反転イベントを考慮する．
    auto flip_mask = mFlipMaskArray[node->id()];
    new_val ^= flip_mask;
    mFlipMaskArray[node->id()] = PV_ALL0;
    set_val(node, new_val);
    if ( new_val != old_val ) {
      mValArray[node->id()] = new_val;
      add_to_clear_list(node, old_val);
      if ( node->is_output() ) {
	auto dbits = diff(new_val, old_val);
	mPropArray[node->output_id()] = dbits;
	obs |= dbits;
      }
      else {
	put_fanouts(node);
      }
    }
  }

  // 今の故障シミュレーションで値の変わったノードを元にもどしておく
  for ( auto& rinfo: mClearArray ) {
    mValArray[rinfo.mId] = rinfo.mVal;
  }
  mClearArray.clear();

  return obs;
}

// @brief mPropArray をクリアする．
void
EventQ::clear_prop_val()
{
  for ( auto i: Range(0, mOutputNum) ) {
    mPropArray[i] = PV_ALL0;
  }
}

END_NAMESPACE_DRUID_FSIM
