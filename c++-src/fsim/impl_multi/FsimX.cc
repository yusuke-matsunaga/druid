﻿
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
#include "SimEngine.h"
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
) : mSyncObj{0},
    mEngineList(mSyncObj.thread_num()),
    mThreadList(mSyncObj.thread_num())
{
  set_network(network);

  auto NT = mSyncObj.thread_num();
  auto NFFR = mFFRArray.size();
  for ( SizeType i = 0; i < NT; ++ i ) {
    vector<const SimFFR*> ffr_list;
    for ( SizeType j = i; j < NFFR; j += NT ) {
      ffr_list.push_back(&mFFRArray[j]);
    }
    auto engine = new SimEngine{i, mSyncObj, *this, ffr_list};
    mEngineList[i] = unique_ptr<SimEngine>{engine};
  }

  // スレッドを生成する．
  for ( SizeType i = 0; i < NT; ++ i ) {
    auto& engine = mEngineList[i];
    mThreadList[i] = std::thread{[&](){
	for ( bool go_on = true; go_on; ) {
	  auto cmd = mSyncObj.get_command(engine->id());
	  switch ( cmd ) {
	  case Cmd::PPSFP:
	    engine->ppsfp(mSyncObj.input_vals());
	    break;

	  case Cmd::SPPFP:
	    engine->sppfp(mSyncObj.input_vals());
	    break;

	  case Cmd::END:
	    go_on = false;
	    break;
	  }
	}
      }};
  }
  // 子スレッドがコマンド待ちになるまで待つ．
  mSyncObj.wait();
}

// @brief デストラクタ
FSIM_CLASSNAME::~FSIM_CLASSNAME()
{
  // 終了コマンドを送る．
  mSyncObj.put_end();

  // 子スレッドの終了を待つ．
  for ( auto& thr: mThreadList ) {
    thr.join();
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
  SimEngine engine{0, mSyncObj, *this, {}};
  auto ff = mFaultMap[f.id()];
  return engine.spsfp(iv, ff, dbits);
}

// @brief ひとつのパタンで故障シミュレーションを行う．
void
FSIM_CLASSNAME::sppfp(
  const TestVector& tv,
  cbtype1 callback
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
  cbtype1 callback
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
  cbtype1 callback
)
{
  // SPPFP コマンドを送る．
  mSyncObj.put_sppfp_command(iv);

  for ( auto& engine: mEngineList ) {
    engine->apply_callback1(callback);
  }
}

// @brief 複数のパタンで故障シミュレーションを行う．
void
FSIM_CLASSNAME::ppsfp(
  const vector<TestVector>& tv_list,
  cbtype2 callback
)
{
  Tv2InputVals iv{tv_list};

  // PPSFP コマンドを送る．
  mSyncObj.put_ppsfp_command(iv);

  for ( auto& engine: mEngineList ) {
    engine->apply_callback2(callback);
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
#if 0
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
#endif
}

// @brief 状態を取得する．
void
FSIM_CLASSNAME::get_state(
  InputVector& i_vect,
  DffVector& f_vect
)
{
#if 0
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
#endif
}

// @brief 1クロック分のシミュレーションを行い，遷移回数を数える．
SizeType
FSIM_CLASSNAME::calc_wsa(
  const InputVector& i_vect,
  bool weighted
)
{
#if 0
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
#else
  return 0;
#endif
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

#if 0
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
#endif
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
