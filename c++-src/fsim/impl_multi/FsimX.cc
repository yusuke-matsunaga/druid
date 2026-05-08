
/// @file FsimX.cc
/// @brief FsimX の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include <thread>
#include "FsimX.h"
#include "types/TpgNetwork.h"
#include "types/TpgNode.h"
#include "types/TpgFault.h"
#include "types/TpgFaultList.h"

#include "types/TestVector.h"
#include "types/InputVector.h"
#include "types/DffVector.h"
#include "types/AssignList.h"

#include "SimNode.h"
#include "SimFFR.h"
#include "SimThrFunc.h"
#include "ym/MtMgr.h"
#include "ym/IdPool.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID_FSIM

std::unique_ptr<FsimImpl>
new_Fsim(
  const TpgFaultList& fault_list,
  SizeType thread_num
)
{
  if ( thread_num == 0 ) {
    thread_num = std::thread::hardware_concurrency();
  }
  return std::unique_ptr<FsimImpl>{new FSIM_CLASSNAME(fault_list, thread_num)};
}


//////////////////////////////////////////////////////////////////////
// FsimX
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
FSIM_CLASSNAME::FSIM_CLASSNAME(
  const TpgFaultList& fault_list,
  SizeType thread_num
)
{
  auto network = fault_list.network();

  // 各スレッドが担当する FFR の数
  std::vector<SizeType> size_list(thread_num, 0);
  SizeType nffr = network.ffr_num();
  for ( SizeType i = 0; i < nffr; ++ i ) {
    ++ size_list[i % thread_num];
  }
  // 各スレッドが担当する FFR の開始位置
  std::vector<SizeType> begin_list(thread_num + 1);
  SizeType base = 0;
  for ( SizeType i = 0; i < thread_num; ++ i ) {
    begin_list[i] = base;
    base += size_list[i];
  }
  begin_list[thread_num] = base;
  // 担当する FFR がないスレッドは生成しない．
  SizeType act_num = 0;
  for ( SizeType i = 0; i < thread_num; ++ i ) {
    auto begin = begin_list[i];
    auto end = begin_list[i + 1];
    if ( begin == end ) {
      break;
    }
    ++ act_num;
  }
  mSyncObj.initialize(act_num);

  mFuncList.reserve(act_num);
  for ( SizeType i = 0; i < act_num; ++ i ) {
    std::vector<SizeType> ffr_list;
    auto begin = begin_list[i];
    auto end = begin_list[i + 1];
    ffr_list.reserve(end - begin);
    for ( SizeType j = begin; j < end; ++ j ) {
      ffr_list.push_back(j);
    }
    auto func = new SimThrFunc(i, network, fault_list, ffr_list);
    mFuncList.push_back(std::unique_ptr<SimThrFunc>{func});
  }

  // スレッドを生成する．
  mThrList.resize(act_num);
  for ( SizeType i = 0; i < act_num; ++ i ) {
    auto func = mFuncList[i].get();
    mThrList[i] = std::thread{
      [&](SimThrFunc* func) {
	for ( bool go_on = true; go_on; ) {
	  auto cmd = mSyncObj.get_command(func->id());
	  switch ( cmd ) {
	  case Cmd::PPSFP:
	    func->engine().calc_val(mSyncObj.testvector_list());
	    func->ppsfp(mSyncObj.testvector_list().size());
	    break;

	  case Cmd::SPPFP_TV:
	    func->engine().calc_val(mSyncObj.testvector());
	    func->sppfp();
	    break;

	  case Cmd::SPPFP_AS:
	    func->engine().calc_val(mSyncObj.assign_list());
	    func->sppfp();
	    break;

	  case Cmd::PPSFP2:
	    func->engine().calc_val(mSyncObj.testvector_list());
	    func->ppsfp2(mSyncObj.testvector_list().size());
	    break;

	  case Cmd::SPPFP2_TV:
	    func->engine().calc_val(mSyncObj.testvector());
	    func->sppfp2();
	    break;

	  case Cmd::SPPFP2_AS:
	    func->engine().calc_val(mSyncObj.assign_list());
	    func->sppfp2();
	    break;

	  case Cmd::END:
	    go_on = false;
	    break;
	  }
	}
      },
      func
    };
  }

  mSyncObj.wait();
}

