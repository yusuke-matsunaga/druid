
/// @file SimEngine.cc
/// @brief SimEngine の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "SimEngine.h"
#include "SyncObj.h"


BEGIN_NAMESPACE_DRUID_FSIM

//////////////////////////////////////////////////////////////////////
// クラス SimEngine
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SimEngine::SimEngine(
  SizeType id,
  SyncObj& sync_obj,
  FSIM_CLASSNAME& fsim,
  const std::vector<const SimFFR*>& ffr_list
) : mId{id},
    mSyncObj{sync_obj},
    mFsim{fsim},
    mFlipMaskArray(fsim.node_num(), PV_ALL0),
    mEventQ{fsim.max_level(), fsim.node_num()},
    mValArray(fsim.node_num()),
#if FSIM_BSIDE
    mPrevValArray(fsim.node_num()),
#endif
    mFFRList{ffr_list}
{
  mClearArray.reserve(fsim.node_num());
  mDebug = false;
  if ( mDebug ) {
    log("instantiated");
  }
}

// @brief デストラクタ
SimEngine::~SimEngine()
{
}

// @brief スレッド本体の実行関数
void
SimEngine::run()
{
  for ( bool go_on = true; go_on; ) {
    auto cmd = mSyncObj.get_command(mId);
    switch ( cmd ) {
    case Cmd::PPSFP:
      sppfp();
      break;

    case Cmd::SPPFP:
      ppsfp();
      break;

    case Cmd::END:
      go_on = false;
      break;
    }
  }
}

// @brief SPSFP 法のシミュレーションを行う．
bool
SimEngine::spsfp(
  const SimFault* f,
  DiffBits& dbits
)
{
  _copy_val();

  SizeType NPO = mFsim.ppo_num();
  dbits.clear();

  // FFR の根までの伝搬条件を求める．
  auto local_obs = local_prop(f);

  // local_obs が 0 ならその後のシミュレーションを行う必要はない．
  if ( local_obs != PV_ALL0 ) {
    // FFR の根のノードを求める．
    auto root = f->origin_node()->ffr_root();

    // イベントシミュレーションを行う．
    put_event(root, local_obs);
    auto dbits_array = simulate();
    if ( dbits_array.elem_num() > 0 ) {
      // 検出された
      dbits = dbits_array.get_slice(0);
      return true;
    }
  }
  return false;
}

// @brief SPPFP 法のシミュレーションを行う．
void
SimEngine::sppfp()
{
  if ( mDebug ) {
    log("sppfp() start");
  }

  // FsimX の値をコピーする．
  _copy_val();

  // 結果を初期化する．
  mRes = std::unique_ptr<FsimResultsRep>{new FsimResultsRep(1)};

  for ( ; ; ) {
    // 処理すべき FFR のリストを取り出す．
    auto ffr_list = mSyncObj.ffr_list(PV_BITLEN);
    if ( ffr_list.empty() ) {
      // 終わり
      break;
    }
    auto nffr = ffr_list.size();
    std::vector<const SimFFR*> ffr_array;
    ffr_array.reserve(nffr);
    for ( auto ffr: ffr_list ) {
      // FFR 内の故障伝搬を行う．
      // 結果は SimFault.mObsMask に保存される．
      // FFR 内の全ての obs マスクを ffr_req に入れる．
      auto ffr_req = foreach_faults(*ffr);
      if ( ffr_req == PV_ALL0 ) {
	// ffr_req が 0 ならその後のシミュレーションを行う必要はない．
	continue;
      }

      auto root = ffr->root();
      if ( root->is_output() ) {
	// 常にこの出力のみで観測可能
	DiffBits dbits;
	// この dbits は1ビットのみなので sort() は必要ない．
	dbits.add_output(root->output_id());
	for ( auto ff: ffr->fault_list() ) {
	  if ( !ff->skip() && ff->obs_mask() != PV_ALL0 ) {
	    auto fid = ff->id();
	    mRes->add(0, fid, dbits);
	  }
	}
      }
      else {
	SizeType pos = ffr_array.size();
	PackedVal mask = 1UL << pos;
	ffr_array.push_back(ffr);
	put_event(root, mask);
      }
    }
    if ( ffr_array.empty() ) {
      // シミュレーションすべき FFR が残っていなかった．
      continue;
    }

    auto dbits_array = simulate();
    auto obs = dbits_array.dbits_union();
    for ( SizeType i = 0; i < ffr_array.size(); ++ i ) {
      PackedVal mask = 1UL << i;
      if ( (obs & mask) == PV_ALL0 ) {
	continue;
      }
      auto& ffr = *ffr_array[i];
      auto& fault_list = ffr.fault_list();
      auto dbits = dbits_array.get_slice(i);
      for ( auto f: fault_list ) {
	if ( !f->skip() && (f->obs_mask() & obs) != PV_ALL0 ) {
	  auto fid = f->id();
	  mRes->add(0, fid, dbits);
	}
      }
      mask <<= 1;
    }
  }
  if ( mDebug ) {
    log("sppfp() end");
  }
}

