
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
  DiffBits _;
  return mImpl->spsfp(tv, fault.id(), _);
}

// @brief SPSFP故障シミュレーションを行う．
bool
Fsim::spsfp(
  const TestVector& tv,
  const TpgFault& fault,
  DiffBits& dbits
)
{
  return mImpl->spsfp(tv, fault.id(), dbits);
}

// @brief SPSFP故障シミュレーションを行う．
bool
Fsim::spsfp(
  const AssignList& assign_list,
  const TpgFault& fault
)
{
  DiffBits _;
  return mImpl->spsfp(assign_list, fault.id(), _);
}

// @brief SPSFP故障シミュレーションを行う．
bool
Fsim::spsfp(
  const AssignList& assign_list,
  const TpgFault& fault,
  DiffBits& dbits
)
{
  return mImpl->spsfp(assign_list, fault.id(), dbits);
}

// @brief SPSFP故障シミュレーションを行う．
bool
Fsim::xspsfp(
  const AssignList& assign_list,
  const TpgFault& fault
)
{
  DiffBits _;
  return mImpl->xspsfp(assign_list, fault.id(), _);
}

// @brief SPSFP故障シミュレーションを行う．
bool
Fsim::xspsfp(
  const AssignList& assign_list,
  const TpgFault& fault,
  DiffBits& dbits
)
{
  return mImpl->xspsfp(assign_list, fault.id(), dbits);
}

// @brief ひとつのパタンで故障シミュレーションを行う．
FsimResults
Fsim::sppfp(
  const TestVector& tv
)
{
  auto res = mImpl->sppfp(tv);
  return FsimResults(_network(), res);
}

// @brief ひとつのパタンで故障シミュレーションを行う．
FsimResults
Fsim::sppfp(
  const AssignList& assign_list
)
{
  auto res = mImpl->sppfp(assign_list);
  return FsimResults(_network(), res);
}

// @brief ひとつのパタンで故障シミュレーションを行う．
FsimResults
Fsim::xsppfp(
  const AssignList& assign_list
)
{
  auto res = mImpl->xsppfp(assign_list);
  return FsimResults(_network(), res);
}

// @brief 複数のパタンで故障シミュレーションを行う．
FsimResults
Fsim::ppsfp(
  const std::vector<TestVector>& tv_list
)
{
  SizeType NV = tv_list.size();

  // 結果を格納するオブジェクトのリスト
  std::vector<std::shared_ptr<FsimResultsRep>> res_list;
  res_list.reserve(NV);

  // PV_BITLEN ごとに分割して処理を行う．
  std::vector<TestVector> tv_buff;
  tv_buff.reserve(PV_BITLEN);
  SizeType base = 0;
  for ( auto& tv: tv_list ) {
    tv_buff.push_back(tv);
    if ( tv_buff.size() == PV_BITLEN || tv_buff.size() + base == NV )  {
      auto res_list1 = mImpl->ppsfp(tv_buff);
      res_list.insert(res_list.end(), res_list1.begin(), res_list1.end());
      base += tv_buff.size();
      tv_buff.clear();
    }
  }
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

// @brief 1クロック分のシミュレーションを行い，遷移回数を数える．
SizeType
Fsim::calc_wsa(
  const InputVector& i_vect,
  bool weighted
)
{
  return mImpl->calc_wsa(i_vect, weighted);
}

END_NAMESPACE_DRUID
