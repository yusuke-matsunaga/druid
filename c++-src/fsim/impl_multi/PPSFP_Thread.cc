
/// @file PPSFP_Thread.cc
/// @brief PPSFP_Thread の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "PPSFP_Thread.h"


BEGIN_NAMESPACE_DRUID_FSIM

//////////////////////////////////////////////////////////////////////
// クラス PPSFP_Thread
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
PPSFP_Thread::PPSFP_Thread(
  FSIM_CLASSNAME& fsim,
  CmdQueue& cmd_queue,
  SizeType pat_base,
  SizeType pat_num,
  cbtype callback
) : mFsim{fsim},
    mCmdQueue{cmd_queue},
    mPatBase{pat_base},
    mPatNum{pat_num},
    mCallBack{callback}
{
  mEventQ.init(mFsim.max_level(), mFsim.ppo_num(), mFsim.node_num());
  mEventQ.copy_val(mFsim.val_array());
}

// @brief デストラクタ
PPSFP_Thread::~PPSFP_Thread()
{
}

// @brief スレッド実行の本体
void
PPSFP_Thread::operator()()
{
  auto NPO = mFsim.ppo_num();
  auto NFFR = mFsim.ffr_array().size();
  for ( ; ; ) {
    auto id = mCmdQueue.get();
    if ( id >= NFFR ) {
      break;
    }

    auto& ffr = mFsim.ffr_array()[id];
    auto ffr_req = mFsim.foreach_faults(ffr);
    if ( ffr_req == PV_ALL0 ) {
      // ffr_req が 0 ならその後のシミュレーションは必要ない．
      continue;
    }

    // イベントシミュレーションを行う．
    auto root = ffr.root();
    mEventQ.put_event(ffr.root(), ffr_req);
    auto obs_array = mEventQ.simulate();
    auto obs = obs_array.back();
    cout << "obs = " << obs << endl;
    if ( obs != PV_ALL0 ) {
      // FFR の故障伝搬値とマージする．
      for ( auto ff: ffr.fault_list() ) {
	if ( ff->skip() ) {
	  continue;
	}
	auto pat = ff->obs_mask() & obs;
	if ( pat != PV_ALL0 ) {
	  // 検出された
	  for ( SizeType i = 0; i < mPatNum; ++ i ) {
	    PackedVal bitmask = 1UL << i;
	    if ( pat & bitmask ) {
	      DiffBits dbits(NPO);
	      for ( SizeType j = 0; j < NPO; ++ j ) {
		if ( obs_array[j] & bitmask ) {
		  dbits.set_val(j);
		}
	      }
	      mCallBack(i + mPatBase, ff->tpg_fault(), dbits);
	    }
	  }
	}
      }
    }
  }
}

END_NAMESPACE_DRUID_FSIM
