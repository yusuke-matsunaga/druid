
/// @file FsimX.cc
/// @brief FsimX の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2017, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "FsimX.h"

#include "TpgNetwork.h"
#include "TpgDFF.h"
#include "TpgNode.h"
#include "TpgFault.h"
#include "TpgFaultMgr.h"

#include "TestVector.h"
#include "InputVector.h"
#include "DffVector.h"
#include "NodeValList.h"

#include "SimNode.h"
#include "SimFFR.h"
#include "InputVals.h"

#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID_FSIM

BEGIN_NONAMESPACE

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
  TpgFaultMgr& fmgr
)
{
  return static_cast<std::unique_ptr<FsimImpl>>(new FSIM_CLASSNAME(network, fmgr));
}


//////////////////////////////////////////////////////////////////////
// FsimX
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
FSIM_CLASSNAME::FSIM_CLASSNAME(
  const TpgNetwork& network,
  TpgFaultMgr& fmgr
) : mFaultMgr{fmgr}
{
  mPatMap = PV_ALL0;

  set_network(network);
}

// @brief デストラクタ
FSIM_CLASSNAME::~FSIM_CLASSNAME()
{
  // mNodeArray が全てのノードを持っている
  for ( auto node: mNodeArray ) {
    delete node;
  }

  for ( auto fault: mFaultList ) {
    delete fault;
  }
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
  vector<SimNode*> simmap(nn);

  mPPIList.clear();
  mPPIList.resize(ni);
  mPPOList.clear();
  mPPOList.resize(no);

  SizeType max_node_id = 0;
  SizeType nf = 0;
  for ( auto tpgnode: network.node_list() ) {
    for ( auto f: mFaultMgr.node_fault_list(tpgnode->id()) ) {
      max_node_id = std::max(max_node_id, f.id());
      ++ nf;
    }

    SimNode* node = nullptr;

    if ( tpgnode->is_ppi() ) {
      // 外部入力に対応する SimNode の生成
      node = make_input();
      mPPIList[tpgnode->input_id()] = node;
    }
    else if ( tpgnode->is_ppo() ) {
      // 外部出力に対応する SimNode の生成
      auto inode = simmap[tpgnode->fanin(0)->id()];
      node = make_output(inode);
      mPPOList[tpgnode->output_id()] = node;
    }
    else if ( tpgnode->is_logic() ) {
      // 論理ノードに対する SimNode の作成
      auto ni = tpgnode->fanin_num();

      // ファンインに対応する SimNode を探す．
      vector<SimNode*> inputs;
      inputs.reserve(ni);
      for ( auto itpgnode: tpgnode->fanin_list() ) {
	auto inode = simmap[itpgnode->id()];
	ASSERT_COND( inode != nullptr );

	inputs.push_back(inode);
      }

      // 出力の論理を表す SimNode を作る．
      auto type = tpgnode->gate_type();
      node = make_gate(type, inputs);
    }
    // 対応表に登録しておく．
    simmap[tpgnode->id()] = node;
  }
  ++ max_node_id;

  // 各ノードのファンアウトリストの設定
  auto node_num = mNodeArray.size();
  {
    vector<vector<SimNode*> > fanout_lists(node_num);
    vector<int> ipos(node_num);
    for ( auto node: mNodeArray ) {
      auto ni = node->fanin_num();
      for ( auto i: Range(0, ni) ) {
	auto inode = node->fanin(i);
	fanout_lists[inode->id()].push_back(node);
	ipos[inode->id()] = i;
      }
    }
    for ( auto i: Range(node_num) ) {
      auto node = mNodeArray[i];
      node->set_fanout_list(fanout_lists[i], ipos[i]);
    }
  }

  // FFR の設定
  auto ffr_num = 0;
  for ( auto node: mNodeArray ) {
    if ( node->is_output() || node->fanout_num() != 1 ) {
      ++ ffr_num;
    }
  }

  mFFRNum = ffr_num;
  mFFRArray.clear();
  mFFRArray.resize(ffr_num);
  vector<SimFFR*> ffr_map(mNodeArray.size());
  ffr_num = 0;
  for ( int i = node_num; -- i >= 0; ) {
    auto node = mNodeArray[i];
    if ( node->is_output() || node->fanout_num() != 1 ) {
      auto ffr = &mFFRArray[ffr_num];
      node->set_ffr_root();
      ffr_map[node->id()] = ffr;
      ffr->set_root(node);
      ++ ffr_num;
    }
    else {
      auto fo_node = node->fanout_top();
      auto ffr = ffr_map[fo_node->id()];
      ffr_map[node->id()] = ffr;
    }
  }

  // 最大レベルを求め，イベントキューを初期化する．
  auto max_level = 0;
  for ( auto inode: mPPOList ) {
    if ( max_level < inode->level() ) {
      max_level = inode->level();
    }
  }
  mEventQ.init(max_level, mNodeArray.size());

  //////////////////////////////////////////////////////////////////////
  // 故障リストの設定
  //////////////////////////////////////////////////////////////////////

  // 同時に各 SimFFR 内の故障リストも再構築する．
  mFaultNum = nf;
  mFaultList.clear();
  mFaultMap.clear();
  mDetFaultArray.clear();
  mDetPatArray.clear();
  mFaultList.reserve(nf);
  mFaultMap.resize(max_node_id, nullptr);
  mDetFaultArray.reserve(nf);
  mDetPatArray.reserve(nf);
  for ( auto tpgnode: network.node_list() ) {
    auto simnode = simmap[tpgnode->id()];
    auto ffr = ffr_map[simnode->id()];
    for ( auto fault: mFaultMgr.node_fault_list(tpgnode->id()) ) {
      auto ff = new SimFault{fault, simnode, simmap};
      mFaultList.push_back(ff);
      mFaultMap[fault.id()] = ff;
      ff->set_skip(false);
      ffr->add_fault(ff);
    }
  }
}

