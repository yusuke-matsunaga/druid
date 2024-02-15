
/// @file SimEngine.cc
/// @brief SimEngine の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "SimEngine.h"
#include "SyncObj.h"
#include "InputVals.h"


BEGIN_NAMESPACE_DRUID_FSIM

//////////////////////////////////////////////////////////////////////
// クラス SimEngine
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SimEngine::SimEngine(
  SizeType id,
  SyncObj& sync_obj,
  FSIM_CLASSNAME& fsim,
  const vector<const SimFFR*>& ffr_list
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

// @brief SPSFP 法のシミュレーションを行う．
bool
SimEngine::spsfp(
  const InputVals& iv,
  const SimFault* f,
  DiffBits& dbits
)
{
  SizeType NPO = mFsim.ppo_num();
  dbits = DiffBits{NPO};

  // 正常値の計算を行う．
  _calc_gval(iv);

  // FFR の根までの伝搬条件を求める．
  auto local_obs = local_prop(f);

  // local_obs が 0 ならその後のシミュレーションを行う必要はない．
  if ( local_obs != PV_ALL0 ) {
    // FFR の根のノードを求める．
    auto root = f->origin_node()->ffr_root();

    // イベントシミュレーションを行う．
    put_event(root, local_obs);
    auto obs_array = simulate();
    auto obs = obs_array.back();
    if ( obs != PV_ALL0 ) {
      // 検出された
      for ( SizeType i = 0; i < NPO; ++ i ) {
	if ( obs_array[i] == PV_ALL1 ) {
	  dbits.set_val(i);
	}
      }
      return true;
    }
  }
  return false;
}

void
SimEngine::ppsfp(
  const InputVals& iv
)
{
  if ( mDebug ) {
    log("ppsfp() start");
  }

  // 結果のリストをクリアする．
  clear_results();

  // 正常値の計算を行う．
  _calc_gval(iv);

  // FFR ごとに処理を行う．
  auto NPO = mFsim.ppo_num();
  auto NFFR = mFsim.ffr_num();
  auto NT = mSyncObj.thread_num();
  for ( auto ffr: mFFRList ) {
    auto ffr_req = foreach_faults(*ffr);
    if ( ffr_req == PV_ALL0 ) {
      // ffr_req が 0 ならその後のシミュレーションは必要ない．
      continue;
    }

    // イベントシミュレーションを行う．
    auto root = ffr->root();
    put_event(ffr->root(), ffr_req);
    auto obs_array = simulate();
    auto obs = obs_array.back() & iv.bitmask();
    if ( obs != PV_ALL0 ) {
      // FFR の故障伝搬値とマージする．
      for ( auto ff: ffr->fault_list() ) {
	if ( ff->skip() ) {
	  continue;
	}
	auto pat = ff->obs_mask() & obs;
	if ( pat != PV_ALL0 ) {
	  // 検出された
	  auto tpg_f = ff->tpg_fault();
	  for ( SizeType i = 0; i < PV_BITLEN; ++ i ) {
	    PackedVal bitmask = 1UL << i;
	    if ( pat & bitmask ) {
	      DiffBits dbits(NPO);
	      for ( SizeType j = 0; j < NPO; ++ j ) {
		if ( obs_array[j] & bitmask ) {
		  dbits.set_val(j);
		}
	      }
	      mResList[i].push_back({tpg_f, dbits});
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
SimEngine::sppfp(
  const InputVals& iv
)
{
  if ( mDebug ) {
    log("sppfp() start");
  }

  // 結果のリストをクリアする．
  clear_results();

  // 正常値の計算を行う．
  _calc_gval(iv);

  auto NPO = mFsim.ppo_num();
  auto NFFR = mFsim.ffr_num();
  auto NT = mSyncObj.thread_num();
  vector<const SimFFR*> ffr_array;
  ffr_array.reserve(PV_BITLEN);
  for ( auto ffr: mFFRList ) {
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
      DiffBits dbits(NPO);
      dbits.set_val(root->output_id());
      for ( auto ff: ffr->fault_list() ) {
	if ( !ff->skip() && ff->obs_mask() != PV_ALL0 ) {
	  auto tpg_f = ff->tpg_fault();
	  mResList[0].push_back({tpg_f, dbits});
	}
      }
    }
    else {
      SizeType pos = ffr_array.size();
      PackedVal mask = 1UL << pos;
      ffr_array.push_back(ffr);
      put_event(root, mask);
      if ( ffr_array.size() == PV_BITLEN ) {
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
SimEngine::sppfp_simulation(
  const vector<const SimFFR*>& ffr_array
)
{
  auto obs_array = simulate();
  auto obs = obs_array.back();
  SizeType NPO = mFsim.ppo_num();
  for ( SizeType i = 0; i < ffr_array.size(); ++ i ) {
    PackedVal mask = 1UL << i;
    if ( (obs & mask) == PV_ALL0 ) {
      continue;
    }
    auto& ffr = *ffr_array[i];
    auto& fault_list = ffr.fault_list();
    DiffBits dbits(NPO);
    for ( SizeType j = 0; j < NPO; ++ j ) {
      if ( obs_array[j] & mask ) {
	dbits.set_val(j);
      }
    }
    for ( auto f: fault_list ) {
      if ( !f->skip() && (f->obs_mask() & obs) != PV_ALL0 ) {
	auto tpg_f = f->tpg_fault();
	mResList[0].push_back({tpg_f, dbits});
      }
    }
    mask <<= 1;
  }
}

void
SimEngine::log(
  const string& msg
)
{
  ostringstream buf;
  buf << "[THR#" << mId << "]: " << msg;
  mSyncObj.log(buf.str());
}

#if FSIM_COMBI
// @brief 正常値の計算を行う．(縮退故障用)
void
SimEngine::_calc_gval(
  const InputVals& input_vals
)
{
  // 入力の設定を行う．
  input_vals.set_val(mFsim, mValArray);

  // 正常値の計算を行う．
  _calc_val(mValArray);
}
#endif

#if FSIM_BSIDE
// @brief 正常値の計算を行う．(遷移故障用)
void
SimEngine::_calc_gval(
  const InputVals& input_vals
)
{
  // 1時刻目の入力を設定する．
  input_vals.set_val1(mFsim, mPrevValArray);

  // 1時刻目の正常値の計算を行う．
  _calc_val(mPrevValArray);

  // DFF の出力の値を入力にコピーする．
  for ( auto i: Range(mFsim.dff_num()) ) {
    auto onode = mFsim.dff_input(i);
    auto inode = mFsim.dff_output(i);
    auto val = mPrevValArray[onode->id()];
    mValArray[inode->id()] = val;
  }

  // 2時刻目の入力を設定する．
  input_vals.set_val2(mFsim, mValArray);

  // 2時刻目の正常値の計算を行う．
  _calc_val(mValArray);
}
#endif

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
vector<PackedVal>
SimEngine::simulate()
{
  auto NO = mFsim.ppo_num() + 1;
  vector<PackedVal> prop_array(NO, PV_ALL0);

  // どこかの外部出力で検出されたことを表すビット
  auto obs = PV_ALL0;
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
	prop_array[node->output_id()] = dbits;
	obs |= dbits;
      }
      else {
	mEventQ.put_fanouts(node);
      }
    }
  }
  prop_array.back() = obs;

  // 今の故障シミュレーションで値の変わったノードを元にもどしておく
  for ( auto& rinfo: mClearArray ) {
    mValArray[rinfo.mId] = rinfo.mVal;
  }
  mClearArray.clear();

  return prop_array;
}

END_NAMESPACE_DRUID_FSIM
