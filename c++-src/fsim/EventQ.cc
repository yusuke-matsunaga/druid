
/// @file EventQ.cc
/// @brief EventQ の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "EventQ.h"
#include "SimNode.h"
#include "GateType.h"


BEGIN_NAMESPACE_DRUID_FSIM

//////////////////////////////////////////////////////////////////////
// 故障シミュレーション用のイベントキューを表すクラス
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
EventQ::EventQ() :
  mArraySize(0),
  mArray(nullptr),
  mNum(0),
  mClearArraySize(0),
  mClearArray(nullptr),
  mClearPos(0),
  mFlipMaskArray(nullptr),
  mMaskPos(0)
{
}

// @brief デストラクタ
EventQ::~EventQ()
{
  delete [] mArray;
  delete [] mClearArray;
  delete [] mFlipMaskArray;
}

// @brief 初期化を行う．
// @param[in] max_level 最大レベル
// @param[in] node_num ノード数
void
EventQ::init(int max_level,
	     int node_num)
{
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

// @brief 初期イベントを追加する．
// @param[in] node 対象のノード
// @param[in] valmask 反転マスク
void
EventQ::put_trigger(SimNode* node,
		    PackedVal valmask,
		    bool immediate)
{
  if ( immediate || node->gate_type() == GateType::Input ) {
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
// @param[in] target 目標のノード
// @retval 出力における変化ビットを返す．
//
// target が nullptr でない時にはイベントが target まで到達したら
// シミュレーションを終える．
// target が nullptr の時には出力ノードまでイベントを伝える．
PackedVal
EventQ::simulate(SimNode* target)
{
  // どこかの外部出力で検出されたことを表すビット
  auto obs = kPvAll0;
  for ( ; ; ) {
    auto node = get();
    // イベントが残っていなければ終わる．
    if ( node == nullptr ) break;

    // すでに検出済みのビットはマスクしておく
    // これは無駄なイベントの発生を抑える．
    auto old_val = node->val();
    node->calc_val(~obs);
    auto new_val = node->val();
    if ( node->has_flip_mask() ) {
      auto flip_mask = mFlipMaskArray[node->id()];
      new_val ^= flip_mask;
      node->set_val(new_val);
    }
    if ( new_val != old_val ) {
      add_to_clear_list(node, old_val);
      if ( node->is_output() || node == target ) {
	auto dbits = diff(new_val, old_val);
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

END_NAMESPACE_DRUID_FSIM