// @brief FFR のリストを返す．
const vector<SimFFR>&
FSIM_CLASSNAME::_ffr_list() const
{
  return mFFRArray;
}

// @brief 全ての故障にスキップマークをつける．
void
FSIM_CLASSNAME::set_skip_all()
{
  for ( auto f: mFaultList ) {
    f->set_skip(true);
  }
}

// @brief 故障にスキップマークをつける．
void
FSIM_CLASSNAME::set_skip(
  const TpgFault& f
)
{
  mFaultMap[f.id()]->set_skip(true);
}

// @brief 全ての故障のスキップマークを消す．
void
FSIM_CLASSNAME::clear_skip_all()
{
  for ( auto f: mFaultList ) {
    f->set_skip(false);
  }
}

// @brief 故障のスキップマークを消す．
void
FSIM_CLASSNAME::clear_skip(
  const TpgFault& f
)
{
  mFaultMap[f.id()]->set_skip(false);
}

// @brief SPSFP故障シミュレーションを行う．
bool
FSIM_CLASSNAME::spsfp(
  const TestVector& tv,
  const TpgFault& f
)
{
  TvInputVals iv{tv};

  // 正常値の計算を行う．
  _calc_gval(iv);

  // 故障伝搬を行う．
  return _spsfp(f);
}

// @brief SPSFP故障シミュレーションを行う．
bool
FSIM_CLASSNAME::spsfp(
  const NodeValList& assign_list,
  const TpgFault& f
)
{
  NvlInputVals iv{assign_list};

  // 正常値の計算を行う．
  _calc_gval(iv);

  // 故障伝搬を行う．
  return _spsfp(f);
}

// @brief ひとつのパタンで故障シミュレーションを行う．
SizeType
FSIM_CLASSNAME::sppfp(
  const TestVector& tv
)
{
  TvInputVals iv{tv};

  // 正常値の計算を行う．
  _calc_gval(iv);

  // 故障伝搬を行う．
  return _sppfp();
}

// @brief ひとつのパタンで故障シミュレーションを行う．
SizeType
FSIM_CLASSNAME::sppfp(
  const NodeValList& assign_list
)
{
  NvlInputVals iv{assign_list};

  // 正常値の計算を行う．
  _calc_gval(iv);

  // 故障伝搬を行う．
  return _sppfp();
}

// @brief 複数のパタンで故障シミュレーションを行う．
SizeType
FSIM_CLASSNAME::ppsfp()
{
  clear_det_array();

  if ( mPatMap == PV_ALL0 ) {
    // パタンが一つも設定されていない．
    return 0;
  }

  Tv2InputVals iv{mPatMap, mPatBuff};

  // 正常値の計算を行う．
  _calc_gval(iv);

  // 故障伝搬を行う．
  return _ppsfp();
}

