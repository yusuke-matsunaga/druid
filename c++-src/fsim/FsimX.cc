
/// @file FsimX.cc
/// @brief FsimX の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2017, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "FsimX.h"

#include "TpgNetwork.h"
#include "TpgDff.h"
#include "TpgNode.h"
#include "TpgFault.h"

#include "GateType.h"

#include "TestVector.h"
#include "InputVector.h"
#include "DffVector.h"
#include "NodeValList.h"

#include "SimNode.h"
#include "SimFFR.h"
#include "InputVals.h"

#include "ym/HashSet.h"
#include "ym/Range.h"
#include "ym/StopWatch.h"


BEGIN_NAMESPACE_DRUID_FSIM

BEGIN_NONAMESPACE

// Val3 を PackedVal/PackedVal3 に変換する．
inline
FSIM_VALTYPE
val3_to_packedval(Val3 val)
{
#if FSIM_VAL2
  // kValX は kVal0 とみなす．
  return (val == Val3::_1) ? kPvAll1 : kPvAll0;
#elif FSIM_VAL3
  switch ( val ) {
  case Val3::_X: return PackedVal3(kPvAll0, kPvAll0);
  case Val3::_0: return PackedVal3(kPvAll1, kPvAll0);
  case Val3::_1: return PackedVal3(kPvAll0, kPvAll1);
  }
#endif
}

// PackedVal/PackedVal3 を Val3 に変換する．
// 最下位ビットだけで判断する．
inline
Val3
packedval_to_val3(FSIM_VALTYPE pval)
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
new_Fsim(const TpgNetwork& network)
{
  return static_cast<std::unique_ptr<FsimImpl>>(new FSIM_CLASSNAME(network));
}


//////////////////////////////////////////////////////////////////////
// FsimX
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] network ネットワーク
FSIM_CLASSNAME::FSIM_CLASSNAME(const TpgNetwork& network)
{
  mPatMap = kPvAll0;
  mPPIArray = nullptr;
  mPPOArray = nullptr;
  mPrevValArray = nullptr;
  mFFRArray = nullptr;
  mFFRMap = nullptr;
  mSimFaults = nullptr;
  mFaultArray = nullptr;
  mDetFaultArray = nullptr;
  mDetPatArray = nullptr;

  set_network(network);
}

// @brief デストラクタ
FSIM_CLASSNAME::~FSIM_CLASSNAME()
{
  clear();
}

