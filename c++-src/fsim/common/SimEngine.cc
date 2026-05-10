
/// @file SimEngine.cc
/// @brief SimEngine の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "SimEngine.h"
#include "SimNode.h"
#include "SnFlip.h"
#include "types/TestVector.h"
#include "types/InputVector.h"
#include "types/DffVector.h"


BEGIN_NAMESPACE_DRUID_FSIM

BEGIN_NONAMESPACE
const bool debug = false;
END_NONAMESPACE

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
  // Val3::_X は Val3::_0 とみなす．
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

inline
std::string
val_str(
  FSIM_VALTYPE val
)
{
  std::ostringstream buf;
#if FSIM_VAL2
  for ( SizeType i = 0; i < PV_BITLEN; ++ i ) {
    PackedVal bit = 1UL << i;
    if ( val & bit ) {
      buf << "1";
    }
    else {
      buf << "0";
    }
  }
#elif FSIM_VAL3
  for ( SizeType i = 0; i < PV_BITLEN; ++ i ) {
    PackedVal bit = 1UL << i;
    if ( val.val0() & bit ) {
      buf << "0";
    }
    else if ( val.val1() & bit ) {
      buf << "1";
    }
    else {
      buf << "X";
    }
  }
#endif
  return buf.str();
}

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

//////////////////////////////////////////////////////////////////////
// 故障シミュレーション用のイベントキューを表すクラス
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SimEngine::SimEngine(
  const TpgNetwork& network,
  const TpgFaultList& fault_list,
  const std::vector<SizeType>& ffr_list
) : mInputNum{network.input_num()},
    mOutputNum{network.output_num()},
    mDffNum{network.dff_num()},
    mPPIList(network.ppi_num()),
    mPPOList(network.ppo_num()),
    mSimNodeMap(network.node_num(), nullptr),
    mFlipNodeMap(network.node_num(), nullptr),
    mFaultMap(fault_list.max_fid() + 1, nullptr)
{
  set_network(network, ffr_list);
  set_fault_list(fault_list);
}

// @brief 全ての故障にスキップマークをつける．
void
SimEngine::set_skip_all()
{
  for ( auto& f: mFaultList ) {
    f->set_skip(true);
  }
}

// @brief 故障にスキップマークをつける．
void
SimEngine::set_skip(
  SizeType fid
)
{
  auto fault = mFaultMap[fid];
  if ( fault != nullptr ) {
    fault->set_skip(true);
  }
}

// @brief 全ての故障のスキップマークを消す．
void
SimEngine::clear_skip_all()
{
  for ( auto& f: mFaultList ) {
    f->set_skip(false);
  }
}

// @brief 故障のスキップマークを消す．
void
SimEngine::clear_skip(
  SizeType fid
)
{
  auto fault = mFaultMap[fid];
  if ( fault != nullptr ) {
    fault->set_skip(false);
  }
}

// @brief 故障のスキップマークを得る．
bool
SimEngine::get_skip(
  SizeType fid
) const
{
  auto fault = mFaultMap[fid];
  if ( fault != nullptr ) {
    return fault->skip();
  }
  return false;
}

// @brief SPSFP故障シミュレーションの本体
bool
SimEngine::spsfp(
  SizeType fid
)
{
  auto ff = mFaultMap[fid];
  if ( ff == nullptr ) {
    throw std::invalid_argument{"invalid fault-id"};
  }

  // FFR の根までの伝搬条件を求める．
  auto lobs = _local_prop(ff);

  // obs が 0 ならその後のシミュレーションを行う必要はない．
  if ( lobs == PV_ALL0 ) {
    return false;
  }

  // FFR の根のノードを求める．
  auto root = ff->origin_node()->ffr_root();

  // root からの故障伝搬シミュレーションを行う．
  set_flip_mask(root, PV_ALL1);
  put(root);
  auto gobs = simulate();
  if ( gobs != PV_ALL0 ) {
    return true;
  }
  return false;
}

