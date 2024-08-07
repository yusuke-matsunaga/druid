﻿
/// @file FsimX.cc
/// @brief FsimX の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2017, 2018, 2022, 2024 Yusuke Matsunaga
/// All rights reserved.

#include "FsimX.h"

#include "TpgNetwork.h"
#include "TpgNode.h"
#include "TpgFault.h"

#include "TestVector.h"
#include "InputVector.h"
#include "DffVector.h"
#include "AssignList.h"

#include "SimNode.h"
#include "SimFFR.h"

#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID_FSIM

BEGIN_NONAMESPACE

// 初期値を求める．
inline
FSIM_VALTYPE
init_val()
{
#if FSIM_VAL2
  // デフォルトで 0 にする．
  return PV_ALL0;
#elif FSIM_VAL3
  // デフォルトで X にする．
  return PackedVal3(PV_ALL0, PV_ALL0);
#endif
}

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

END_NONAMESPACE

std::unique_ptr<FsimImpl>
new_Fsim(
  const TpgNetwork& network,
  const vector<const TpgFault*>& fault_list
)
{
  return static_cast<std::unique_ptr<FsimImpl>>(new FSIM_CLASSNAME{network, fault_list});
}


//////////////////////////////////////////////////////////////////////
// FsimX
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
FSIM_CLASSNAME::FSIM_CLASSNAME(
  const TpgNetwork& network,
  const vector<const TpgFault*>& fault_list
)
{
  set_network(network);
  set_fault_list(fault_list);
}

// @brief デストラクタ
FSIM_CLASSNAME::~FSIM_CLASSNAME()
{
}

