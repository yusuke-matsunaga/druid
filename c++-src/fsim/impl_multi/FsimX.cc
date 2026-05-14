
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
) : mThreadNum{thread_num}
{
  auto network = fault_list.network();

  mFuncList.reserve(thread_num);
  for ( SizeType i = 0; i < thread_num; ++ i ) {
    auto func = new SimThrFunc(i, network, fault_list);
    mFuncList.push_back(std::unique_ptr<SimThrFunc>{func});
  }
}

// @brief デストラクタ
FSIM_CLASSNAME::~FSIM_CLASSNAME()
{
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
  // 実はシングルスレッドで実行する．
  auto func = mFuncList.front().get();
  func->engine().calc_val(tv);
  return func->engine().spsfp(fid);
}

// @brief SPSFP故障シミュレーションを行う．
bool
FSIM_CLASSNAME::spsfp(
  const AssignList& assign_list,
  SizeType fid
)
{
  // 実はシングルスレッドで実行する．
  auto func = mFuncList.front().get();
  func->engine().calc_val(assign_list);
  return func->engine().spsfp(fid);
}

// @brief ひとつのパタンで故障シミュレーションを行う．
std::vector<SizeType>
FSIM_CLASSNAME::sppfp(
  const TestVector& tv
)
{
  // 実はシングルスレッドで実行する．
  auto func = mFuncList.front().get();
  return func->sppfp(tv);
}

// @brief 複数のパタンで故障シミュレーションを行う．
std::vector<std::vector<SizeType>>
FSIM_CLASSNAME::sppfp(
  const std::vector<TestVector>& tv_list
)
{
  // 全パタン数
  auto ntv = tv_list.size();
  // 実際のスレッド数
  SizeType act_num = std::min(ntv, mThreadNum);
  std::vector<std::thread> thr_list(act_num);
  // スレッド当たりのパタン数
  auto pat_size = (ntv + act_num - 1) / act_num;
  // 結果を格納するオブジェクト
  std::vector<std::vector<SizeType>> det_list_array(ntv);
  for ( SizeType i = 0; i < act_num; ++ i ) {
    auto func = mFuncList[i].get();
    SizeType begin = pat_size * i;
    SizeType end = std::min(pat_size * (i + 1), ntv);
    thr_list[i] = std::thread{
      [&](SimThrFunc* func,
	  const std::vector<TestVector>& tv_list,
	  SizeType begin, SizeType end) {
	func->sppfp(tv_list, begin, end, det_list_array);
      },
      func, tv_list, begin, end
    };
  }
  for ( auto& thr: thr_list ) {
    thr.join();
  }
  return det_list_array;
}

// @brief ひとつのパタンで故障シミュレーションを行う．
std::vector<SizeType>
FSIM_CLASSNAME::sppfp(
  const AssignList& assign_list
)
{
  // 実はシングルスレッドで実行する．
  auto func = mFuncList.front().get();
  return func->sppfp(assign_list);
}

// @brief 複数のパタンで故障シミュレーションを行う．
std::vector<std::vector<SizeType>>
FSIM_CLASSNAME::ppsfp(
  const std::vector<TestVector>& tv_list
)
{
  // 全パタン数
  auto ntv = tv_list.size();
  // ブロック数
  auto blk_num = (ntv + PV_BITLEN - 1) / PV_BITLEN;
  // 実際のスレッド数
  SizeType act_num = std::min(blk_num, mThreadNum);
  std::vector<std::thread> thr_list(act_num);
  // スレッド当たりのパタン数
  auto pat_size = (ntv + act_num - 1) / act_num;
  // 結果を格納するオブジェクト
  std::vector<std::vector<SizeType>> det_list_array(ntv);
  for ( SizeType i = 0; i < act_num; ++ i ) {
    auto func = mFuncList[i].get();
    SizeType begin = pat_size * i;
    SizeType end = std::min(pat_size * (i + 1), ntv);
    thr_list[i] = std::thread{
      [&](SimThrFunc* func,
	  const std::vector<TestVector>& tv_list,
	  SizeType begin, SizeType end) {
	func->ppsfp(tv_list, begin, end, det_list_array);
      },
      func, tv_list, begin, end
    };
  }
  for ( auto& thr: thr_list ) {
    thr.join();
  }
  return det_list_array;
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
  // 実はシングルスレッドで実行する．
  auto func = mFuncList.front().get();
  return func->sppfp2(tv);
}

// @brief ひとつのパタンで故障シミュレーションを行う．
std::vector<FsimResultsRep*>
FSIM_CLASSNAME::sppfp2(
  const std::vector<TestVector>& tv_list
)
{
  // 全パタン数
  auto ntv = tv_list.size();
  // 実際のスレッド数
  SizeType act_num = std::min(ntv, mThreadNum);
  std::vector<std::thread> thr_list(act_num);
  // スレッド当たりのパタン数
  auto pat_size = (ntv + act_num - 1) / act_num;
  // 結果を格納するオブジェクト
  std::vector<FsimResultsRep*> res_array(ntv);
  for ( SizeType i = 0; i < act_num; ++ i ) {
    auto func = mFuncList[i].get();
    SizeType begin = pat_size * i;
    SizeType end = std::min(pat_size * (i + 1), ntv);
    thr_list[i] = std::thread{
      [&](SimThrFunc* func,
	  const std::vector<TestVector>& tv_list,
	  SizeType begin, SizeType end) {
	func->sppfp2(tv_list, begin, end, res_array);
      },
      func, tv_list, begin, end
    };
  }
  for ( auto& thr: thr_list ) {
    thr.join();
  }
  return res_array;
}

// @brief ひとつのパタンで故障シミュレーションを行う．
FsimResultsRep*
FSIM_CLASSNAME::sppfp2(
  const AssignList& assign_list
)
{
  // 実はシングルスレッドで実行する．
  auto func = mFuncList.front().get();
  return func->sppfp2(assign_list);
}

// @brief 複数のパタンで故障シミュレーションを行う．
std::vector<FsimResultsRep*>
FSIM_CLASSNAME::ppsfp2(
  const std::vector<TestVector>& tv_list
)
{
  // 全パタン数
  auto ntv = tv_list.size();
  // ブロック数
  auto blk_num = (ntv + PV_BITLEN - 1) / PV_BITLEN;
  // 実際のスレッド数
  SizeType act_num = std::min(blk_num, mThreadNum);
  std::vector<std::thread> thr_list(act_num);
  // スレッド当たりのパタン数
  auto pat_size = (ntv + act_num - 1) / act_num;
  // 結果を格納するオブジェクト
  std::vector<FsimResultsRep*> res_array(ntv);
  for ( SizeType i = 0; i < act_num; ++ i ) {
    auto func = mFuncList[i].get();
    SizeType begin = pat_size * i;
    SizeType end = std::min(pat_size * (i + 1), ntv);
    thr_list[i] = std::thread{
      [&](SimThrFunc* func,
	  const std::vector<TestVector>& tv_list,
	  SizeType begin, SizeType end) {
	func->ppsfp2(tv_list, begin, end, res_array);
      },
      func, tv_list, begin, end
    };
  }
  for ( auto& thr: thr_list ) {
    thr.join();
  }
  return res_array;
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

END_NAMESPACE_DRUID_FSIM
