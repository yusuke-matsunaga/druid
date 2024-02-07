
/// @file FsimX.cc
/// @brief FsimX の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2017, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include <thread>
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

#include "CmdQueue.h"
#include "PPSFP_Thread.h"
#include "SPPFP_Thread.h"

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
  const TpgNetwork& network
)
{
  return static_cast<std::unique_ptr<FsimImpl>>(new FSIM_CLASSNAME{network});
}


//////////////////////////////////////////////////////////////////////
// FsimX
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
FSIM_CLASSNAME::FSIM_CLASSNAME(
  const TpgNetwork& network
)
{
  set_network(network);
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
      vector<SizeType> inputs;
      inputs.reserve(ni);
      SizeType max_level = 0;
      for ( auto itpgnode: tpgnode->fanin_list() ) {
	auto inode = mSimNodeMap[itpgnode->id()];
	ASSERT_COND( inode != nullptr );

	inputs.push_back(inode->id());
	max_level = std::max(max_level, inode->level());
      }

      // 出力の論理を表す SimNode を作る．
      auto type = tpgnode->gate_type();
      ++ max_level;
      node = make_gate(type, max_level, inputs);
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
	fanout_lists[inode].push_back(node.get());
	ipos[inode] = i;
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

  // mValArray, mPrevValArray の確保
  mValArray.resize(node_num);
  mPrevValArray.resize(node_num);

  // 最大レベルを求める．
  mMaxLevel = 0;
  for ( auto inode: mPPOList ) {
    if ( mMaxLevel < inode->level() ) {
      mMaxLevel = inode->level();
    }
  }
  ++ mMaxLevel;
}