// @brief ネットワークをセットする関数
void
FSIM_CLASSNAME::set_network(
  const TpgNetwork& network
)
{
  mInputNum = network.input_num();
  mOutputNum = network.output_num();
  mDffNum = network.dff_num();

  auto nn = network.node_num();
  auto ni = network.ppi_num();
  auto no = network.ppo_num();

  ASSERT_COND( ni == mInputNum + mDffNum );
  ASSERT_COND( no == mOutputNum + mDffNum );

  // 対応付けを行うマップの初期化
  mSimNodeMap.resize(nn);

  mPPIList.clear();
  mPPIList.resize(ni);
  mPPOList.clear();
  mPPOList.resize(no);

  for ( auto tpgnode: network.node_list() ) {
    SimNode* node = nullptr;

    if ( tpgnode->is_ppi() ) {
      // 外部入力に対応する SimNode の生成
      node = make_input();
      mPPIList[tpgnode->input_id()] = node;
    }
    else if ( tpgnode->is_ppo() ) {
      // 外部出力に対応する SimNode の生成
      auto inode = mSimNodeMap[tpgnode->fanin(0)->id()];
      auto oid = tpgnode->output_id();
      node = make_output(inode, oid);
      mPPOList[oid] = node;
    }
    else if ( tpgnode->is_logic() ) {
      // 論理ノードに対する SimNode の作成
      auto ni = tpgnode->fanin_num();

      // ファンインに対応する SimNode を探す．
      vector<SimNode*> inputs;
      inputs.reserve(ni);
      for ( auto itpgnode: tpgnode->fanin_list() ) {
	auto inode = mSimNodeMap[itpgnode->id()];
	ASSERT_COND( inode != nullptr );

	inputs.push_back(inode);
      }

      // 出力の論理を表す SimNode を作る．
      auto type = tpgnode->gate_type();
      node = make_gate(type, inputs);
    }
    // 対応表に登録しておく．
    mSimNodeMap[tpgnode->id()] = node;
  }

  // 各ノードのファンアウトリストの設定
  auto node_num = mNodeArray.size();
  {
    vector<vector<SimNode*>> fanout_lists(node_num);
    vector<int> ipos(node_num);
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
  auto ffr_num = 0;
  for ( auto& node: mNodeArray ) {
    if ( node->is_output() || node->fanout_num() != 1 ) {
      ++ ffr_num;
    }
  }

  mFFRArray.clear();
  mFFRArray.resize(ffr_num);
  mFFRMap.resize(mNodeArray.size());
  ffr_num = 0;
  for ( int i = node_num; -- i >= 0; ) {
    auto& node = mNodeArray[i];
    if ( node->is_output() || node->fanout_num() != 1 ) {
      auto ffr = &mFFRArray[ffr_num];
      node->set_ffr_root();
      mFFRMap[node->id()] = ffr;
      ffr->set_root(node.get());
      ++ ffr_num;
    }
    else {
      auto fo_node = node->fanout_top();
      auto ffr = mFFRMap[fo_node->id()];
      mFFRMap[node->id()] = ffr;
    }
  }

  // 最大レベルを求め，イベントキューを初期化する．
  auto max_level = 0;
  for ( auto inode: mPPOList ) {
    if ( max_level < inode->level() ) {
      max_level = inode->level();
    }
  }
  mEventQ.init(max_level, ppo_num(), mNodeArray.size());
}

// @brief 対象の故障をセットする．
void
FSIM_CLASSNAME::set_fault_list(
  const vector<const TpgFault*>& fault_list
)
{
  SizeType nf = fault_list.size();
  SizeType max_fid = 0;
  for ( auto fault: fault_list ) {
    max_fid = std::max(max_fid, fault->id());
  }
  ++ max_fid;
  mFaultList.clear();
  mFaultMap.clear();
  mDetFaultArray.clear();
  mFaultList.reserve(nf);
  mFaultMap.resize(max_fid, nullptr);
  mDetFaultArray.reserve(nf);

  for ( auto fault: fault_list ) {
    auto tpgnode = fault->origin_node();
    auto simnode = mSimNodeMap[tpgnode->id()];
    auto sim_f = new SimFault{fault, simnode, mSimNodeMap};
    mFaultList.push_back(unique_ptr<SimFault>{sim_f});
    mFaultMap[fault->id()] = sim_f;
    sim_f->set_skip(false);
    auto ffr = mFFRMap[simnode->id()];
    ffr->add_fault(sim_f);
  }
}

// @brief 全ての故障にスキップマークをつける．
void
FSIM_CLASSNAME::set_skip_all()
{
  for ( auto& f: mFaultList ) {
    f->set_skip(true);
  }
}

// @brief 故障にスキップマークをつける．
void
FSIM_CLASSNAME::set_skip(
  const TpgFault* f
)
{
  mFaultMap[f->id()]->set_skip(true);
}

// @brief 全ての故障のスキップマークを消す．
void
FSIM_CLASSNAME::clear_skip_all()
{
  for ( auto& f: mFaultList ) {
    f->set_skip(false);
  }
}

// @brief 故障のスキップマークを消す．
void
FSIM_CLASSNAME::clear_skip(
  const TpgFault* f
)
{
  mFaultMap[f->id()]->set_skip(false);
}

// @brief 故障のスキップマークを得る．
bool
FSIM_CLASSNAME::get_skip(
  const TpgFault* f
) const
{
  return mFaultMap[f->id()]->skip();
}

// @brief SPSFP故障シミュレーションを行う．
bool
FSIM_CLASSNAME::spsfp(
  const TestVector& tv,
  const TpgFault* f,
  DiffBits& dbits
)
{
  // 正常値の計算を行う．
  _calc_gval(tv);

  // 故障伝搬を行う．
  return _spsfp(f, dbits);
}

// @brief SPSFP故障シミュレーションを行う．
bool
FSIM_CLASSNAME::spsfp(
  const AssignList& assign_list,
  const TpgFault* f,
  DiffBits& dbits
)
{
  // 正常値の計算を行う．
  _calc_gval(assign_list);

  // 故障伝搬を行う．
  return _spsfp(f, dbits);
}

// @brief SPSFP故障シミュレーションを行う．
bool
FSIM_CLASSNAME::xspsfp(
  const AssignList& assign_list,
  const TpgFault* f,
  DiffBits& dbits
)
{
  // dbits を初期化しておく．
  dbits.clear();

  // 正常値の計算を行う．
  _calc_gval2(assign_list);

  // 故障伝搬を行う．
  return _spsfp(f, dbits);
}

// @brief SPSFP故障シミュレーションの本体
bool
FSIM_CLASSNAME::_spsfp(
  const TpgFault* f,
  DiffBits& dbits
)
{
  // dbits を初期化しておく．
  dbits.clear();

  auto ff = mFaultMap[f->id()];

  // FFR の根までの伝搬条件を求める．
  auto obs = _local_prop(ff);

  // obs が 0 ならその後のシミュレーションを行う必要はない．
  if ( obs == PV_ALL0 ) {
    return false;
  }

  // FFR の根のノードを求める．
  auto root = ff->origin_node()->ffr_root();

  // root からの故障伝搬シミュレーションを行う．
  auto dbits_array = _global_prop(root, PV_ALL1);
  if ( dbits_array.dbits_union() != PV_ALL0 ) {
    dbits = dbits_array.get_slice(0);
    return true;
  }
  return false;
}

// @brief ひとつのパタンで故障シミュレーションを行う．
void
FSIM_CLASSNAME::sppfp(
  const TestVector& tv,
  cbtype1 callback
)
{
  // 正常値の計算を行う．
  _calc_gval(tv);

  // 故障伝搬を行う．
  _sppfp(callback);
}

// @brief ひとつのパタンで故障シミュレーションを行う．
void
FSIM_CLASSNAME::sppfp(
  const AssignList& assign_list,
  cbtype1 callback
)
{
  // 正常値の計算を行う．
  _calc_gval(assign_list);

  // 故障伝搬を行う．
  _sppfp(callback);
}

// @brief ひとつのパタンで故障シミュレーションを行う．
void
FSIM_CLASSNAME::xsppfp(
  const AssignList& assign_list,
  cbtype1 callback
)
{
  // 正常値の計算を行う．
  _calc_gval2(assign_list);

  // 故障伝搬を行う．
  _sppfp(callback);
}

// @brief SPPFP故障シミュレーションの本体
void
FSIM_CLASSNAME::_sppfp(
  cbtype1 callback
)
{
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
      _sppfp_apply_callback(ffr, dbits, callback);
    }
    else {
      // キューに積んでおく
      PackedVal bitmask = 1ULL << bitpos;
      mEventQ.put_event(root, bitmask);
      ffr_buff[bitpos] = &ffr;
      ++ bitpos;

      if ( bitpos == PV_BITLEN ) {
	_sppfp_simulation(ffr_buff, bitpos, callback);
	bitpos = 0;
      }
    }
  }
  if ( bitpos > 0 ) {
    _sppfp_simulation(ffr_buff, bitpos, callback);
  }
}