// @brief SPPFP故障シミュレーションの本体
std::vector<SizeType>
SimEngine::sppfp()
{
  std::vector<SizeType> det_list;

  const SimFFR* ffr_buff[PV_BITLEN];
  auto buff_size = 0;

  // FFR ごとに処理を行う．
  for ( auto& ffr: mFFRArray ) {
    // FFR 内の故障伝搬を行う．
    // 結果は SimFault.mObsMask に保存される．
    // FFR 内の全ての obs マスクを ffr_req に入れる．
    auto ffr_req = _foreach_faults(ffr);
    if ( ffr_req == PV_ALL0 ) {
      // ffr_req が 0 ならその後のシミュレーションを行う必要はない．
      continue;
    }

    auto root = ffr.root();
    if ( root->is_output() ) {
      // 常に観測可能
      _sppfp_sub(ffr, PV_ALL1, det_list);
      continue;
    }

    // バッファに積んでおく
    ffr_buff[buff_size] = &ffr;
    ++ buff_size;
    if ( buff_size == PV_BITLEN ) {
      // バッファが一杯になったのでイベントドリヴンシミュレーションを行う．
      _sppfp_simulation(ffr_buff, buff_size, det_list);
      buff_size = 0;
    }
  }
  if ( buff_size > 0 ) {
    // バッファに要素が残っていた．
    _sppfp_simulation(ffr_buff, buff_size, det_list);
  }

  return det_list;
}

// @brief sppfp 用のシミュレーションを行う．
void
SimEngine::_sppfp_simulation(
  const SimFFR* ffr_buff[],
  SizeType buff_size,
  std::vector<SizeType>& det_list
)
{
  PackedVal bitmask = 1ULL;
  for ( SizeType i = 0; i < buff_size; ++ i, bitmask <<= 1 ) {
    auto& ffr = *ffr_buff[i];
    auto root = ffr.root();
    auto flip = mFlipNodeMap[root->id()];
    flip->set_flip_mask(bitmask);
    put(flip);
  }
  auto obs = simulate1();
  bitmask = 1ULL;
  for ( SizeType i = 0; i < buff_size; ++ i, bitmask <<= 1 ) {
    auto& ffr = *ffr_buff[i];
    auto root = ffr.root();
    auto flip = mFlipNodeMap[root->id()];
    flip->set_flip_mask(PV_ALL0);
    if ( (obs & bitmask) != PV_ALL0 ) {
      _sppfp_sub(ffr, bitmask, det_list);
    }
  }
}

// @brief PPSFP故障シミュレーションの本体
std::vector<std::vector<SizeType>>
SimEngine::ppsfp(
  SizeType tv_num
)
{
  // データを持っているビットを表すビットマスク
  PackedVal bitmask = 0UL;
  for ( SizeType i = 0; i < tv_num; ++ i ) {
    bitmask |= (1UL << i);
  }

  // 結果を格納するオブジェクトのリスト
  std::vector<std::vector<SizeType>> det_list_array(tv_num);

  // FFR ごとに処理を行う．
  for ( auto& ffr: mFFRArray ) {
    // FFR 内の故障伝搬を行う．
    // 結果は SimFault::mObsMask に保存される．
    // FFR 内の全ての obs マスクを ffr_req に入れる．
    auto ffr_req = _foreach_faults(ffr) & bitmask;

    // ffr_req が 0 ならその後のシミュレーションを行う必要はない．
    if ( ffr_req == PV_ALL0 ) {
      continue;
    }

    // FFR の出力の故障伝搬を行う．
    auto root = ffr.root();
    PackedVal gobs = PV_ALL1;
    if ( !root->is_output() ) {
      // root が外部出力なら常に観測可能
      auto old_val = root->val();
      auto new_val = old_val ^ ffr_req;
      root->set_val(new_val);
      add_to_clear_list(root, old_val);
      put_fanouts(root);
      gobs = simulate1();
      if ( gobs == PV_ALL0 ) {
	continue;
      }
    }

    // FFR 内の故障伝搬を行う．
    for ( auto ff: ffr.fault_list() ) {
      if ( ff->skip() ) {
	continue;
      }
      auto obs = ff->obs_mask() & gobs;
      if ( obs != PV_ALL0 ) {
	// 検出された．
	auto fid = ff->id();
	for ( SizeType i = 0; i < tv_num; ++ i ) {
	  PackedVal bitmask = 1UL << i;
	  if ( (obs & bitmask) != PV_ALL0 ) {
	    det_list_array[i].push_back(fid);
	  }
	}
      }
    }
  }

  return det_list_array;
}