// @brief デストラクタ
FSIM_CLASSNAME::~FSIM_CLASSNAME()
{
  // 終了コマンドを送る．
  mSyncObj.put_end();

  // 子スレッドの終了を待つ．
  for ( auto& thr: mThrList ) {
    thr.join();
  }
}

// @brief 全ての故障にスキップマークをつける．
void
FSIM_CLASSNAME::set_skip_all()
{
  for ( auto& func: mFuncList ) {
    func->engine().set_skip_all();
  }
}

// @brief 故障にスキップマークをつける．
void
FSIM_CLASSNAME::set_skip(
  SizeType fid
)
{
  for ( auto& func: mFuncList ) {
    func->engine().set_skip(fid);
  }
}

// @brief 全ての故障のスキップマークを消す．
void
FSIM_CLASSNAME::clear_skip_all()
{
  for ( auto& func: mFuncList ) {
    func->engine().clear_skip_all();
  }
}

// @brief 故障のスキップマークを消す．
void
FSIM_CLASSNAME::clear_skip(
  SizeType fid
)
{
  for ( auto& func: mFuncList ) {
    func->engine().clear_skip(fid);
  }
}

// @brief 故障のスキップマークを得る．
bool
FSIM_CLASSNAME::get_skip(
  SizeType fid
) const
{
  for ( auto& func: mFuncList ) {
    if ( func->engine().get_skip(fid) ) {
      return true;
    }
  }
  return false;
}

// @brief SPSFP故障シミュレーションを行う．
bool
FSIM_CLASSNAME::spsfp(
  const TestVector& tv,
  SizeType fid
)
{
  // マルチスレッド版は対応していない．
  return false;
}

// @brief SPSFP故障シミュレーションを行う．
bool
FSIM_CLASSNAME::spsfp(
  const AssignList& assign_list,
  SizeType fid
)
{
  // マルチスレッド版は対応していない．
  return false;
}

// @brief ひとつのパタンで故障シミュレーションを行う．
std::vector<SizeType>
FSIM_CLASSNAME::sppfp(
  const TestVector& tv
)
{
  mSyncObj.put_sppfp_command(tv);

  return merge_det_list();
}

// @brief ひとつのパタンで故障シミュレーションを行う．
std::vector<SizeType>
FSIM_CLASSNAME::sppfp(
  const AssignList& assign_list
)
{
  mSyncObj.put_sppfp_command(assign_list);

  return merge_det_list();
}

// @brief 複数のパタンで故障シミュレーションを行う．
std::vector<std::vector<SizeType>>
FSIM_CLASSNAME::ppsfp(
  const std::vector<TestVector>& tv_list
)
{
  mSyncObj.put_ppsfp_command(tv_list);

  auto ntv = tv_list.size();
  return merge_det_list_array(ntv);
}

// @brief SPSFP故障シミュレーションを行う．
DiffBits
FSIM_CLASSNAME::spsfp2(
  const TestVector& tv,
  SizeType fid
)
{
  // マルチスレッド版は対応していない．
  return DiffBits();
}

// @brief SPSFP故障シミュレーションを行う．
DiffBits
FSIM_CLASSNAME::spsfp2(
  const AssignList& assign_list,
  SizeType fid
)
{
  // マルチスレッド版は対応していない．
  return DiffBits();
}

// @brief ひとつのパタンで故障シミュレーションを行う．
FsimResultsRep*
FSIM_CLASSNAME::sppfp2(
  const TestVector& tv
)
{
  mSyncObj.put_sppfp2_command(tv);

  return merge_diffbits_list();
}

