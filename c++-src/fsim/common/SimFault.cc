
/// @file SimFault.cc
/// @brief SimFault の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "SimFault.h"
#include "types/AssignList.h"
#include "SimNode.h"
#include "SimEngine.h"


BEGIN_NAMESPACE_DRUID_FSIM

//////////////////////////////////////////////////////////////////////
// クラス SimFault
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SimFault::SimFault(
  TpgFault f,
  SimNode* node,
  SimEngine& engine
) : mId{f.id()},
    mNode{node}
{
  // もとの excitation_condition を SimNode に置き換える．
  for ( auto nodeval: f.excitation_condition() ) {
    auto src_node = nodeval.node();
    auto val = nodeval.val();
    auto simnode = engine.node(src_node.id());
    if ( nodeval.time() == 1 ) {
      mExCondList.push_back({simnode, val});
    }
#if FSIM_BSIDE
    else {
      mPrevCondList.push_back({simnode, val});
    }
#endif
  }
}

// @brief 故障の活性化条件を求める．
PackedVal
SimFault::excitation_condition() const
{
  auto cond = PV_ALL1;
  for ( auto& tmp: mExCondList ) {
    auto node = tmp.node();
    auto val0 = tmp.val();
#if FSIM_VAL2
    auto val = node->val();
    if ( !val0 ) {
      val = ~val;
    }
#elif FSIM_VAL3
    auto val3 = node->val();
    auto val = val0 ? val3.val1() : val3.val0();
#else
    #error "Neither FSIM_VAL2 nor FSIM_VAL3 are defined"
#endif
    cond &= val;
  }
  return cond;
}

#if FSIM_BSIDE
// @brief 遷移故障用の1時刻前の条件を求める．
PackedVal
SimFault::previous_condition() const
{
  auto cond = PV_ALL1;
  for ( auto& tmp: mPrevCondList ) {
    auto node = tmp.node();
    auto val0 = tmp.val();
#if FSIM_VAL2
    auto val = node->prev_val();
    if ( !val0 ) {
      val = ~val;
    }
#elif FSIM_VAL3
    auto val3 = node->prev_val();
    auto val = val0 ? val3.val1() : val3.val0();
#else
#error "Neither FSIM_VAL2 nor FSIM_VAL3 are defined"
#endif
    cond &= val;
  }
  return cond;
}
#endif

END_NAMESPACE_DRUID_FSIM
