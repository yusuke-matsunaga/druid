
/// @file EventQ.cc
/// @brief EventQ の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "EventQ.h"
#include "SimNode.h"


BEGIN_NAMESPACE_DRUID_FSIM

BEGIN_NONAMESPACE
const bool debug = false;
END_NONAMESPACE

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

BEGIN_NONAMESPACE

inline
std::string
val_str2(
  FSIM_VALTYPE val
)
{
  std::ostringstream buf;
#if FSIM_VAL2
  for ( SizeType i = 0; i < 4; ++ i ) {
    PackedVal bit = 1UL << i;
    if ( val & bit ) {
      buf << " 1";
    }
    else {
      buf << " 0";
    }
  }
#elif FSIM_VAL3
  for ( SizeType i = 0; i < 4; ++ i ) {
    PackedVal bit = 1UL << i;
    if ( val.val0() & bit ) {
      buf << " 0";
    }
    else if ( val.val1() & bit ) {
      buf << " 1";
    }
    else {
      buf << " X";
    }
  }
#endif
  return buf.str();
}

END_NONAMESPACE

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
    auto new_val = old_val;
    if ( node->need_init() ) {
      new_val = node->init_val();
    }
    else {
      new_val = node->calc_val();
    }
    if ( node->has_flip_mask() ) {
      // node に反転イベントがある場合
      auto flip_mask = mFlipMaskArray[node->id()];
      new_val ^= flip_mask;
    }
    node->set_val(new_val);
    if ( debug ) {
      std::cout << "Node#" << node->id() << std::endl
		<< " old_val = " << val_str2(old_val) << std::endl
		<< " new_val = " << val_str2(new_val) << std::endl;
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