// @brief SPSFP故障シミュレーションの本体
DiffBits
SimEngine::spsfp2(
  SizeType fid
)
{
  auto ff = mFaultMap[fid];
  if ( ff == nullptr ) {
    throw std::invalid_argument{"invalid fault-id"};
  }

  // FFR の根までの伝搬条件を求める．
  auto obs = _local_prop(ff);

  // obs が 0 ならその後のシミュレーションを行う必要はない．
  if ( obs == PV_ALL0 ) {
    return DiffBits();
  }

  // FFR の根のノードを求める．
  auto root = ff->origin_node()->ffr_root();

  // root からの故障伝搬シミュレーションを行う．
  set_flip_mask(root, PV_ALL1);
  put(root);
  auto dbits_array = simulate2();
  if ( dbits_array.dbits_union() != PV_ALL0 ) {
    return dbits_array.get_slice(0);
  }
  return DiffBits();
}

// @brief SPPFP故障シミュレーションの本体
FsimResultsRep*
SimEngine::sppfp2()
{
  mRes = new FsimResultsRep;

  const SimFFR* ffr_buff[PV_BITLEN];
  auto bitpos = 0;
  // FFR ごとに処理を行う．
  for ( auto& ffr: mFFRArray ) {
    // FFR 内の故障伝搬を行う．
    // 結果は SimFault.mObsMask に保存される．
    // FFR 内の全ての obs マスクを ffr_req に入れる．
    auto ffr_req = _foreach_faults(ffr);
    if ( ffr_req == PV_ALL0 ) {
      // ffr_req が 0 ならその後のシミュレーションを行う必要はない．
      continue;
    }

    auto root = ffr.root();
    if ( root->is_output() ) {
      // 常にこの出力のみで観測可能
      DiffBits dbits;
      dbits.add_output(root->output_id());
      _sppfp2_sub(ffr, dbits);
    }
    else {
      // キューに積んでおく
      PackedVal bitmask = 1ULL << bitpos;
      set_flip_mask(root, bitmask);
      put(root);
      ffr_buff[bitpos] = &ffr;
      ++ bitpos;
      if ( bitpos == PV_BITLEN ) {
	_sppfp2_simulation(ffr_buff, bitpos);
	bitpos = 0;
      }
    }
  }
  if ( bitpos > 0 ) {
    _sppfp2_simulation(ffr_buff, bitpos);
  }

  return mRes;
}

// @brief sppfp 用のシミュレーションを行う．
void
SimEngine::_sppfp2_simulation(
  const SimFFR* ffr_buff[],
  SizeType ffr_num
)
{
  auto dbits_array = simulate2();
  auto obs = dbits_array.dbits_union();
  PackedVal mask = 1ULL;
  for ( auto i = 0; i < ffr_num; ++ i, mask <<= 1 ) {
    if ( obs & mask ) {
      auto& ffr = *ffr_buff[i];
      auto dbits = dbits_array.get_slice(i);
      dbits.sort();
      _sppfp2_sub(ffr, dbits);
    }
  }
}

