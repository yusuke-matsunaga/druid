
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
  mPropArray.clear();
  mPropArray.resize(output_num + 1, PV_ALL0);

  mArray.clear();
  mArray.resize(max_level + 1, nullptr);

  mEventMap.clear();
  mEventMap.resize(node_num, nullptr);

  mFlipMaskArray.clear();
  mFlipMaskArray.resize(node_num, PV_ALL0);

  mValArray.clear();
  mValArray.resize(node_num);

  mClearArray.clear();
  mClearArray.reserve(node_num);

  mCurLevel = 0;
  mNum = 0;
}

// @brief イベントドリブンシミュレーションを行う．
vector<PackedVal>
EventQ::simulate()
{
  for ( auto i = 0; i < mPropArray.size(); ++ i ) {
    mPropArray[i] = PV_ALL0;
  }

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
#if FSIM_VAL2
    cout << "Node#" << node->id() << ": " << old_val << " => " << new_val << endl;
#endif
#if FSIM_VAL3
    cout << "Node#" << node->id() << ":" << old_val.val0() << ":" << old_val.val1()
	 << " => " << new_val.val0() << ":" << new_val.val1() << endl;
#endif
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
  mPropArray.back() = obs;

  // 今の故障シミュレーションで値の変わったノードを元にもどしておく
  for ( auto& rinfo: mClearArray ) {
    mValArray[rinfo.mId] = rinfo.mVal;
  }
  mClearArray.clear();

  return mPropArray;
}

END_NAMESPACE_DRUID_FSIM