// @brief sppfp 用のシミュレーションを行う．
void
FSIM_CLASSNAME::_sppfp_simulation(
  const SimFFR* ffr_buff[],
  SizeType ffr_num,
  cbtype1 callback
)
{
  auto dbits_array = mEventQ.simulate();
  auto obs = dbits_array.dbits_union();
  PackedVal mask = 1ULL;
  for ( auto i = 0; i < ffr_num; ++ i, mask <<= 1 ) {
    if ( obs & mask ) {
      auto& ffr = *ffr_buff[i];
      auto dbits = dbits_array.get_slice(i);
      dbits.sort();
      _sppfp_apply_callback(ffr, dbits, callback);
    }
  }
}

// @brief 複数のパタンで故障シミュレーションを行う．
void
FSIM_CLASSNAME::ppsfp(
  const vector<TestVector>& tv_list,
  cbtype2 callback
)
{
  // 正常値の計算を行う．
  _calc_gval(tv_list);

  // データを持っているビットを表すビットマスク
  PackedVal bitmask = 0UL;
  for ( SizeType i = 0; i < tv_list.size(); ++ i ) {
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
    auto dbits_array = _global_prop(ffr.root(), ffr_req);
    auto gobs = dbits_array.dbits_union();
    if ( gobs != PV_ALL0 ) {
      // FFR 内の故障伝搬を行う．
      for ( auto ff: ffr.fault_list() ) {
	if ( ff->skip() ) {
	  continue;
	}
	if ( (ff->obs_mask() & gobs) != PV_ALL0 ) {
	  // 検出された．
	  auto tpg_f = ff->tpg_fault();
	  auto dbits = dbits_array.masking(ff->obs_mask());
	  dbits.sort();
	  callback(tpg_f, dbits);
	}
      }
    }
  }
}