// @brief ネットワークをセットする関数
// @param[in] network ネットワーク
void
FSIM_CLASSNAME::set_network(const TpgNetwork& network)
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

  mPPIArray = new SimNode*[ni];
  mPPOArray = new SimNode*[no];
  mPrevValArray = new FSIM_VALTYPE[nn];

  auto nf = 0;
  for ( auto tpgnode: network.node_list() ) {
    nf += network.node_rep_fault_num(tpgnode->id());

    SimNode* node = nullptr;

    if ( tpgnode->is_ppi() ) {
      // 外部入力に対応する SimNode の生成
      node = make_input();
      mPPIArray[tpgnode->input_id()] = node;
    }
    else if ( tpgnode->is_ppo() ) {
      // 外部出力に対応する SimNode の生成
      auto inode = simmap[tpgnode->fanin(0)->id()];
      // 実際にはバッファタイプのノードに出力の印をつけるだけ．
      node = make_gate(GateType::Buff, vector<SimNode*>(1, inode));
      node->set_output();
      mPPOArray[tpgnode->output_id()] = node;
    }
    else if ( tpgnode->is_dff_clock() ||
	      tpgnode->is_dff_clear() ||
	      tpgnode->is_dff_preset() ) {
      // DFFの制御端子に対応する SimNode の生成
      auto inode = simmap[tpgnode->fanin(0)->id()];
      // 実際にはバッファタイプのノードに出力の印をつけるだけ．
      node = make_gate(GateType::Buff, vector<SimNode*>(1, inode));
      node->set_output();
    }
    else if ( tpgnode->is_logic() ) {
      // 論理ノードに対する SimNode の作成
      auto ni = tpgnode->fanin_num();

      // ファンインに対応する SimNode を探す．
      vector<SimNode*> inputs;
      inputs.reserve(ni);
      for ( auto itpgnode: tpgnode->fanin_list() ) {
	auto inode = simmap[itpgnode->id()];
	ASSERT_COND( inode != nullptr);

	inputs.push_back(inode);
      }

      // 出力の論理を表す SimNode を作る．
      auto type = tpgnode->gate_type();
      node = make_gate(type, inputs);
    }
    // 対応表に登録しておく．
    simmap[tpgnode->id()] = node;
  }

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
  mFFRArray = new SimFFR[ffr_num];
  mFFRMap = new SimFFR*[mNodeArray.size()];
  ffr_num = 0;
  for ( int i = node_num; -- i >= 0; ) {
    auto node = mNodeArray[i];
    if ( node->is_output() || node->fanout_num() != 1 ) {
      auto ffr = &mFFRArray[ffr_num];
      node->set_ffr_root();
      mFFRMap[node->id()] = ffr;
      ffr->set_root(node);
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
  for ( auto inode: Array<SimNode*>(mPPOArray, 0, no) ) {
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
  mSimFaults = new SimFault[nf];
  mFaultArray = new SimFault*[network.max_fault_id()];
  mDetFaultArray = new const TpgFault*[nf];
  mDetPatArray = new PackedVal[nf];
  auto fid = 0;
  for ( auto tpgnode: network.node_list() ) {
    auto simnode = simmap[tpgnode->id()];
    auto ffr = mFFRMap[simnode->id()];
    auto nf1 = network.node_rep_fault_num(tpgnode->id());
    for ( auto j: Range(0, nf1) ) {
      auto fault = network.node_rep_fault(tpgnode->id(), j);
      SimNode* isimnode = nullptr;
      int ipos = 0;
      if ( fault->is_branch_fault() ) {
	ipos = fault->tpg_pos();
	auto inode = tpgnode->fanin(ipos);
	isimnode = simmap[inode->id()];
      }
      else {
	isimnode = simnode;
      }
      mSimFaults[fid].set(fault, simnode, ipos, isimnode);
      auto ff = &mSimFaults[fid];
      mFaultArray[fault->id()] = ff;
      ff->mSkip = false;
      ffr->add_fault(ff);
      ++ fid;
    }
  }
}

// @brief FFR のリストを返す．
Array<SimFFR>
FSIM_CLASSNAME::_ffr_list() const
{
  return Array<SimFFR>(mFFRArray, 0, mFFRNum);
}

// @brief 全ての故障にスキップマークをつける．
void
FSIM_CLASSNAME::set_skip_all()
{
  for ( auto& sim_fault: Array<SimFault>(mSimFaults, 0, mFaultNum) ) {
    sim_fault.mSkip = true;
  }
}

// @brief 故障にスキップマークをつける．
// @param[in] f 対象の故障
void
FSIM_CLASSNAME::set_skip(const TpgFault* f)
{
  mFaultArray[f->id()]->mSkip = true;
}

// @brief 全ての故障のスキップマークを消す．
void
FSIM_CLASSNAME::clear_skip_all()
{
  for ( auto& sim_fault: Array<SimFault>(mSimFaults, 0, mFaultNum) ) {
    sim_fault.mSkip = false;
  }
}

// @brief 故障のスキップマークを消す．
// @param[in] f 対象の故障
void
FSIM_CLASSNAME::clear_skip(const TpgFault* f)
{
  mFaultArray[f->id()]->mSkip = false;
}

// @brief SPSFP故障シミュレーションを行う．
// @param[in] tv テストベクタ
// @param[in] f 対象の故障
// @retval true 故障の検出が行えた．
// @retval false 故障の検出が行えなかった．
bool
FSIM_CLASSNAME::spsfp(const TestVector& tv,
		      const TpgFault* f)
{
  TvInputVals iv(tv);

  // 正常値の計算を行う．
  _calc_gval(iv);

  // 故障伝搬を行う．
  return _spsfp(f);
}

// @brief SPSFP故障シミュレーションを行う．
// @param[in] assign_list 値の割当リスト
// @param[in] f 対象の故障
// @retval true 故障の検出が行えた．
// @retval false 故障の検出が行えなかった．
bool
FSIM_CLASSNAME::spsfp(const NodeValList& assign_list,
		      const TpgFault* f)
{
  NvlInputVals iv(assign_list);

  // 正常値の計算を行う．
  _calc_gval(iv);

  // 故障伝搬を行う．
  return _spsfp(f);
}

// @brief ひとつのパタンで故障シミュレーションを行う．
// @param[in] tv テストベクタ
// @return 検出された故障数を返す．
//
// 検出された故障は det_fault() で取得する．
int
FSIM_CLASSNAME::sppfp(const TestVector& tv)
{
  TvInputVals iv(tv);

  // 正常値の計算を行う．
  _calc_gval(iv);

  // 故障伝搬を行う．
  return _sppfp();
}

// @brief ひとつのパタンで故障シミュレーションを行う．
// @param[in] assign_list 値の割当リスト
// @return 検出された故障数を返す．
//
// 検出された故障は det_fault() で取得する．
int
FSIM_CLASSNAME::sppfp(const NodeValList& assign_list)
{
  NvlInputVals iv(assign_list);

  // 正常値の計算を行う．
  _calc_gval(iv);

  // 故障伝搬を行う．
  return _sppfp();
}

// @brief 複数のパタンで故障シミュレーションを行う．
// @return 検出された故障数を返す．
//
// 検出された故障は det_fault() で取得する．<br>
// 最低1つのパタンが set_pattern() で設定されている必要がある．<br>
int
FSIM_CLASSNAME::ppsfp()
{
  if ( mPatMap == kPvAll0 ) {
    // パタンが一つも設定されていない．
    mDetNum = 0;
    return 0;
  }

  Tv2InputVals iv(mPatMap, mPatBuff);

  // 正常値の計算を行う．
  _calc_gval(iv);

  // 故障伝搬を行う．
  return _ppsfp();
}

// @brief ppsfp 用のパタンバッファをクリアする．
void
FSIM_CLASSNAME::clear_patterns()
{
  mPatMap = kPvAll0;
  mPatFirstBit = kPvBitLen;
}

// @brief ppsfp 用のパタンを設定する．
// @param[in] pos 位置番号 ( 0 <= pos < kPvBitLen )
// @param[in] tv テストベクタ
void
FSIM_CLASSNAME::set_pattern(int pos,
			    const TestVector& tv)
{
  ASSERT_COND( pos >= 0 && pos < kPvBitLen );

  mPatBuff[pos] = tv;
  mPatMap |= (1ULL << pos);

  if ( mPatFirstBit > pos ) {
    mPatFirstBit = pos;
  }
}

// @brief 設定した ppsfp 用のパタンを読み出す．
// @param[in] pos 位置番号 ( 0 <= pos < kPvBitLen )
TestVector
FSIM_CLASSNAME::get_pattern(int pos)
{
  ASSERT_COND( pos >= 0 && pos < kPvBitLen );
  ASSERT_COND ( mPatMap & (1ULL << pos) );

  return mPatBuff[pos];
}

// @brief SPSFP故障シミュレーションの本体
// @param[in] f 対象の故障
// @retval true 故障の検出が行えた．
// @retval false 故障の検出が行えなかった．
bool
FSIM_CLASSNAME::_spsfp(const TpgFault* f)
{
  auto ff = mFaultArray[f->id()];

  // FFR の根までの伝搬条件を求める．
  auto obs = _fault_prop(ff);

  // obs が 0 ならその後のシミュレーションを行う必要はない．
  if ( obs == kPvAll0 ) {
    return false;
  }

  // FFR の根のノードを求める．
  auto root = ff->mNode->ffr_root();

  // root からの故障伝搬シミュレーションを行う．
  obs = _prop_sim(root, kPvAll1);

  return (obs != kPvAll0);
}

// @brief SPPFP故障シミュレーションの本体
// @return 検出された故障数を返す．
int
FSIM_CLASSNAME::_sppfp()
{
  const SimFFR* ffr_buff[kPvBitLen];

  mDetNum = 0;
  auto bitpos = 0;
  // FFR ごとに処理を行う．
  for ( auto& ffr: _ffr_list() ) {
    // FFR 内の故障伝搬を行う．
    // 結果は SimFault.mObsMask に保存される．
    // FFR 内の全ての obs マスクを ffr_req に入れる．
    auto ffr_req = _foreach_faults(ffr.fault_list());
    if ( ffr_req == kPvAll0 ) {
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

      if ( bitpos == kPvBitLen ) {
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
// @return 検出された故障数を返す．
//
// 検出された故障は det_fault() で取得する．<br>
// 最低1つのパタンが set_pattern() で設定されている必要がある．<br>
int
FSIM_CLASSNAME::_ppsfp()
{
  // FFR ごとに処理を行う．
  mDetNum = 0;
  for ( auto& ffr: _ffr_list() ) {
    auto& fault_list = ffr.fault_list();
    // FFR 内の故障伝搬を行う．
    // 結果は SimFault::mObsMask に保存される．
    // FFR 内の全ての obs マスクを ffr_req に入れる．
    auto ffr_req = _foreach_faults(fault_list) & mPatMap;

    // ffr_req が 0 ならその後のシミュレーションを行う必要はない．
    if ( ffr_req == kPvAll0 ) {
      continue;
    }

    // FFR の出力の故障伝搬を行う．
    auto obs = _prop_sim(ffr.root(), ffr_req);

    _fault_sweep(fault_list, obs);
  }

  return mDetNum;
}

// @brief 状態を設定する．
// @param[in] i_vect 外部入力のビットベクタ
// @param[in] f_vect FFの値のビットベクタ
void
FSIM_CLASSNAME::set_state(const InputVector& i_vect,
			  const DffVector& f_vect)
{
  int i = 0;
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

  // mPrevValArray に値をコピーする．
  for ( auto node: mNodeArray ) {
    mPrevValArray[node->id()] = node->val();
  }

  // DFF の出力の値を入力にコピーする．
  for ( auto i: Range(mDffNum) ) {
    auto onode = mPPOArray[i + mOutputNum];
    auto inode = mPPIArray[i + mInputNum];
    inode->set_val(onode->val());
  }
}

// @brief 状態を取得する．
// @param[in] i_vect 外部入力のビットベクタ
// @param[in] f_vect FFの値のビットベクタ
void
FSIM_CLASSNAME::get_state(InputVector& i_vect,
			  DffVector& f_vect)
{
  int i = 0;
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
// @param[in] i_vect 外部入力のビットベクタ
int
FSIM_CLASSNAME::calc_wsa(const InputVector& i_vect,
			 bool weighted)
{
  int i = 0;
  for ( auto simnode: input_list() ) {
    auto val3 = i_vect.val(i);
    simnode->set_val(val3_to_packedval(val3));
  }

  // 各信号線の値を計算する．
  _calc_val();

  // 遷移回数を数える．
  int wsa = 0;
  for ( auto node: mNodeArray ) {
    wsa += _calc_wsa(node, weighted);
  }

  // 1時刻シフトする．

  // mPrevValArray に値をコピーする．
  for ( auto node: mNodeArray ) {
    mPrevValArray[node->id()] = node->val();
  }

  // DFF の出力の値を入力にコピーする．
  for ( auto i: Range(mDffNum) ) {
    auto onode = mPPOArray[i + mOutputNum];
    auto inode = mPPIArray[i + mInputNum];
    inode->set_val(onode->val());
  }

  return wsa;
}

// @brief 1クロック分のシミュレーションを行い，遷移回数を数える．
// @param[in] tv テストベクタ
//
// - 外部入力以外は無視する．
// - 時刻1の割り当ても無視する
// weightedの意味は以下の通り
// - false: ゲートの出力の遷移回数の和
// - true : ゲートの出力の遷移回数に(ファンアウト数＋１)を掛けたものの和
int
FSIM_CLASSNAME::calc_wsa(const TestVector& tv,
			 bool weighted)
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
int
FSIM_CLASSNAME::_calc_wsa(SimNode* node,
			  bool weighted)
{
  int wsa = 0;
  if ( mPrevValArray[node->id()] != node->val() ) {
    wsa = 1;
    if ( weighted ) {
      wsa += node->fanout_num();
    }
  }
  return wsa;
}

#if FSIM_SA
// @brief 正常値の計算を行う．(縮退故障用)
// @param[in] input_vals 入力値
void
FSIM_CLASSNAME::_calc_gval(const InputVals& input_vals)
{
  // 入力の設定を行う．
  input_vals.set_val(*this);

  // 正常値の計算を行う．
  _calc_val();
}
#endif

#if FSIM_TD
// @brief 正常値の計算を行う．(遷移故障用)
// @param[in] input_vals 入力値
void
FSIM_CLASSNAME::_calc_gval(const InputVals& input_vals)
{
  // 1時刻目の入力を設定する．
  input_vals.set_val1(*this);

  // 1時刻目の正常値の計算を行う．
  _calc_val();

  // 1時刻シフトする．

  // mPrevValArray に値をコピーする．
  for ( auto node: mNodeArray ) {
    mPrevValArray[node->id()] = node->val();
  }

  // DFF の出力の値を入力にコピーする．
  for ( auto i: Range(mDffNum) ) {
    auto onode = mPPOArray[i + mOutputNum];
    auto inode = mPPIArray[i + mInputNum];
    inode->set_val(onode->val());
  }

  // 2時刻目の入力を設定する．
  input_vals.set_val2(*this);

  // 2時刻目の正常値の計算を行う．
  _calc_val();
}
#endif

// @brief 値の計算を行う．
//
// 入力ノードに値の設定は済んでいるものとする．
void
FSIM_CLASSNAME::_calc_val()
{
  for ( auto node: mLogicArray ) {
    node->calc_val();
  }
}

// @brief 個々の故障に FaultProp を適用する．
// @param[in] fault_list 故障のリスト
// @return 全ての故障の伝搬結果のORを返す．
PackedVal
FSIM_CLASSNAME::_foreach_faults(const vector<SimFault*>& fault_list)
{
  auto ffr_req = kPvAll0;
  for ( auto ff: fault_list ) {
    if ( ff->mSkip ) {
      continue;
    }

    auto obs = _fault_prop(ff);

    ff->mObsMask = obs;
    ffr_req |= obs;
  }

  return ffr_req;
}

// @brief シミュレーションを行って sppfp 用の _fault_sweep() を呼ぶ出す．
// @param[in] ffr_buf FFR を入れた配列
// @param[in] ffr_num FFR 数
void
FSIM_CLASSNAME::_do_simulation(const SimFFR* ffr_buff[],
			       int ffr_num)
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
// @param[in] fault_list 故障のリスト
void
FSIM_CLASSNAME::_fault_sweep(const vector<SimFault*>& fault_list)
{
  for ( auto ff: fault_list ) {
    if ( ff->mSkip || ff->mObsMask == kPvAll0 ) {
      continue;
    }
    auto f = ff->mOrigF;
    mDetFaultArray[mDetNum] = f;
    ++ mDetNum;
  }
}

// @brief 故障をスキャンして結果をセットする(ppsfp用)
// @param[in] fault_list 故障のリスト
// @param[in] pat 検出パタン
void
FSIM_CLASSNAME::_fault_sweep(const vector<SimFault*>& fault_list,
			     PackedVal mask)
{
  for ( auto ff: fault_list ) {
    if ( ff->mSkip ) {
      continue;
    }
    auto pat = ff->mObsMask & mask;
    if ( pat != kPvAll0 ) {
      auto f = ff->mOrigF;
      mDetFaultArray[mDetNum] = f;
      mDetPatArray[mDetNum] = pat & mPatMap;
      ++ mDetNum;
    }
  }
}

// @brief 現在保持している SimNode のネットワークを破棄する．
void
FSIM_CLASSNAME::clear()
{
  // mNodeArray が全てのノードを持っている
  for ( auto node: mNodeArray ) {
    delete node;
  }
  mNodeArray.clear();

  delete [] mPPIArray;
  delete [] mPPOArray;
  delete [] mPrevValArray;

  mLogicArray.clear();

  delete [] mFFRArray;
  delete [] mFFRMap;

  delete [] mSimFaults;
  delete [] mFaultArray;
  delete [] mDetFaultArray;
  delete [] mDetPatArray;
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
FSIM_CLASSNAME::make_gate(GateType type,
			  const vector<SimNode*>& inputs)
{
  auto id = mNodeArray.size();
  auto node = SimNode::new_gate(id, type, inputs);
  mNodeArray.push_back(node);
  mLogicArray.push_back(node);
  return node;
}

END_NAMESPACE_DRUID_FSIM
