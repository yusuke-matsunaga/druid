
/// @file SimThrFunc.cc
/// @brief SimThrFunc の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "SimThrFunc.h"


BEGIN_NAMESPACE_DRUID_FSIM

BEGIN_NONAMESPACE

// 0/1 を PackedVal/PackedVal3 に変換する．
inline
FSIM_VALTYPE
bool_to_packedval(
  bool val
)
{
#if FSIM_VAL2
  return val ? PV_ALL1 : PV_ALL0;
#elif FSIM_VAL3
  return val ? PackedVal3(PV_ALL1) : PackedVal3(PV_ALL0);
#endif
}

// Val3 を PackedVal/PackedVal3 に変換する．
inline
FSIM_VALTYPE
val3_to_packedval(
  Val3 val
)
{
#if FSIM_VAL2
  // kValX は kVal0 とみなす．
  return (val == Val3::_1) ? PV_ALL1 : PV_ALL0;
#elif FSIM_VAL3
  switch ( val ) {
  case Val3::_X: return PackedVal3(PV_ALL0, PV_ALL0);
  case Val3::_0: return PackedVal3(PV_ALL1, PV_ALL0);
  case Val3::_1: return PackedVal3(PV_ALL0, PV_ALL1);
  }
#endif
}

// bit のビットに値を設定する．
inline
void
bit_set(
  FSIM_VALTYPE& val,
  Val3 ival,
  PackedVal bit
)
{
#if FSIM_VAL2
  if ( ival == Val3::_1 ) {
    val |= bit;
  }
#elif FSIM_VAL3
  FSIM_VALTYPE val1 = val3_to_packedval(ival);
  val.set_with_mask(val1, bit);
#endif
}

// PackedVal/PackedVal3 を Val3 に変換する．
// 最下位ビットだけで判断する．
inline
Val3
packedval_to_val3(
  FSIM_VALTYPE pval
)
{
#if FSIM_VAL2
  return (pval & 1UL) ? Val3::_1 : Val3::_0;
#elif FSIM_VAL3
  if ( pval.val0() & 1UL) {
    return Val3::_0;
  }
  else if ( pval.val1() & 1UL ) {
    return Val3::_1;
  }
  else {
    return Val3::_X;
  }
#endif
}

END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// クラス SimThrFunc
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SimThrFunc::SimThrFunc(
  SizeType id,
  FSIM_CLASSNAME& fsim
) : mId{id},
    mFsim{fsim},
    mFlipMaskArray(fsim.node_num(), PV_ALL0),
    mEventQ{fsim.max_level(), fsim.node_num()},
    mValArray(fsim.node_num())
#if FSIM_BSIDE
  ,mPrevValArray(fsim.node_num())
#endif
{
  mClearArray.reserve(fsim.node_num());
  mDebug = false;
  if ( mDebug ) {
    log("instantiated");
  }
}

// @brief デストラクタ
SimThrFunc::~SimThrFunc()
{
}

#if FSIM_COMBI
// @brief 正常値の計算を行う．(縮退故障用)
void
SimThrFunc::calc_gval(
  const TestVector& tv
)
{
  // 入力の設定を行う．
  for ( SizeType iid = 0; iid < mFsim.ppi_num(); ++ iid ) {
    auto simnode = mFsim.ppi(iid);
    auto val3 = tv.ppi_val(iid);
    mValArray[simnode->id()] = val3_to_packedval(val3);
  }

  // 正常値の計算を行う．
  _calc_val(mValArray);
}

// @brief 正常値の計算を行う．
void
SimThrFunc::calc_gval(
  const std::vector<TestVector>& tv_list
)
{
  // 設定されていないビットはどこか他の設定されているビットをコピーする．
  auto x_val = FSIM_INITVAL;
  for ( SizeType iid = 0; iid < mFsim.ppi_num(); ++ iid ) {
    auto simnode = mFsim.ppi(iid);
    auto val = x_val;
    PackedVal bit = 1UL;
    for ( SizeType pos = 0; pos < PV_BITLEN; ++ pos, bit <<= 1 ) {
      SizeType epos = (pos < tv_list.size()) ? pos : 0;
      auto ival = tv_list[epos].ppi_val(iid);
      bit_set(val, ival, bit);
    }
    mValArray[simnode->id()] = val;
  }

  // 正常値の計算を行う．
  _calc_val(mValArray);
}

// @brief 正常値の計算を行う．
void
SimThrFunc::calc_gval(
    const AssignList& assign_list
)
{
  // デフォルト値で初期化する．
  auto val0 = FSIM_INITVAL;
  for ( auto simnode: mFsim.ppi_list() ) {
    mValArray[simnode->id()] = val0;
  }

  for ( auto nv: assign_list ) {
    if ( nv.time() != 1 ) {
      throw std::logic_error{"nv.time() != 1"};
    }
    auto iid = nv.node().input_id();
    auto simnode = mFsim.ppi(iid);
    mValArray[simnode->id()] = bool_to_packedval(nv.val());
  }

  // 正常値の計算を行う．
  _calc_val(mValArray);
}