void
SimEngine::ppsfp()
{
  if ( mDebug ) {
    log("ppsfp() start");
  }

  // FsimX の値をコピーする．
  _copy_val();

  auto ntv = mSyncObj.ntv();

  // 結果を初期化する．
  mRes = std::unique_ptr<FsimResultsRep>{new FsimResultsRep(ntv)};

  // データを持っているビットを表すビットマスク
  PackedVal bitmask = 0UL;
  for ( SizeType i = 0; i < ntv; ++ i ) {
    bitmask |= (1UL << i);
  }

  for ( ; ; ) {
    // 処理すべき FFR を取り出す．
    auto ffr = mSyncObj.ffr();
    if ( ffr == nullptr ) {
      // 終わり
      break;
    }

    // FFR ごとに処理を行う．
    auto ffr_req = foreach_faults(*ffr) & bitmask;
    if ( ffr_req == PV_ALL0 ) {
      // ffr_req が 0 ならその後のシミュレーションは必要ない．
      continue;
    }

    // イベントシミュレーションを行う．
    auto root = ffr->root();
    put_event(ffr->root(), ffr_req);
    auto dbits_array = simulate();
    auto gobs = dbits_array.dbits_union();
    if ( gobs != PV_ALL0 ) {
      // FFR の故障伝搬値とマージする．
      for ( auto ff: ffr->fault_list() ) {
	if ( ff->skip() ) {
	  continue;
	}
	if ( (ff->obs_mask() & gobs) != PV_ALL0 ) {
	  // 検出された
	  auto fid = ff->id();
	  auto dbits_array1 = dbits_array.masking(ff->obs_mask());
	  for ( SizeType i = 0; i < ntv; ++ i ) {
	    auto dbits = dbits_array1.get_slice(i);
	    if ( dbits.elem_num() > 0 ) {
	      mRes->add(i, fid, dbits);
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
SimEngine::log(
  const std::string& msg
)
{
  std::ostringstream buf;
  buf << "[THR#" << mId << "]: " << msg;
  mSyncObj.log(buf.str());
}

// @brief Fsim の値をコピーする．
void
SimEngine::_copy_val()
{
  for ( auto node: mFsim.node_list() ) {
    auto node_id = node->id();
    mValArray[node_id] = mFsim.val(node_id);
#if FSIM_BSIDE
    mPrevValArray[node_id] = mFsim.prev_val(node_id);
#endif
  }
}

// @brief 個々の故障に FaultProp を適用する．
PackedVal
SimEngine::foreach_faults(
  const SimFFR& ffr
)
{
  auto& fault_list = ffr.fault_list();
  auto ffr_req = PV_ALL0;
  for ( auto ff: fault_list ) {
    if ( ff->skip() ) {
      continue;
    }

    auto obs = local_prop(ff);
    ff->set_obs_mask(obs);
    ffr_req |= obs;
  }

  return ffr_req;
}

// @brief イベントドリブンシミュレーションを行う．
DiffBitsArray
SimEngine::simulate()
{
  DiffBitsArray dbits_array;

  for ( ; ; ) {
    auto node = mEventQ.get();
    // イベントが残っていなければ終わる．
    if ( node == nullptr ) break;

    auto old_val = get_val(node);
    auto new_val = node->calc_val(mValArray);
    // 反転イベントを考慮する．
    auto flip_mask = mFlipMaskArray[node->id()];
    new_val ^= flip_mask;
    mFlipMaskArray[node->id()] = PV_ALL0;
    set_val(node, new_val);
    if ( new_val != old_val ) {
      mValArray[node->id()] = new_val;
      add_to_clear_list(node, old_val);
      if ( node->is_output() ) {
	auto dbits = diff(new_val, old_val);
	dbits_array.add_output(node->output_id(), dbits);
      }
      else {
	mEventQ.put_fanouts(node);
      }
    }
  }

  // 今の故障シミュレーションで値の変わったノードを元にもどしておく
  for ( auto& rinfo: mClearArray ) {
    mValArray[rinfo.mId] = rinfo.mVal;
  }
  mClearArray.clear();

  return dbits_array;
}

END_NAMESPACE_DRUID_FSIM
