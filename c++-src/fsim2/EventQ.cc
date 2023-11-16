﻿
/// @file EventQ.cc
/// @brief EventQ の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "EventQ.h"
#include "SimNode.h"


BEGIN_NAMESPACE_DRUID_FSIM2

//////////////////////////////////////////////////////////////////////
// 故障シミュレーション用のイベントキューを表すクラス
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
EventQ::EventQ(
)
{
}

// @brief デストラクタ
EventQ::~EventQ()
{
  delete [] mPropArray;
  delete [] mArray;
  delete [] mClearArray;
  delete [] mFlipMaskArray;
}

// @brief 初期化を行う．
void
EventQ::init(
  SizeType max_level,
  SizeType node_num,
  SizeType output_num
)
{
  if ( output_num != mOutputNum ) {
    delete [] mPropArray;
    mOutputNum = output_num;
    mPropArray = new PackedVal[output_num];
  }
  if ( max_level >= mArraySize ) {
    delete [] mArray;
    mArraySize = max_level + 1;
    mArray = new SimNode*[mArraySize];
  }
  if ( node_num > mClearArraySize ) {
    delete [] mClearArray;
    delete [] mFlipMaskArray;
    mClearArraySize = node_num;
    mClearArray = new RestoreInfo[mClearArraySize];
    mFlipMaskArray = new PackedVal[mClearArraySize];
  }

  for ( auto i: Range(0, mOutputNum) ) {
    mPropArray[i] = PV_ALL0;
  }
  mCurLevel = 0;
  for ( auto i: Range(0, mArraySize) ) {
    mArray[i] = nullptr;
  }
  mNum = 0;
}

// @brief 初期イベントを追加する．
void
EventQ::put_trigger(
  SimNode* node,
  PackedVal valmask,
  bool immediate
)
{
  if ( immediate || node->gate_type() == PrimType::None ) {
    // 入力の場合，他のイベントの干渉は受けないので
    // 今計算してしまう．
    // もしくは ppsfp のようにイベントが単独であると
    // わかっている場合も即座に計算してしまう．
    auto old_val = node->val();
    node->set_val(old_val ^ valmask);
    add_to_clear_list(node, old_val);
    put_fanouts(node);
  }
  else {
    // 複数のイベントを登録する場合があるので
    // ここでは計算せずに反転マスクのみをセットする．
    set_flip_mask(node, valmask);
    put(node);
  }
}

// @brief イベントドリブンシミュレーションを行う．
PackedVal
EventQ::simulate()
{
  // どこかの外部出力で検出されたことを表すビット
  PackedVal obs = PV_ALL0;
  for ( ; ; ) {
    auto node = get();
    // イベントが残っていなければ終わる．
    if ( node == nullptr ) break;

    auto old_val = node->val();
    node->calc_val(PV_ALL1);
    auto new_val = node->val();
    if ( node->has_flip_mask() ) {
      auto flip_mask = mFlipMaskArray[node->id()];
      new_val ^= flip_mask;
      node->set_val(new_val);
    }
    if ( new_val != old_val ) {
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
  for ( auto i: Range(0, mClearPos) ) {
    auto& rinfo = mClearArray[i];
    auto node = rinfo.mNode;
    node->set_val(rinfo.mVal);
  }
  mClearPos = 0;

  for ( auto i: Range(0, mMaskPos) ) {
    auto node = mMaskList[i];
    node->clear_flip();
  }
  mMaskPos = 0;

  return obs;
}

END_NAMESPACE_DRUID_FSIM2