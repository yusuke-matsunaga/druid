
/// @file Fsim.cc
/// @brief Fsim の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "fsim/Fsim.h"
#include "FsimImpl.h"
#include "types/TpgNetwork.h"
#include "types/TestVector.h"
#include "types/TpgFault.h"
#include "types/TpgFaultList.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// Fsim の実装コード
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
Fsim::Fsim(
  const TpgFaultList& fault_list,
  const ConfigParam& option
) : TpgBase(fault_list)
{
  bool has_x = option.get_bool_elem("has_x", false);
  bool multi = option.get_bool_elem("multi_thread", false);

  auto network = fault_list.network();
  bool has_previous_state = network.fault_type() == FaultType::TransitionDelay;
  if ( multi ) {
    SizeType thread_num = option.get_int_elem("thread_num", 0);
    initialize_multi(fault_list, has_previous_state, has_x, thread_num);
  }
  else {
    initialize_naive(fault_list, has_previous_state, has_x);
  }
}

// @brief デストラクタ
Fsim::~Fsim()
{
}

// @brief 全ての故障にスキップマークをつける．
void
Fsim::set_skip_all()
{
  mImpl->set_skip_all();
}

// @brief 故障にスキップマークをつける．
void
Fsim::set_skip(
  const TpgFault& fault
)
{
  mImpl->set_skip(fault.id());
}

// @brief 複数の故障にスキップマークをつける．
void
Fsim::set_skip(
  const TpgFaultList& fault_list
)
{
  for ( auto fault: fault_list ) {
    set_skip(fault);
  }
}

// @brief 全ての故障のスキップマークを消す．
void
Fsim::clear_skip_all()
{
  mImpl->clear_skip_all();
}

// @brief 故障のスキップマークを消す．
void
Fsim::clear_skip(
  const TpgFault& fault
)
{
  mImpl->clear_skip(fault.id());
}

// @brief 複数の故障のスキップマークを消す．
void
Fsim::clear_skip(
  const TpgFaultList& fault_list
)
{
  set_skip_all();
  for ( auto fault: fault_list ) {
    clear_skip(fault);
  }
}

// @brief 故障のスキップマークを得る．
bool
Fsim::get_skip(
  const TpgFault& fault
) const
{
  return mImpl->get_skip(fault.id());
}

// @brief SPSFP故障シミュレーションを行う．
bool
Fsim::spsfp(
  const TestVector& tv,
  const TpgFault& fault
)
{
  return mImpl->spsfp(tv, fault.id());
}

// @brief SPSFP故障シミュレーションを行う．
bool
Fsim::spsfp(
  const AssignList& assign_list,
  const TpgFault& fault
)
{
  return mImpl->spsfp(assign_list, fault.id());
}

// @brief ひとつのパタンで故障シミュレーションを行う．
TpgFaultList
Fsim::sppfp(
  const TestVector& tv
)
{
  auto fid_list = mImpl->sppfp(tv);
  return TpgBase::fault_list(fid_list);
}

// @brief SPPFPで故障シミュレーションを行う．
std::vector<TpgFaultList>
Fsim::sppfp(
  const std::vector<TestVector>& tv_list
)
{
  auto fid_list_array = mImpl->sppfp(tv_list);

  // 結果を格納するオブジェクトのリスト
  std::vector<TpgFaultList> det_list_array;
  det_list_array.reserve(fid_list_array.size());

  // TpgFaultList に変換する．
  for ( auto& fid_list: fid_list_array ) {
    det_list_array.push_back(TpgBase::fault_list(fid_list));
  }
  return det_list_array;
}

// @brief ひとつのパタンで故障シミュレーションを行う．
TpgFaultList
Fsim::sppfp(
  const AssignList& assign_list
)
{
  auto fid_list = mImpl->sppfp(assign_list);
  return TpgBase::fault_list(fid_list);
}

// @brief 複数のパタンで故障シミュレーションを行う．
std::vector<TpgFaultList>
Fsim::ppsfp(
  const std::vector<TestVector>& tv_list
)
{
  auto fid_list_array = mImpl->ppsfp(tv_list);

  // 結果を格納するオブジェクトのリスト
  std::vector<TpgFaultList> det_list_array;
  det_list_array.reserve(fid_list_array.size());

  // TpgFaultList に変換する．
  for ( auto& fid_list: fid_list_array ) {
    det_list_array.push_back(TpgBase::fault_list(fid_list));
  }
  return det_list_array;
}

// @brief SPSFP故障シミュレーションを行う．
DiffBits
Fsim::spsfp2(
  const TestVector& tv,
  const TpgFault& fault
)
{
  return mImpl->spsfp2(tv, fault.id());
}

// @brief SPSFP故障シミュレーションを行う．
DiffBits
Fsim::spsfp2(
  const AssignList& assign_list,
  const TpgFault& fault
)
{
  return mImpl->spsfp2(assign_list, fault.id());
}

// @brief ひとつのパタンで故障シミュレーションを行う．
FsimResults
Fsim::sppfp2(
  const TestVector& tv
)
{
  auto res = mImpl->sppfp2(tv);
  return FsimResults(_network(), res);
}

// @brief SPPFPで故障シミュレーションを行う．
FsimResults
Fsim::sppfp2(
  const std::vector<TestVector>& tv_list
)
{
  auto res_list = mImpl->sppfp2(tv_list);
  return FsimResults(_network(), res_list);
}

// @brief ひとつのパタンで故障シミュレーションを行う．
FsimResults
Fsim::sppfp2(
  const AssignList& assign_list
)
{
  auto res = mImpl->sppfp2(assign_list);
  return FsimResults(_network(), res);
}

// @brief 複数のパタンで故障シミュレーションを行う．
FsimResults
Fsim::ppsfp2(
  const std::vector<TestVector>& tv_list
)
{
  auto res_list = mImpl->ppsfp2(tv_list);
  return FsimResults(_network(), res_list);
}

// @brief 1クロック分のシミュレーションを行い，遷移回数を数える．
SizeType
Fsim::calc_wsa(
  const TestVector& tv,
  bool weighted
)
{
  return mImpl->calc_wsa(tv, weighted);
}

// @brief 1クロック分のシミュレーションを行い，遷移回数を数える．
SizeType
Fsim::calc_wsa(
  const InputVector& i_vect,
  bool weighted
)
{
  return mImpl->calc_wsa(i_vect, weighted);
}

// @brief 状態を設定する．
void
Fsim::set_state(
  const InputVector& i_vect,
  const DffVector& f_vect
)
{
  mImpl->set_state(i_vect, f_vect);
}

// @brief 状態を取得する．
void
Fsim::get_state(
  InputVector& i_vect,
  DffVector& f_vect
)
{
  mImpl->get_state(i_vect, f_vect);
}

END_NAMESPACE_DRUID