// @brief PPSFP故障シミュレーションの本体
std::vector<FsimResultsRep*>
SimEngine::ppsfp2(
  SizeType tv_num
)
{
  std::vector<FsimResultsRep*> res_list(tv_num);
  for ( SizeType i = 0; i < tv_num; ++ i ) {
    auto res = new FsimResultsRep;
    res_list[i] = res;
  }

  // データを持っているビットを表すビットマスク
  PackedVal bitmask = 0UL;
  for ( SizeType i = 0; i < tv_num; ++ i ) {
    bitmask |= (1UL << i);
  }

  // FFR ごとに処理を行う．
  for ( auto& ffr: mFFRArray ) {
    // FFR 内の故障伝搬を行う．
    // 結果は SimFault::mObsMask に保存される．
    // FFR 内の全ての obs マスクを ffr_req に入れる．
    auto ffr_req = _foreach_faults(ffr) & bitmask;

    // ffr_req が 0 ならその後のシミュレーションを行う必要はない．
    if ( ffr_req == PV_ALL0 ) {
      continue;
    }

    // FFR の出力の故障伝搬を行う．
    auto root = ffr.root();
    set_flip_mask(root, ffr_req);
    put(root);
    auto dbits_array = simulate2();
    auto gobs = dbits_array.dbits_union();
    if ( gobs != PV_ALL0 ) {
      // FFR 内の故障伝搬を行う．
      for ( auto ff: ffr.fault_list() ) {
	if ( ff->skip() ) {
	  continue;
	}
	if ( (ff->obs_mask() & gobs) != PV_ALL0 ) {
	  // 検出された．
	  auto fid = ff->id();
	  auto dbits_array1 = dbits_array.masking(ff->obs_mask());
	  for ( SizeType i = 0; i < tv_num; ++ i ) {
	    auto dbits = dbits_array1.get_slice(i);
	    if ( dbits.elem_num() > 0 ) {
	      res_list[i]->add(fid, dbits);
	    }
	  }
	}
      }
    }
  }
  return res_list;
}

#if FSIM_COMBI
// @brief 値の計算を行う．
void
SimEngine::calc_val(
  const TestVector& tv
)
{
  // 入力の設定を行う．
  for ( SizeType iid = 0; iid < ppi_num(); ++ iid ) {
    auto node = ppi(iid);
    auto val3 = tv.ppi_val(iid);
    node->set_val(val3_to_packedval(val3));
  }

  // 正常値の計算を行う．
  _calc_val();
}

// @brief 値の計算を行う．
void
SimEngine::calc_val(
  const std::vector<TestVector>& tv_list
)
{
  // 設定されていないビットはどこか他の設定されているビットをコピーする．
  auto x_val = FSIM_INITVAL;
  for ( SizeType iid = 0; iid < ppi_num(); ++ iid ) {
    auto simnode = ppi(iid);
    auto val = x_val;
    PackedVal bit = 1UL;
    for ( SizeType pos = 0; pos < PV_BITLEN; ++ pos, bit <<= 1 ) {
      SizeType epos = (pos < tv_list.size()) ? pos : 0;
      auto ival = tv_list[epos].ppi_val(iid);
      bit_set(val, ival, bit);
    }
    simnode->set_val(val);
  }

  // 正常値の計算を行う．
  _calc_val();
}

// @brief 値の計算を行う．
void
SimEngine::calc_val(
  const AssignList& assign_list
)
{
  // デフォルト値で初期化する．
  auto val0 = FSIM_INITVAL;
  for ( auto simnode: ppi_list() ) {
    simnode->set_val(val0);
  }

  for ( auto nv: assign_list ) {
    if ( nv.time() != 1 ) {
      throw std::logic_error{"nv.time() != 1"};
    }
    auto iid = nv.node().input_id();
    auto simnode = ppi(iid);
    auto val = nv.val();
    simnode->set_val(bool_to_packedval(nv.val()));
  }

  // 正常値の計算を行う．
  _calc_val();
}
#endif

#if FSIM_BSIDE
// @brief 値の計算を行う．
void
SimEngine::calc_val(
  const TestVector& tv
)
{
  // 1時刻目の入力を設定する．
  for ( SizeType iid = 0; iid < ppi_num(); ++ iid ) {
    auto simnode = ppi(iid);
    auto val3 = tv.ppi_val(iid);
    simnode->set_val(val3_to_packedval(val3));
  }

  // 1時刻目の正常値の計算を行う．
  _calc_val();

  // 1時刻シフトする．
  for ( auto& node: mNodeArray ) {
    node->shift_val();
  }

  // DFF の出力の値を入力にコピーする．
  for ( auto i: Range(mDffNum) ) {
    auto onode = mPPOList[i + mOutputNum];
    auto inode = mPPIList[i + mInputNum];
    inode->set_val(onode->val());
  }

  // 2時刻目の入力を設定する．
  for ( SizeType iid = 0; iid < input_num(); ++ iid ) {
    auto simnode = ppi(iid);
    auto val3 = tv.aux_input_val(iid);
    simnode->set_val(val3_to_packedval(val3));
  }

  // 2時刻目の正常値の計算を行う．
  _calc_val();
}