// @brief ppsfp 用のパタンバッファをクリアする．
void
FSIM_CLASSNAME::clear_patterns()
{
  mPatMap = PV_ALL0;
  mPatFirstBit = PV_BITLEN;
}

// @brief ppsfp 用のパタンを設定する．
void
FSIM_CLASSNAME::set_pattern(
  SizeType pos,
  const TestVector& tv
)
{
  ASSERT_COND( pos >= 0 && pos < PV_BITLEN );

  mPatBuff[pos] = tv;
  mPatMap |= (1ULL << pos);

  if ( mPatFirstBit > pos ) {
    mPatFirstBit = pos;
  }
}

// @brief 設定した ppsfp 用のパタンを読み出す．
TestVector
FSIM_CLASSNAME::get_pattern(
  SizeType pos
)
{
  ASSERT_COND( pos >= 0 && pos < PV_BITLEN );
  ASSERT_COND ( mPatMap & (1ULL << pos) );

  return mPatBuff[pos];
}

// @brief 直前の sppfp/ppsfp で検出された故障を返す．
TpgFault
FSIM_CLASSNAME::det_fault(
  SizeType pos
)
{
  ASSERT_COND( pos >= 0 && pos < det_fault_num() );

  return mFaultMgr.fault(mDetFaultArray[pos]);
}

// @brief 直前の sppfp/ppsfp で検出された故障のリストを返す．
vector<TpgFault>
FSIM_CLASSNAME::det_fault_list()
{
  vector<TpgFault> fault_list;
  fault_list.reserve(det_fault_num());
  for ( auto fid: mDetFaultArray ) {
    fault_list.push_back(mFaultMgr.fault(fid));
  }
  return fault_list;
}

// @brief SPSFP故障シミュレーションの本体
bool
FSIM_CLASSNAME::_spsfp(
  const TpgFault& f
)
{
  auto ff = mFaultMap[f.id()];

  // FFR の根までの伝搬条件を求める．
  auto obs = _fault_prop(ff);

  // obs が 0 ならその後のシミュレーションを行う必要はない．
  if ( obs == PV_ALL0 ) {
    return false;
  }

  // FFR の根のノードを求める．
  auto root = ff->origin_node()->ffr_root();

  // root からの故障伝搬シミュレーションを行う．
  obs = _prop_sim(root, PV_ALL1);

  return (obs != PV_ALL0);
}

// @brief SPPFP故障シミュレーションの本体
SizeType
FSIM_CLASSNAME::_sppfp()
{
  clear_det_array();

  const SimFFR* ffr_buff[PV_BITLEN];
  auto bitpos = 0;
  // FFR ごとに処理を行う．
  for ( auto& ffr: _ffr_list() ) {
    // FFR 内の故障伝搬を行う．
    // 結果は SimFault.mObsMask に保存される．
    // FFR 内の全ての obs マスクを ffr_req に入れる．
    auto ffr_req = _foreach_faults(ffr.fault_list());
    if ( ffr_req == PV_ALL0 ) {
      // ffr_req が 0 ならその後のシミュレーションを行う必要はない．
      continue;
    }

    auto root = ffr.root();
    if ( root->is_output() ) {
      // 常に観測可能
      _fault_sweep(ffr.fault_list());
    }
    else {
      // キューに積んでおく
      PackedVal bitmask = 1ULL << bitpos;
      mEventQ.put_trigger(root, bitmask, false);
      ffr_buff[bitpos] = &ffr;
      ++ bitpos;

      if ( bitpos == PV_BITLEN ) {
	_do_simulation(ffr_buff, bitpos);
	bitpos = 0;
      }
    }
  }
  if ( bitpos > 0 ) {
    _do_simulation(ffr_buff, bitpos);
  }

  return mDetNum;
}

