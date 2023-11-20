
/// @file EventQ.cc
/// @brief EventQ の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "EventQ.h"
#include "SimNode.h"


BEGIN_NAMESPACE_DRUID_FSIM

//////////////////////////////////////////////////////////////////////
// 故障シミュレーション用のイベントキューを表すクラス
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
EventQ::EventQ(
){
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
  SizeType output_num,
  SizeType node_num
)
{
  if ( output_num != mOutputNum ) {
    delete [] mPropArray;
    mOutputNum = output_num;
    mPropArray = new PackedVal[mOutputNum];
  }
  for ( auto i: Range(0, mOutputNum) ) {
    mPropArray[i] = PV_ALL0;
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

  mCurLevel = 0;
  for ( auto i: Range(0, mArraySize) ) {
    mArray[i] = nullptr;
  }
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
	obs |= dbits;
	mPropArray[node->output_id()] = dbits;
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

// @brief mPropArray をクリアする．
void
EventQ::clear_prop_val()
{
  for ( auto i: Range(0, mOutputNum) ) {
    mPropArray[i] = PV_ALL0;
  }
}

END_NAMESPACE_DRUID_FSIM