// @brief 値の計算を行う．
void
SimEngine::calc_val(
  const std::vector<TestVector>& tv_list
)
{
  // 1時刻目の入力を設定する．
  // 設定されていないビットはどこか他の設定されているビットをコピーする．
  for ( SizeType iid = 0; iid < ppi_num(); ++ iid ) {
    auto simnode = ppi(iid);
    auto val = FSIM_INITVAL;
    PackedVal bit = 1UL;
    for ( SizeType pos = 0; pos < PV_BITLEN; ++ pos, bit <<= 1 ) {
      auto epos = (pos < tv_list.size()) ? pos : 0;
      auto ival = tv_list[epos].ppi_val(iid);
      bit_set(val, ival, bit);
    }
    simnode->set_val(val);
  }

  // 1時刻目の正常値の計算を行う．
  _calc_val();

  // 1時刻シフトする．
  for ( auto& node: mNodeArray ) {
    node->shift_val();
  }

  // DFF の出力の値を入力にコピーする．
  for ( auto i: Range(mDffNum) ) {
    auto onode = mPPOList[i + mOutputNum];
    auto inode = mPPIList[i + mInputNum];
    inode->set_val(onode->val());
  }

  // 2時刻目の入力を設定する．
  // 設定されていないビットはどこか他の設定されているビットをコピーする．
  for ( SizeType iid = 0; iid < input_num(); ++ iid ) {
    auto simnode = ppi(iid);
    auto val = FSIM_INITVAL;
    PackedVal bit = 1UL;
    for ( SizeType pos = 0; pos < PV_BITLEN; ++ pos, bit <<= 1 ) {
      SizeType epos = (pos < tv_list.size()) ? pos : 0;
      auto ival = tv_list[epos].aux_input_val(iid);
      bit_set(val, ival, bit);
    }
    simnode->set_val(val);
  }

  // 2時刻目の正常値の計算を行う．
  _calc_val();
}

// @brief 値の計算を行う．
void
SimEngine::calc_val(
  const AssignList& assign_list
)
{
  auto val0 = FSIM_INITVAL;

  // 1時刻目の入力を設定する．
  for ( auto simnode: ppi_list() ) {
    simnode->set_val(val0);
  }

  for ( auto nv: assign_list ) {
    if ( nv.time() == 0 ) {
      auto iid = nv.node().input_id();
      auto simnode = ppi(iid);
      simnode->set_val(bool_to_packedval(nv.val()));
    }
  }

  // 1時刻目の正常値の計算を行う．
  _calc_val();

  // 1時刻シフトする．
  for ( auto& node: mNodeArray ) {
    node->shift_val();
  }

  // DFF の出力の値を入力にコピーする．
  for ( auto i: Range(mDffNum) ) {
    auto onode = mPPOList[i + mOutputNum];
    auto inode = mPPIList[i + mInputNum];
    inode->set_val(onode->val());
  }

  // 2時刻目の入力を設定する．
  for ( auto simnode: input_list() ) {
    simnode->set_val(val0);
  }

  for ( auto nv: assign_list ) {
    if ( nv.time() == 1 ) {
      auto iid = nv.node().input_id();
      auto simnode = ppi(iid);
      simnode->set_val(bool_to_packedval(nv.val()));
    }
  }

  // 2時刻目の正常値の計算を行う．
  _calc_val();
}
#endif