// @brief 正常値の計算を行う．
void
SimThrFunc::calc_gvalx(
    const AssignList& assign_list
)
{
  // デフォルト値で初期化する．
  auto x_val = FSIM_INITVAL;
  for ( auto simnode: mFsim.logic_list() ) {
    mValArray[simnode->id()] = x_val;
  }

  // 値をセットする．
  for ( auto nv: assign_list ) {
    if ( nv.time() != 1 ) {
      throw std::logic_error{"nv.time() != 1"};
    }
    auto node_id = nv.node().id();
    auto val = nv.val();
    auto simnode = mFsim.node(node_id);
    mValArray[simnode->id()] = bool_to_packedval(val);
  }

  // 正常値の計算を行う．
  _calc_val(mValArray);
}
#endif

#if FSIM_BSIDE
// @brief 正常値の計算を行う．(遷移故障用)
void
SimThrFunc::calc_gval(
  const TestVector& tv
)
{
  // 1時刻目の入力を設定する．
  for ( SizeType iid = 0; iid < mFsim.ppi_num(); ++ iid ) {
    auto simnode = mFsim.ppi(iid);
    auto val3 = tv.ppi_val(iid);
    mPrevValArray[simnode->id()] = val3_to_packedval(val3);
  }

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
  for ( SizeType iid = 0; iid < mFsim.input_num(); ++ iid ) {
    auto simnode = mFsim.ppi(iid);
    auto val3 = tv.aux_input_val(iid);
    mValArray[simnode->id()] = val3_to_packedval(val3);
  }

  // 2時刻目の正常値の計算を行う．
  _calc_val(mValArray);
}

// @brief 正常値の計算を行う．
void
SimThrFunc::calc_gval(
  const std::vector<TestVector>& tv_list
)
{
  // 1時刻目の入力を設定する．
  // 設定されていないビットはどこか他の設定されているビットをコピーする．
  for ( SizeType iid = 0; iid < mFsim.ppi_num(); ++ iid ) {
    auto simnode = mFsim.ppi(iid);
    auto val = FSIM_INITVAL;
    PackedVal bit = 1UL;
    for ( SizeType pos = 0; pos < PV_BITLEN; ++ pos, bit <<= 1 ) {
      SizeType epos = (pos < tv_list.size()) ? pos : 0;
      auto ival = tv_list[epos].ppi_val(iid);
      bit_set(val, ival, bit);
    }
    mPrevValArray[simnode->id()] = val;
  }

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
  // 設定されていないビットはどこか他の設定されているビットをコピーする．
  for ( SizeType iid = 0; iid < mFsim.input_num(); ++ iid ) {
    auto simnode = mFsim.ppi(iid);
    auto val = FSIM_INITVAL;
    PackedVal bit = 1UL;
    for ( SizeType pos = 0; pos < PV_BITLEN; ++ pos, bit <<= 1 ) {
      SizeType epos = (pos < tv_list.size()) ? pos : 0;
      auto ival = tv_list[epos].aux_input_val(iid);
      bit_set(val, ival, bit);
    }
    mValArray[simnode->id()] = val;
  }

  // 2時刻目の正常値の計算を行う．
  _calc_val(mValArray);
}

// @brief 正常値の計算を行う．
void
SimThrFunc::calc_gval(
    const AssignList& assign_list
)
{
  // 1時刻目の入力を設定する．
  auto val0 = FSIM_INITVAL;
  for ( auto simnode: mFsim.ppi_list() ) {
    mPrevValArray[simnode->id()] = val0;
  }

  for ( auto nv: assign_list ) {
    if ( nv.time() == 0 ) {
      SizeType iid = nv.node().input_id();
      auto simnode = mFsim.ppi(iid);
      mPrevValArray[simnode->id()] = bool_to_packedval(nv.val());
    }
  }

  // 1時刻目の正常値の計算を行う．
  _calc_val(mPrevValArray);

  // DFF の出力の値を入力にコピーする．
  for ( auto i: Range(mFsim.dff_num()) ) {
    auto onode = mFsim.dff_output(i);
    auto inode = mFsim.dff_input(i);
    auto val = mPrevValArray[onode->id()];
    mValArray[inode->id()] = val;
  }

  // 2時刻目の入力を設定する．
  for ( auto simnode: mFsim.input_list() ) {
    mValArray[simnode->id()] = val0;
  }
  for ( auto nv: assign_list ) {
    if ( nv.time() == 1 ) {
      SizeType iid = nv.node().input_id();
      auto simnode = mFsim.ppi(iid);
      mValArray[simnode->id()] = bool_to_packedval(nv.val());
    }
  }

  // 2時刻目の正常値の計算を行う．
  _calc_val(mValArray);
}