// @brief 複数のパタンで故障シミュレーションを行う．
SizeType
FSIM_CLASSNAME::_ppsfp()
{
  // FFR ごとに処理を行う．
  clear_det_array();
  for ( auto& ffr: _ffr_list() ) {
    auto& fault_list = ffr.fault_list();
    // FFR 内の故障伝搬を行う．
    // 結果は SimFault::mObsMask に保存される．
    // FFR 内の全ての obs マスクを ffr_req に入れる．
    auto ffr_req = _foreach_faults(fault_list) & mPatMap;

    // ffr_req が 0 ならその後のシミュレーションを行う必要はない．
    if ( ffr_req == PV_ALL0 ) {
      continue;
    }

    // FFR の出力の故障伝搬を行う．
    auto obs = _prop_sim(ffr.root(), ffr_req);

    _fault_sweep(fault_list, obs);
  }

  return mDetNum;
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
  for ( auto node: mNodeArray ) {
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
  for ( auto node: mNodeArray ) {
    wsa += _calc_wsa(node, weighted);
  }

  // 1時刻シフトする．
  for ( auto node: mNodeArray ) {
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
  const InputVals& input_vals
)
{
  // 入力の設定を行う．
  input_vals.set_val(*this);

  // 正常値の計算を行う．
  _calc_val();
}
#endif

#if FSIM_BSIDE
// @brief 正常値の計算を行う．(遷移故障用)
void
FSIM_CLASSNAME::_calc_gval(
  const InputVals& input_vals
)
{
  // 1時刻目の入力を設定する．
  input_vals.set_val1(*this);

  // 1時刻目の正常値の計算を行う．
  _calc_val();

  // 1時刻シフトする．
  for ( auto node: mNodeArray ) {
    node->shift_val();
  }

  // DFF の出力の値を入力にコピーする．
  for ( auto i: Range(mDffNum) ) {
    auto onode = mPPOList[i + mOutputNum];
    auto inode = mPPIList[i + mInputNum];
    inode->set_val(onode->val());
  }

  // 2時刻目の入力を設定する．
  input_vals.set_val2(*this);

  // 2時刻目の正常値の計算を行う．
  _calc_val();
}
#endif

// @brief 値の計算を行う．
void
FSIM_CLASSNAME::_calc_val()
{
  for ( auto node: mLogicArray ) {
    node->calc_val();
  }
}

// @brief 個々の故障に FaultProp を適用する．
PackedVal
FSIM_CLASSNAME::_foreach_faults(
  const vector<SimFault*>& fault_list
)
{
  auto ffr_req = PV_ALL0;
  for ( auto ff: fault_list ) {
    if ( ff->skip() ) {
      continue;
    }

    auto obs = _fault_prop(ff);
    ff->set_obs_mask(obs);
    ffr_req |= obs;
  }

  return ffr_req;
}

// @brief シミュレーションを行って sppfp 用の _fault_sweep() を呼ぶ出す．
void
FSIM_CLASSNAME::_do_simulation(
  const SimFFR* ffr_buff[],
  SizeType ffr_num
)
{
  auto obs = mEventQ.simulate();
  PackedVal mask = 1ULL;
  for ( auto i = 0; i < ffr_num; ++ i, mask <<= 1 ) {
    if ( obs & mask ) {
      _fault_sweep(ffr_buff[i]->fault_list());
    }
  }
}

// @brief 故障をスキャンして結果をセットする(sppfp用)
void
FSIM_CLASSNAME::_fault_sweep(
  const vector<SimFault*>& fault_list
)
{
  for ( auto ff: fault_list ) {
    if ( ff->skip() || ff->obs_mask() == PV_ALL0 ) {
      continue;
    }
    add_det_array(ff);
  }
}

// @brief 故障をスキャンして結果をセットする(ppsfp用)
void
FSIM_CLASSNAME::_fault_sweep(
  const vector<SimFault*>& fault_list,
  PackedVal mask
)
{
  for ( auto ff: fault_list ) {
    if ( ff->skip() ) {
      continue;
    }
    auto pat = ff->obs_mask() & mask;
    if ( pat != PV_ALL0 ) {
      add_det_array(pat, ff);
    }
  }
}

// @brief 外部入力ノードを作る．
SimNode*
FSIM_CLASSNAME::make_input()
{
  auto id = mNodeArray.size();
  auto node = SimNode::new_input(id);
  mNodeArray.push_back(node);
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
  mNodeArray.push_back(node);
  mLogicArray.push_back(node);
  return node;
}


//////////////////////////////////////////////////////////////////////
// クラス SimFault
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SimFault::SimFault(
  const TpgFault& f,
  SimNode* node,
  const vector<SimNode*>& simmap
) : mId{f.id()},
    mNode{node}
{
  // もとの excitation_condition を SimNode に置き換える．
  for ( auto nodeval: f.excitation_condition() ) {
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
