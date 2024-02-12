
/// @file ThrFunc.cc
/// @brief ThrFunc の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "ThrFunc.h"
#include "SyncObj.h"


BEGIN_NAMESPACE_DRUID_FSIM

//////////////////////////////////////////////////////////////////////
// クラス ThrFunc
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
ThrFunc::ThrFunc(
  SizeType id,
  FSIM_CLASSNAME& fsim,
  SyncObj& sync
) : mId{id},
    mFsim{fsim},
    mSyncObj{sync},
    mEventQ{fsim.max_level(), fsim.ppo_num(), fsim.node_num()}
{
  //mDebug = true;
  if ( mDebug ) {
    log("instantiated");
  }
}

// @brief デストラクタ
ThrFunc::~ThrFunc()
{
}

// @brief スレッド実行の本体
void
ThrFunc::main_loop()
{
  if ( mDebug ) {
    log("main_loop start");
  }
  // 全体の流れ
  //
  // - メインスレッドが準備できるまで待つ
  // - FFR番号を取り出して処理する．
  // - 処理が終わったら待ちに戻る．
  for ( bool go_on = true; go_on; ) {
    auto cmd = mSyncObj.get_command(mId);
    switch ( cmd ) {
    case Cmd::PPSFP:
      ppsfp();
      break;

    case Cmd::SPPFP:
      sppfp();
      break;

    case Cmd::END:
      go_on = false;
      break;
    }
  }
  if ( mDebug ) {
    log("main_loop end");
  }
}

void
ThrFunc::ppsfp()
{
  if ( mDebug ) {
    log("ppsfp() start");
  }
  mEventQ.init(mFsim);
  for ( SizeType i = 0; i < PV_BITLEN; ++ i ) {
    mResList[i].clear();
  }
  auto NPO = mFsim.ppo_num();
  auto NFFR = mFsim.ffr_array().size();
  auto NT = mSyncObj.thread_num();
  for ( SizeType id = mId; id < NFFR; id += NT ) {
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
    if ( obs != PV_ALL0 ) {
      // FFR の故障伝搬値とマージする．
      for ( auto ff: ffr.fault_list() ) {
	if ( ff->skip() ) {
	  continue;
	}
	auto pat = ff->obs_mask() & obs;
	if ( pat != PV_ALL0 ) {
	  // 検出された
	  for ( SizeType i = 0; i < PV_BITLEN; ++ i ) {
	    PackedVal bitmask = 1UL << i;
	    if ( pat & bitmask ) {
	      DiffBits dbits(NPO);
	      for ( SizeType j = 0; j < NPO; ++ j ) {
		if ( obs_array[j] & bitmask ) {
		  dbits.set_val(j);
		}
	      }
	      mResList[i].push_back({ff->tpg_fault(), dbits});
	    }
	  }
	}
      }
    }
  }
  if ( mDebug ) {
    log("ppsfp() end");
  }
}

void
ThrFunc::sppfp()
{
  if ( mDebug ) {
    log("sppfp() start");
  }
  mEventQ.init(mFsim);
  for ( SizeType i = 0; i < PV_BITLEN; ++ i ) {
    mResList[i].clear();
  }
  auto NPO = mFsim.ppo_num();
  auto NFFR = mFsim.ffr_array().size();
  vector<const SimFFR*> ffr_array;
  ffr_array.reserve(PV_BITLEN);
  for ( ; ; ) {
    auto id = mSyncObj.get_id();
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
	  mResList[0].push_back({ff->tpg_fault(), dbits});
	}
      }
    }
    else {
      SizeType pos = ffr_array.size();
      PackedVal mask = 1UL << pos;
      ffr_array.push_back(&ffr);
      mEventQ.put_event(root, mask);
      if ( pos == PV_BITLEN - 1) {
	sppfp_simulation(ffr_array);
	ffr_array.clear();
      }
    }
  }
  if ( !ffr_array.empty() ) {
    sppfp_simulation(ffr_array);
  }
  if ( mDebug ) {
    log("sppfp() end");
  }
}

// @brief 実際にイベントドリヴンシミュレーションを行う．
void
ThrFunc::sppfp_simulation(
  const vector<const SimFFR*>& ffr_array
)
{
  SizeType NPO = mFsim.ppo_num();
  auto obs_array = mEventQ.simulate();
  PackedVal mask = 1UL;
  for ( auto& ffr_p: ffr_array ) {
    auto& fault_list = ffr_p->fault_list();
    DiffBits dbits(NPO);
    for ( SizeType j = 0; j < NPO; ++ j ) {
      if ( obs_array[j] & mask ) {
	dbits.set_val(j);
      }
    }
    for ( auto f: fault_list ) {
      if ( !f->skip() && f->obs_mask() != PV_ALL0 ) {
	auto tpg_f = f->tpg_fault();
	mResList[0].push_back({tpg_f, dbits});
      }
    }
  }
}

void
ThrFunc::log(
  const string& msg
)
{
  ostringstream buf;
  buf << "[THR#" << mId << "]: " << msg;
  mSyncObj.log(buf.str());
}

END_NAMESPACE_DRUID_FSIM