#if FSIM_BSIDE
// @brief 状態を設定する．
void
FSIM_CLASSNAME::set_state(
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
FSIM_CLASSNAME::get_state(
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

// @brief 1クロック分のシミュレーションを行い，遷移回数を数える．
SizeType
FSIM_CLASSNAME::calc_wsa(
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

// @brief 1クロック分のシミュレーションを行い，遷移回数を数える．
SizeType
FSIM_CLASSNAME::calc_wsa(
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

// @brief ノードの出力の(重み付き)信号遷移回数を求める．
SizeType
FSIM_CLASSNAME::_calc_wsa(
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
#else
// @brief 1クロック分のシミュレーションを行い，遷移回数を数える．
SizeType
FSIM_CLASSNAME::calc_wsa(
  const TestVector& tv,
  bool weighted
)
{
  return 0;
}

// @brief 状態を設定する．
void
FSIM_CLASSNAME::set_state(
  const InputVector& i_vect,
  const DffVector& f_vect
)
{
}

// @brief 状態を取得する．
void
FSIM_CLASSNAME::get_state(
  InputVector& i_vect,
  DffVector& f_vect
)
{
}

// @brief 1クロック分のシミュレーションを行い，遷移回数を数える．
SizeType
FSIM_CLASSNAME::calc_wsa(
  const InputVector& i_vect,
  bool weighted
)
{
  return 0;
}
#endif

#if FSIM_COMBI
// @brief 正常値の計算を行う．(縮退故障用)
void
FSIM_CLASSNAME::_calc_gval(
  const TestVector& tv
)
{
  // 入力の設定を行う．
  for ( SizeType iid = 0; iid < ppi_num(); ++ iid ) {
    auto simnode = ppi(iid);
    auto val3 = tv.ppi_val(iid);
    simnode->set_val(val3_to_packedval(val3));
  }

  // 正常値の計算を行う．
  _calc_val();
}

// @brief 正常値の計算を行う．
void
FSIM_CLASSNAME::_calc_gval(
  const vector<TestVector>& tv_list
)
{
  // 設定されていないビットはどこか他の設定されているビットをコピーする．
  auto x_val = init_val();
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

// @brief 正常値の計算を行う．(縮退故障用)
void
FSIM_CLASSNAME::_calc_gval(
  const AssignList& assign_list
)
{
  // デフォルト値で初期化する．
  auto val0 = init_val();
  for ( auto simnode: ppi_list() ) {
    simnode->set_val(val0);
  }

  for ( auto nv: assign_list ) {
    ASSERT_COND( nv.time() == 1 );
    SizeType iid = nv.node()->input_id();
    auto simnode = ppi(iid);
    simnode->set_val(bool_to_packedval(nv.val()));
  }

  // 正常値の計算を行う．
  _calc_val();
}

// @brief 正常値の計算を行う．
void
FSIM_CLASSNAME::_calc_gval2(
  const AssignList& assign_list
)
{
  // 値をクリアする．
  auto x_val = init_val();
  for ( auto node: mLogicArray ) {
    node->set_val(x_val);
  }

  // 値をセットする．
  for ( auto nv: assign_list ) {
    ASSERT_COND( nv.time() == 1 );
    auto node = nv.node();
    auto val = nv.val();
    auto simnode = mSimNodeMap[node->id()];
    simnode->set_val(bool_to_packedval(val));
  }

  // 正常値の計算を行う．
  _calc_val();
}
#endif

#if FSIM_BSIDE
// @brief 正常値の計算を行う．(遷移故障用)
void
FSIM_CLASSNAME::_calc_gval(
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

// @brief 正常値の計算を行う．(遷移故障用)
void
FSIM_CLASSNAME::_calc_gval(
  const vector<TestVector>& tv_list
)
{
  // 1時刻目の入力を設定する．
  // 設定されていないビットはどこか他の設定されているビットをコピーする．
  for ( SizeType iid = 0; iid < ppi_num(); ++ iid ) {
    auto simnode = ppi(iid);
    auto val = init_val();
    PackedVal bit = 1UL;
    for ( SizeType pos = 0; pos < PV_BITLEN; ++ pos, bit <<= 1 ) {
      SizeType epos = (pos < tv_list.size()) ? pos : 0;
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
    auto val = init_val();
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

// @brief 正常値の計算を行う．(遷移故障用)
void
FSIM_CLASSNAME::_calc_gval(
  const AssignList& assign_list
)
{
  // 1時刻目の入力を設定する．
  auto val0 = init_val();
  for ( auto simnode: ppi_list() ) {
    simnode->set_val(val0);
  }

  for ( auto nv: assign_list ) {
    if ( nv.time() == 0 ) {
      SizeType iid = nv.node()->input_id();
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
      SizeType iid = nv.node()->input_id();
      auto simnode = ppi(iid);
      simnode->set_val(bool_to_packedval(nv.val()));
    }
  }

  // 2時刻目の正常値の計算を行う．
  _calc_val();
}

// @brief 正常値の計算を行う．
void
FSIM_CLASSNAME::_calc_gval2(
  const AssignList& assign_list
)
{
  // 値をクリアする．
  auto x_val = init_val();
  for ( auto node: mLogicArray ) {
    node->set_val(x_val);
  }

  // 1時刻目の値をセットする．
  for ( auto nv: assign_list ) {
    if ( nv.time() == 0 ) {
      auto node = nv.node();
      auto val = nv.val();
      auto simnode = mSimNodeMap[node->id()];
      simnode->set_val(bool_to_packedval(val));
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

  // 値をクリアする．
  for ( auto node: mLogicArray ) {
    node->set_val(x_val);
  }

  // 2時刻目の値をセットする．
  for ( auto nv: assign_list ) {
    if ( nv.time() == 1 ) {
      auto node = nv.node();
      auto val = nv.val();
      auto simnode = mSimNodeMap[node->id()];
      simnode->set_val(bool_to_packedval(val));
    }
  }

  // 2時刻目の正常値の計算を行う．
  _calc_val();
}
#endif

// @brief 外部入力ノードを作る．
SimNode*
FSIM_CLASSNAME::make_input()
{
  auto id = mNodeArray.size();
  auto node = SimNode::new_input(id);
  mNodeArray.push_back(unique_ptr<SimNode>{node});
  return node;
}

// @brief 単純な logic ノードを作る．
SimNode*
FSIM_CLASSNAME::make_gate(
  PrimType type,
  const vector<SimNode*>& inputs
)
{
  auto id = mNodeArray.size();
  auto node = SimNode::new_gate(id, type, inputs);
  mNodeArray.push_back(unique_ptr<SimNode>{node});
  mLogicArray.push_back(node);
  return node;
}


//////////////////////////////////////////////////////////////////////
// クラス SimFault
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SimFault::SimFault(
  const TpgFault* f,
  SimNode* node,
  const vector<SimNode*>& simmap
) : mTpgFault{f},
    mNode{node}
{
  // もとの excitation_condition を SimNode に置き換える．
  for ( auto nodeval: f->excitation_condition() ) {
    auto src_node = nodeval.node();
    auto val = nodeval.val();
    auto simnode = simmap[src_node->id()];
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
