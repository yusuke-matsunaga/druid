
/// @file SPPFP_Thread.cc
/// @brief SPPFP_Thread の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "SPPFP_Thread.h"


BEGIN_NAMESPACE_DRUID_FSIM

//////////////////////////////////////////////////////////////////////
// クラス SPPFP_Thread
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SPPFP_Thread::SPPFP_Thread(
  FSIM_CLASSNAME& fsim,
  CmdQueue& cmd_queue,
  cbtype callback
) : mFsim{fsim},
    mCmdQueue{cmd_queue},
    mCallBack{callback}
{
  mEventQ.copy_val(mFsim.val_array());
}

// @brief デストラクタ
SPPFP_Thread::~SPPFP_Thread()
{
}

// @brief スレッド実行の本体
void
SPPFP_Thread::operator()()
{
  SizeType NPO = mFsim.ppo_num();
  SizeType NFFR = mFsim.ffr_array().size();
  mFFRPos = 0;
  for ( ; ; ) {
    auto id = mCmdQueue.get();
    if ( id >= NFFR ) {
      break;
    }
    // FFR 内の故障伝搬を行う．
    // 結果は SimFault.mObsMask に保存される．
    // FFR 内の全ての obs マスクを ffr_req に入れる．
    auto& ffr = mFsim.ffr_array()[id];
    auto ffr_req = mFsim.foreach_faults(ffr);
    if ( ffr_req == PV_ALL0 ) {
      // ffr_req が 0 ならその後のシミュレーションを行う必要はない．
      continue;
    }
    auto root = ffr.root();
    if ( root->is_output() ) {
      // 常にこの出力のみで観測可能
      DiffBits dbits(NPO);
      dbits.set_val(root->output_id());
      for ( auto ff: ffr.fault_list() ) {
	if ( !ff->skip() && ff->obs_mask() != PV_ALL0 ) {
	  auto tpg_f = ff->tpg_fault();
	  mCallBack(0, tpg_f, dbits);
	}
      }
    }
    else {
      mFFRArray[mFFRPos] = &ffr;
      PackedVal mask = 1UL << mFFRPos;
      mEventQ.put_event(root, mask);
      ++ mFFRPos;
      if ( mFFRPos == PV_BITLEN ) {
	do_simulation();
      }
    }
  }
  if ( mFFRPos > 0 ) {
    do_simulation();
  }
}

// @brief 実際にイベントドリヴンシミュレーションを行う．
void
SPPFP_Thread::do_simulation()
{
  SizeType NPO = mFsim.ppo_num();
  auto obs_array = mEventQ.simulate();
  PackedVal mask = 1UL;
  for ( SizeType i = 0; i < mFFRPos; ++ i ) {
    auto& ffr = *mFFRArray[i];
    auto& fault_list = ffr.fault_list();
    DiffBits dbits(NPO);
    for ( SizeType j = 0; j < NPO; ++ j ) {
      if ( obs_array[j] & mask ) {
	dbits.set_val(j);
      }
    }
    for ( auto f: fault_list ) {
      if ( !f->skip() && f->obs_mask() != PV_ALL0 ) {
	auto tpg_f = f->tpg_fault();
	mCallBack(0, tpg_f, dbits);
      }
    }
  }
}

END_NAMESPACE_DRUID_FSIM