// @brief イベントドリブンシミュレーションを行う．
PackedVal
SimEngine::simulate()
{
  // 検出結果
  PackedVal det = PV_ALL0;
  for ( ; ; ) {
    auto node = get();
    // イベントが残っていなければ終わる．
    if ( node == nullptr ) break;

    auto old_val = node->val();
    auto new_val = node->calc_val();
    // 反転イベントを考慮する．
    auto flip_mask = mFlipMaskArray[node->id()];
    new_val ^= flip_mask;
    mFlipMaskArray[node->id()] = PV_ALL0;
    if ( new_val != old_val ) {
      node->set_val(new_val);
      add_to_clear_list(node, old_val);
      if ( node->is_output() ) {
	det |= diff(new_val, old_val);
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

  return det;
}

// @brief イベントドリブンシミュレーションを行う．
PackedVal
SimEngine::simulate1()
{
  // 検出結果
  PackedVal det = PV_ALL0;
  for ( ; ; ) {
    auto node = get();
    // イベントが残っていなければ終わる．
    if ( node == nullptr ) break;

    auto old_val = node->val();
    auto new_val = node->calc_val();
    if ( new_val != old_val ) {
      node->set_val(new_val);
      add_to_clear_list(node, old_val);
      if ( node->is_output() ) {
	det |= diff(new_val, old_val);
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

  return det;
}

// @brief イベントドリブンシミュレーションを行う．
DiffBitsArray
SimEngine::simulate2()
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
      new_val = _get_init(node->id());
    }
    else {
      new_val = node->calc_val();
    }
    // 反転イベントの考慮
    auto flip_mask = mFlipMaskArray[node->id()];
    new_val ^= flip_mask;
    mFlipMaskArray[node->id()] = PV_ALL0;
    node->set_val(new_val);
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

  return dbits_array;
}

#if FSIM_BSIDE
SizeType
SimEngine::calc_wsa(
  const TestVector& tv,
  bool weighted
)
{
#if 0
  set_state(tv.input_vector(), tv.dff_vector());
  return calc_wsa(tv.aux_input_vector(), weighted);
#else
#warning "TestVector を用いたバージョン未完成"
  return 0;
#endif
}

// @brief 1クロック分のシミュレーションを行い，遷移回数を数える．
SizeType
SimEngine::calc_wsa(
  const InputVector& i_vect,
  bool weighted
)
{
  SizeType i = 0;
  for ( auto simnode: input_list() ) {
    auto val3 = i_vect.val(i);
    simnode->set_val(val3_to_packedval(val3));
  }

  // 各信号線の値を計算する．
  _calc_val();

  // 遷移回数を数える．
  SizeType wsa = 0;
  for ( auto& node: mNodeArray ) {
    wsa += _calc_wsa(node.get(), weighted);
  }

  // 1時刻シフトする．
  for ( auto& node: mNodeArray ) {
    node->shift_val();
  }

  // DFF の出力の値を入力にコピーする．
  for ( auto i: Range(mDffNum) ) {
    auto onode = mPPOList[i + mOutputNum];
    auto inode = mPPIList[i + mInputNum];
    inode->set_val(onode->val());
  }

  return wsa;
}

// @brief 状態を設定する．
void
SimEngine::set_state(
  const InputVector& i_vect,
  const DffVector& f_vect
)
{
  SizeType i = 0;
  for ( auto simnode: input_list() ) {
    auto val3 = i_vect.val(i);
    simnode->set_val(val3_to_packedval(val3));
    ++ i;
  }

  i = 0;
  for ( auto simnode: dff_output_list() ) {
    auto val3 = f_vect.val(i);
    simnode->set_val(val3_to_packedval(val3));
    ++ i;
  }

  // 各信号線の値を計算する．
  _calc_val();

  // 1時刻シフトする．
  for ( auto& node: mNodeArray ) {
    node->shift_val();
  }

  // DFF の出力の値を入力にコピーする．
  for ( auto i: Range(mDffNum) ) {
    auto onode = mPPOList[i + mOutputNum];
    auto inode = mPPIList[i + mInputNum];
    inode->set_val(onode->val());
  }
}

// @brief 状態を取得する．
void
SimEngine::get_state(
  InputVector& i_vect,
  DffVector& f_vect
)
{
  SizeType i = 0;
  for ( auto simnode: input_list() ) {
    auto val = packedval_to_val3(simnode->val());
    i_vect.set_val(i, val);
    ++ i;
  }

  i = 0;
  for ( auto simnode: dff_output_list() ) {
    auto val = packedval_to_val3(simnode->val());
    f_vect.set_val(i, val);
    ++ i;
  }
}

// @brief ノードの出力の(重み付き)信号遷移回数を求める．
SizeType
SimEngine::_calc_wsa(
  SimNode* node,
  bool weighted
)
{
  SizeType wsa = 0;
  if ( node->prev_val() != node->val() ) {
    wsa = 1;
    if ( weighted ) {
      wsa += node->fanout_num();
    }
  }
  return wsa;
}
#endif

// @brief ネットワークの構造を設定する．
void
SimEngine::set_network(
  const TpgNetwork& network,
  const std::vector<SizeType>& ffr_list
)
{
  auto nn = network.node_num();
  auto ni = network.ppi_num();
  auto no = network.ppo_num();

  if ( ni != mInputNum + mDffNum ) {
    throw std::logic_error{"ni != mInputNum + mDffNum"};
  }
  if ( no != mOutputNum + mDffNum ) {
    throw std::logic_error{"no != mOutputNum + mDffNum"};
  }

  // 反転イベントを挿入するノードに印を付ける．
  std::vector<bool> flip_mark(nn, false);
  for ( auto ffr_id: ffr_list ) {
    auto ffr = network.ffr(ffr_id);
    auto tpgroot = ffr.root();
    flip_mark[tpgroot.id()] = true;
  }

  // SimNode を作る．
  for ( SizeType id = 0; id < nn; ++ id ) {
    auto tpgnode = network.node(id);
    SimNode* node = nullptr;

    if ( tpgnode.is_ppi() ) {
      // 外部入力に対応する SimNode の生成
      node = make_input();
      mPPIList[tpgnode.input_id()] = node;
    }
    else if ( tpgnode.is_ppo() ) {
      // 外部出力に対応する SimNode の生成
      auto tpginode = tpgnode.fanin(0);
      auto inode = mSimNodeMap[tpginode.id()];
      auto oid = tpgnode.output_id();
      node = make_output(inode, oid);
      mPPOList[oid] = node;
    }
    else if ( tpgnode.is_logic() ) {
      // 論理ノードに対する SimNode の作成
      auto ni = tpgnode.fanin_num();

      // ファンインに対応する SimNode を探す．
      std::vector<SimNode*> inputs;
      inputs.reserve(ni);
      for ( auto itpgnode: tpgnode.fanin_list() ) {
	auto inode = mSimNodeMap[itpgnode.id()];
	if ( inode == nullptr ) {
	  throw std::logic_error{"mSimNodeMap[itpgnode->id()] == nullptr"};
	}

	inputs.push_back(inode);
      }

      // 出力の論理を表す SimNode を作る．
      auto type = tpgnode.gate_type();
      node = make_gate(type, inputs);
    }

    if ( flip_mark[id] ) {
      // 反転イベント用のノードを挿入する．
      node = make_flip(node);
    }

    // 対応表に登録しておく．
    mSimNodeMap[id] = node;
  }

  // 各ノードのファンアウトリストの設定
  auto node_num = mNodeArray.size();
  {
    std::vector<std::vector<SimNode*>> fanout_lists(node_num);
    std::vector<int> ipos(node_num);
    for ( auto& node: mNodeArray ) {
      auto ni = node->fanin_num();
      for ( auto i: Range(0, ni) ) {
	auto inode = node->fanin(i);
	fanout_lists[inode->id()].push_back(node.get());
	ipos[inode->id()] = i;
      }
    }
    for ( auto i: Range(node_num) ) {
      auto& node = mNodeArray[i];
      if ( !node->is_output() ) {
	node->set_fanout_list(fanout_lists[i], ipos[i]);
      }
    }
  }

  // FFR の設定
  mFFRMap.resize(mNodeArray.size());
  if ( ffr_list.empty() ) {
    // すべての FFR を対象にする．
    auto ffr_num = network.ffr_num();
    mFFRArray.resize(ffr_num);
    for ( auto tpgffr: network.ffr_list() ) {
      auto tpgroot = tpgffr.root();
      auto node = mSimNodeMap[tpgroot.id()];
      auto ffr = &mFFRArray[tpgffr.id()];
      node->set_ffr_root();
      ffr->set(tpgffr.id(), node);
      for ( auto tpgnode: tpgffr.node_list() ) {
	auto node = mSimNodeMap[tpgnode.id()];
	mFFRMap[tpgnode.id()] = ffr;
      }
    }
  }
  else {
    // ffr_list のみを対象にする．
    auto ffr_num = ffr_list.size();
    mFFRArray.resize(ffr_num);
    SizeType next_id = 0;
    for ( auto ffr_id: ffr_list ) {
      auto tpgffr = network.ffr(ffr_id);
      auto tpgroot = tpgffr.root();
      auto node = mSimNodeMap[tpgroot.id()];
      auto ffr = &mFFRArray[next_id];
      node->set_ffr_root();
      ffr->set(next_id, node);
      for ( auto tpgnode: tpgffr.node_list() ) {
	auto node = mSimNodeMap[tpgnode.id()];
	mFFRMap[tpgnode.id()] = ffr;
      }
      ++ next_id;
    }
  }

  // 最大レベルを求め，イベントキューを初期化する．
  auto max_level = 0;
  for ( auto inode: mPPOList ) {
    if ( max_level < inode->level() ) {
      max_level = inode->level();
    }
  }
  mArray.resize(max_level + 1, nullptr);
  mClearArray.reserve(node_num);
  mFlipMaskArray.resize(node_num, PV_ALL0);

  mCurLevel = 0;
  mNum = 0;

  for ( auto& node: mNodeArray ) {
    std::cout << "Node#" << node->id()
	      << ": ";
    node->dump(std::cout);
  }
}

// @brief 故障を設定する．
void
SimEngine::set_fault_list(
  const TpgFaultList& fault_list
)
{
  // 対象の故障数を数える．
  SizeType nf = 0;
  for ( auto fault: fault_list ) {
    auto tpgnode = fault.origin_node();
    auto ffr = mFFRMap[tpgnode.id()];
    if ( ffr != nullptr ) {
      ++ nf;
    }
  }

  mFaultList.reserve(nf);
  for ( auto fault: fault_list ) {
    auto tpgnode = fault.origin_node();
    auto ffr = mFFRMap[tpgnode.id()];
    if ( ffr == nullptr ) {
      continue;
    }
    auto simnode = mSimNodeMap[tpgnode.id()];
    auto sim_f = new SimFault(fault, simnode, *this);
    mFaultList.push_back(std::unique_ptr<SimFault>{sim_f});
    mFaultMap[fault.id()] = sim_f;
    sim_f->set_skip(false);
    ffr->add_fault(sim_f);
  }
}

// @brief 外部入力ノードを作る．
SimNode*
SimEngine::make_input()
{
  auto id = mNodeArray.size();
  auto node = SimNode::new_input(id);
  mNodeArray.push_back(std::unique_ptr<SimNode>{node});
  return node;
}

// @brief 単純な logic ノードを作る．
SimNode*
SimEngine::make_gate(
  PrimType type,
  const std::vector<SimNode*>& inputs
)
{
  auto id = mNodeArray.size();
  auto node = SimNode::new_gate(id, type, inputs);
  mNodeArray.push_back(std::unique_ptr<SimNode>{node});
  mLogicArray.push_back(node);
  return node;
}

// @brief 反転イベントノードを作る．
SimNode*
SimEngine::make_flip(
  SimNode* node
)
{
  auto id = mNodeArray.size();
  auto flip = new SnFlip(id, node);
  mNodeArray.push_back(std::unique_ptr<SimNode>{flip});
  mFlipNodeMap[id] = flip;
  return flip;
}

END_NAMESPACE_DRUID_FSIM
