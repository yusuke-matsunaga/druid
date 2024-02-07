
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
  PPSFP_CmdQueue& cmd_queue,
  cbtype callback
) : mFsim{fsim},
    mCmdQueue{cmd_queue},
    mCallBack{callback}
{
}

// @brief デストラクタ
PPSFP_Thread::~PPSFP_Thread()
{
}

// @brief スレッド実行の本体
void
PPSFP_Thread::operator()()
{
  SizeType NFFR = mFsim.ffr_array().size();
  for ( ; ; ) {
    auto id = mCmdQueue.get();
    if ( id >= NFFR ) {
      break;
    }

    // タイムスタンプを調べて同期を行う．
    if ( mTimeStamp != mFsim.timestamp() ) {
      mEventQ.copy_val(mFsim.val_array());
      mTimeStamp = mFsim.timestamp();
    }
    auto& ffr = mFsim.ffr_array(id);
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
    if ( obs != PV_ALL0 ) {
      // FFR の故障伝搬値とマージする．
      for ( auto ff: ffr.fault_list() ) {
	if ( ff->skip() ) {
	  continue;
	}
	auto pat = ff->obs_mask() & obs;
	if ( pat != PV_ALL0 ) {
	  // 検出された
	  for ( SizeType i = 0; i < npat; ++ i ) {
	    PackedVal bitmask = 1UL << i;
	    if ( pat & bitmask ) {
	      auto NPO = mFsim.ppo_num();
	      DiffBits dbits(NPO);
	      for ( SizeType i = 0; i < NPO; ++ i ) {
		if ( obs_array[i] & bitmask ) {
		  dbits.set_val(i);
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