// @brief 対象の故障をセットする．
void
FSIM_CLASSNAME::set_fault_list(
  const vector<TpgFault>& fault_list
)
{
  SizeType nf = fault_list.size();
  SizeType max_fid = 0;
  for ( auto fault: fault_list ) {
    max_fid = std::max(max_fid, fault.id());
  }
  ++ max_fid;

  mFaultList.clear();
  mFaultList.reserve(nf);

  mFaultMap.clear();
  mFaultMap.resize(max_fid, nullptr);

  for ( auto fault: fault_list ) {
    auto tpgnode = fault.origin_node();
    auto simnode = mSimNodeMap[tpgnode->id()];
    auto sim_f = new SimFault{fault, simnode, mSimNodeMap};
    mFaultList.push_back(unique_ptr<SimFault>{sim_f});
    mFaultMap[fault.id()] = sim_f;
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
  const TpgFault& f
)
{
  mFaultMap[f.id()]->set_skip(true);
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
  const TpgFault& f
)
{
  mFaultMap[f.id()]->set_skip(false);
}

// @brief 故障のスキップマークを得る．
bool
FSIM_CLASSNAME::get_skip(
  const TpgFault& f
) const
{
  return mFaultMap[f.id()]->skip();
}

// @brief SPSFP故障シミュレーションを行う．
bool
FSIM_CLASSNAME::spsfp(
  const TestVector& tv,
  const TpgFault& f,
  DiffBits& dbits
)
{
  TvInputVals iv{tv};

  // 故障伝搬を行う．
  return _spsfp(iv, f, dbits);
}

// @brief SPSFP故障シミュレーションを行う．
bool
FSIM_CLASSNAME::spsfp(
  const NodeValList& assign_list,
  const TpgFault& f,
  DiffBits& dbits
)
{
  NvlInputVals iv{assign_list};

  // 故障伝搬を行う．
  return _spsfp(iv, f, dbits);
}

// @brief SPSFP故障シミュレーションの本体
bool
FSIM_CLASSNAME::_spsfp(
  const InputVals& iv,
  const TpgFault& f,
  DiffBits& dbits
)
{
  dbits = DiffBits(ppo_num());

  // 正常値の計算を行う．
  _calc_gval(iv);

  auto ff = mFaultMap[f.id()];

  // FFR の根までの伝搬条件を求める．
  auto obs = local_prop(ff);

  // obs が 0 ならその後のシミュレーションを行う必要はない．
  if ( obs == PV_ALL0 ) {
    return false;
  }

  // FFR の根のノードを求める．
  auto root = ff->origin_node()->ffr_root();
#if 0
  // root からの故障伝搬シミュレーションを行う．
  auto obs_array = _global_prop(root, PV_ALL1);
  for ( SizeType i = 0; i < ppo_num(); ++ i ) {
    if ( obs_array[i] == PV_ALL1 ) {
      dbits.set_val(i);
    }
  }
  return (obs_array.back() != PV_ALL0);
#else
  return false;
#endif
}

// @brief ひとつのパタンで故障シミュレーションを行う．
void
FSIM_CLASSNAME::sppfp(
  const TestVector& tv,
  cbtype callback
)
{
  TvInputVals iv{tv};

  // 故障伝搬を行う．
  return _sppfp(iv, callback);
}

// @brief ひとつのパタンで故障シミュレーションを行う．
void
FSIM_CLASSNAME::sppfp(
  const NodeValList& assign_list,
  cbtype callback
)
{
  NvlInputVals iv{assign_list};

  // 故障伝搬を行う．
  return _sppfp(iv, callback);
}

// @brief SPPFP故障シミュレーションの本体
void
FSIM_CLASSNAME::_sppfp(
  const InputVals& iv,
  cbtype callback
)
{
  // 正常値の計算を行う．
  _calc_gval(iv);

  // スレッドを生成する．
  SizeType nt = std::thread::hardware_concurrency();
  CmdQueue cmd_queue;
  vector<std::thread> thread_list(nt);
  vector<SPPFP_Thread> thread_func_list(nt, SPPFP_Thread{*this, cmd_queue, callback});
  for ( SizeType i = 0; i < nt; ++ i ) {
    thread_list[i] = std::thread{thread_func_list[i]};
  }

  // 子スレッドの終了を待つ．
  for ( auto& thr: thread_list ) {
    thr.join();
  }
}

// @brief 複数のパタンで故障シミュレーションを行う．
void
FSIM_CLASSNAME::ppsfp(
  const vector<TestVector>& tv_list,
  cbtype callback
)
{
  clear_patterns();
  SizeType base = 0;
  for ( SizeType index = 0; index < tv_list.size(); ++ index ) {
    auto& tv = tv_list[index];
    auto lindex = index - base;
    set_pattern(lindex, tv);
    if ( lindex == PV_BITLEN - 1 || index == tv_list.size() - 1 ) {
      _ppsfp(base, lindex + 1, callback);
      clear_patterns();
      base += PV_BITLEN;
    }
  }
}

// @brief 複数のパタンで故障シミュレーションを行う．
void
FSIM_CLASSNAME::_ppsfp(
  SizeType base,
  SizeType npat,
  cbtype callback
)
{
  Tv2InputVals iv{mPatMask, mPatBuff};

  // 正常値の計算を行う．
  _calc_gval(iv);

#if 0
  // スレッドを生成する．
  SizeType nt = std::thread::hardware_concurrency();
  CmdQueue cmd_queue;
  vector<std::thread> thread_list(nt);
  for ( SizeType i = 0; i < nt; ++ i ) {
    PPSFP_Thread func{*this, cmd_queue, base, npat, callback};
    thread_list[i] = std::thread{func};
  }

  // 子スレッドの終了を待つ．
  for ( auto& thr: thread_list ) {
    thr.join();
  }
#else
#if 0
  CmdQueue cmd_queue;
  PPSFP_Thread func{*this, cmd_queue, base, npat, callback};
  func();
#else
  EventQ event_q;
  event_q.init(max_level(), ppo_num(), node_num());
  event_q.copy_val(val_array());
  for ( auto& ffr: ffr_array() ) {
    auto ffr_req = foreach_faults(ffr);
    if ( ffr_req == PV_ALL0 ) {
      // ffr_req が 0 ならその後のシミュレーションは必要ない．
      continue;
    }

    // イベントシミュレーションを行う．
    auto root = ffr.root();
    cout << "ffr_req = " << hex << ffr_req << dec << endl;
    event_q.put_event(ffr.root(), ffr_req);
    auto obs_array = event_q.simulate();
    auto obs = obs_array.back();
    cout << "obs = " << hex << obs << dec << endl;
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
	      DiffBits dbits(ppo_num());
	      for ( SizeType j = 0; j < ppo_num(); ++ j ) {
		if ( obs_array[j] & bitmask ) {
		  dbits.set_val(j);
		}
	      }
	      callback(i + base, ff->tpg_fault(), dbits);
	    }
	  }
	}
      }
    }
  }