// @brief ひとつのパタンで故障シミュレーションを行う．
FsimResultsRep*
FSIM_CLASSNAME::sppfp2(
  const AssignList& assign_list
)
{
  mSyncObj.put_sppfp2_command(assign_list);

  return merge_diffbits_list();
}

// @brief 複数のパタンで故障シミュレーションを行う．
std::vector<FsimResultsRep*>
FSIM_CLASSNAME::ppsfp2(
  const std::vector<TestVector>& tv_list
)
{

  mSyncObj.put_ppsfp2_command(tv_list);

  auto ntv = tv_list.size();
  return merge_diffbits_list_array(ntv);
}

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

// @brief 各 SimThrFunc の持っている det_list をマージする．
std::vector<SizeType>
FSIM_CLASSNAME::merge_det_list()
{
  // 要素数を数える．
  SizeType n = 0;
  for ( auto& func: mFuncList ) {
    n += func->det_list().size();
  }
  // 結果のリストを用意する．
  std::vector<SizeType> det_list;
  det_list.reserve(n);
  // 結果を作る．
  for ( auto& func: mFuncList ) {
    auto& det_list1 = func->det_list();
    det_list.insert(det_list.end(), det_list1.begin(), det_list1.end());
  }
  return det_list;
}

// @brief 各 SimThrFunc の持っている det_list_array をマージする．
std::vector<std::vector<SizeType>>
FSIM_CLASSNAME::merge_det_list_array(
  SizeType tv_num
)
{
  // 要素数を数える．
  std::vector<SizeType> n_array(tv_num, 0);
  for ( auto& func: mFuncList ) {
    for ( SizeType i = 0; i < tv_num; ++ i ) {
      n_array[i] += func->det_list(i).size();
    }
  }
  // 領域を確保する．
  std::vector<std::vector<SizeType>> det_list_array(tv_num);
  for ( SizeType i = 0; i < tv_num; ++ i ) {
    det_list_array[i].reserve(n_array[i]);
  }
  // 結果を作る．
  for ( auto& func: mFuncList ) {
    for ( SizeType i = 0; i < tv_num; ++ i ) {
      auto& det_list = det_list_array[i];
      auto& det_list1 = func->det_list(i);
      det_list.insert(det_list.end(), det_list1.begin(), det_list1.end());
    }
  }
  return det_list_array;
}

// @brief 各 SimThrFunc の持っている diffbits_list をマージする．
FsimResultsRep*
FSIM_CLASSNAME::merge_diffbits_list()
{
  auto res = new FsimResultsRep;
  for ( auto& func: mFuncList ) {
    auto res1 = func->diffbits_list();
    auto n = res1->det_num();
    for ( SizeType i = 0; i < n; ++ i ) {
      auto fid = res1->fault_id(i);
      auto dbits = res1->diffbits(i);
      res->add(fid, dbits);
    }
    delete res1;
  }
  return res;
}

// @brief 各 SimThrFunc の持っている diffbits_list_array をマージする．
std::vector<FsimResultsRep*>
FSIM_CLASSNAME::merge_diffbits_list_array(
  SizeType tv_num
)
{
  std::vector<FsimResultsRep*> res_list(tv_num);
  for ( SizeType i = 0; i < tv_num; ++ i ) {
    res_list[i] = new FsimResultsRep;
  }
  for ( auto& func: mFuncList ) {
    for ( SizeType i = 0; i < tv_num; ++ i ) {
      auto res1 = func->diffbits_list(i);
      auto n = res1->det_num();
      for ( SizeType j = 0; j < n; ++ j ) {
	auto fid = res1->fault_id(j);
	auto dbits = res1->diffbits(j);
	res_list[i]->add(fid, dbits);
      }
      delete res1;
    }
  }
  return res_list;
}

END_NAMESPACE_DRUID_FSIM