// @brief 正常値の計算を行う．
void
SimThrFunc::calc_gvalx(
    const AssignList& assign_list
)
{
  // 1時刻目の入力を設定する．
  auto x_val = FSIM_INITVAL;
  for ( auto simnode: mFsim.logic_list() ) {
    mPrevValArray[simnode->id()] = x_val;
  }

  for ( auto nv: assign_list ) {
    if ( nv.time() == 0 ) {
      auto node_id = nv.node().id();
      auto simnode = mFsim.node(node_id);
      mPrevValArray[simnode->id()] = bool_to_packedval(nv.val());
    }
  }

  // 1時刻目の正常値の計算を行う．
  _calc_val(mPrevValArray);

  // DFF の出力の値を入力にコピーする．
  for ( auto i: Range(mFsim.dff_num()) ) {
    auto onode = mFsim.dff_output(i);
    auto inode = mFsim.dff_input(i);
    auto val = mPrevValArray[onode->id()];
    mValArray[inode->id()] = val;
  }

  // 2時刻目の入力を設定する．
  for ( auto simnode: mFsim.logic_list() ) {
    mValArray[simnode->id()] = x_val;
  }

  for ( auto nv: assign_list ) {
    if ( nv.time() == 1 ) {
      auto node_id = nv.node().id();
      auto simnode = mFsim.node(node_id);
      mValArray[simnode->id()] = bool_to_packedval(nv.val());
    }
  }

  // 2時刻目の正常値の計算を行う．
  _calc_val(mValArray);
}
#endif

// @brief SPSFP 法のシミュレーションを行う．
bool
SimThrFunc::spsfp(
  const SimFault* f,
  DiffBits& dbits
)
{
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
SimThrFunc::sppfp(
  const std::vector<const SimFFR*>& ffr_list,
  FsimResultsRep* res
)
{
  if ( mDebug ) {
    log("sppfp() start");
  }

  auto nffr = ffr_list.size();
  // PV_BITLEN ずつ並列に行う．
  const SimFFR* ffr_buff[PV_BITLEN];
  SizeType bitpos = 0;
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
	  res->add(fid, dbits);
	}
      }
    }
    else {
      // キューにイベントを積む
      PackedVal mask = 1UL << bitpos;
      put_event(root, mask);
      ffr_buff[bitpos] = ffr;
      ++ bitpos;

      if ( bitpos == PV_BITLEN ) {
	// ffr_buff が一杯になった．
	_sppfp_simulation(ffr_buff, bitpos, res);
	bitpos = 0;
      }
    }
  }
  if ( bitpos > 0 ) {
    // ffr_buff にFFRが残っていた．
    _sppfp_simulation(ffr_buff, bitpos, res);
  }

  if ( mDebug ) {
    log("sppfp() end");
  }
}

// @brief sppfp 用のシミュレーションを行う．
void
SimThrFunc::_sppfp_simulation(
  const SimFFR* ffr_buff[],
  SizeType ffr_num,
  FsimResultsRep* res
)
{
  auto dbits_array = simulate();
  auto obs = dbits_array.dbits_union();
  for ( SizeType i = 0; i < ffr_num; ++ i ) {
    PackedVal mask = 1UL << i;
    if ( (obs & mask) == PV_ALL0 ) {
      continue;
    }
    auto ffr = ffr_buff[i];
    auto& fault_list = ffr->fault_list();
    auto dbits = dbits_array.get_slice(i);
    for ( auto f: fault_list ) {
      if ( !f->skip() && (f->obs_mask() & obs) != PV_ALL0 ) {
	auto fid = f->id();
	res->add(fid, dbits);
      }
    }
    mask <<= 1;
  }
}

void
SimThrFunc::ppsfp(
  const SimFFR* ffr,
  const std::vector<FsimResultsRep*>& res_list
)
{
  if ( mDebug ) {
    log("ppsfp() start");
  }

  auto ntv = res_list.size();

  // データを持っているビットを表すビットマスク
  PackedVal bitmask = 0UL;
  for ( SizeType i = 0; i < ntv; ++ i ) {
    bitmask |= (1UL << i);
  }

  // FFR ごとに処理を行う．
  auto ffr_req = foreach_faults(*ffr) & bitmask;
  if ( ffr_req == PV_ALL0 ) {
    // ffr_req が 0 ならその後のシミュレーションは必要ない．
    return;
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
	    res_list[i]->add(fid, dbits);
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
SimThrFunc::log(
  const std::string& msg
)
{
  std::ostringstream buf;
  buf << "[THR#" << mId << "]: " << msg;
  //mSyncObj.log(buf.str());
}

// @brief 個々の故障に FaultProp を適用する．
PackedVal
SimThrFunc::foreach_faults(
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
SimThrFunc::simulate()
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