#endif
#endif
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
    auto val = val3_to_packedval(val3);
    mPrevValArray[simnode->id()] = val;
    ++ i;
  }

  i = 0;
  for ( auto simnode: dff_output_list() ) {
    auto val3 = f_vect.val(i);
    auto val = val3_to_packedval(val3);
    mPrevValArray[simnode->id()] = val;
    ++ i;
  }

  // 各信号線の値を計算する．
  _calc_val(mPrevValArray);

  // DFF の出力の値を入力にコピーする．
  for ( auto i: Range(mDffNum) ) {
    auto onode = mPPOList[i + mOutputNum];
    auto inode = mPPIList[i + mInputNum];
    auto val = mPrevValArray[onode->id()];
    mValArray[inode->id()] = val;
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
    auto val = mValArray[simnode->id()];
    auto val3 = packedval_to_val3(val);
    i_vect.set_val(i, val3);
    ++ i;
  }

  i = 0;
  for ( auto simnode: dff_output_list() ) {
    auto val = mValArray[simnode->id()];
    auto val3 = packedval_to_val3(val);
    f_vect.set_val(i, val3);
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
  // mValArray を mPrevValArray にコピーする．
  for ( auto& node: mNodeArray ) {
    auto val = mValArray[node->id()];
    mPrevValArray[node->id()] = val;
  }

  // DFF の出力の値を入力にコピーする．
  for ( auto i: Range(mDffNum) ) {
    auto onode = mPPOList[i + mOutputNum];
    auto inode = mPPIList[i + mInputNum];
    auto val = mPrevValArray[onode->id()];
    mValArray[inode->id()];
  }

  // 外部入力の値をセットする．
  SizeType i = 0;
  for ( auto node: input_list() ) {
    auto val3 = i_vect.val(i);
    auto val = val3_to_packedval(val3);
    mValArray[node->id()] = val;
  }

  // 各信号線の値を計算する．
  _calc_val(mValArray);

  // 遷移回数を数える．
  SizeType wsa = 0;
  for ( auto& node: mNodeArray ) {
    wsa += _calc_wsa(node.get(), weighted);
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
  if ( mPrevValArray[node->id()] != mValArray[node->id()] ) {
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
  input_vals.set_val(*this, mValArray);

  // 正常値の計算を行う．
  _calc_val(mValArray);
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
  input_vals.set_val1(*this, mPrevValArray);

  // 1時刻目の正常値の計算を行う．
  _calc_val(mPrevValArray);

  // DFF の出力の値を入力にコピーする．
  for ( auto i: Range(mDffNum) ) {
    auto onode = mPPOList[i + mOutputNum];
    auto inode = mPPIList[i + mInputNum];
    auto val = mPrevValArray[onode->id()];
    mValArray[inode->id()] = val;
  }

  // 2時刻目の入力を設定する．
  input_vals.set_val2(*this, mValArray);

  // 2時刻目の正常値の計算を行う．
  _calc_val(mValArray);
}
#endif

// @brief 個々の故障に FaultProp を適用する．
PackedVal
FSIM_CLASSNAME::foreach_faults(
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
  SizeType level,
  const vector<SizeType>& inputs
)
{
  auto id = mNodeArray.size();
  auto node = SimNode::new_gate(id, type, level, inputs);
  mNodeArray.push_back(unique_ptr<SimNode>{node});
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
) : mTpgFault{f},
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
SimFault::excitation_condition(
  const vector<FSIM_VALTYPE>& val_array
) const
{
  auto cond = PV_ALL1;
  for ( auto& tmp: mExCondList ) {
    auto node = tmp.node();
    auto val0 = tmp.val();
#if FSIM_VAL2
    auto val = val_array[node->id()];
    if ( !val0 ) {
      val = ~val;
    }
#elif FSIM_VAL3
    auto val3 = val_array[node->id()];
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
SimFault::previous_condition(
  const vector<FSIM_VALTYPE>& val_array
) const
{
  auto cond = PV_ALL1;
  for ( auto& tmp: mPrevCondList ) {
    auto node = tmp.node();
    auto val0 = tmp.val();
#if FSIM_VAL2
    auto val = val_array[node->id()];
    if ( !val0 ) {
      val = ~val;
    }
#elif FSIM_VAL3
    auto val3 = val_array[node->id()];
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
