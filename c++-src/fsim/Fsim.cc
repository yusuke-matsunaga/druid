
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
  const TpgNetwork& network,
  const TpgFaultList& fault_list,
  const JsonValue& option
) : mNetwork{network}
{
  if ( !TpgBase::check_eq(network, fault_list) ) {
    throw std::invalid_argument{"fault_list does not match with network"};
  }
  bool has_x = get_bool(option, "has_x", false);
  bool multi = get_bool(option, "multi_thread", false);
  bool has_previous_state = network.fault_type() == FaultType::TransitionDelay;
  if ( multi ) {
    initialize_multi(network, fault_list, has_previous_state, has_x);
  }
  else {
    initialize_naive(network, fault_list, has_previous_state, has_x);
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
  const TpgFault& fault,
  DiffBits& dbits
)
{
  return mImpl->xspsfp(assign_list, fault.id(), dbits);
}

// @brief ひとつのパタンで故障シミュレーションを行う．
void
Fsim::sppfp(
  const TestVector& tv,
  cbtype1 callback
)
{
  return mImpl->sppfp(tv,
		      [&](SizeType fid, const DiffBits& dbits){
			auto f = mNetwork.fault(fid);
			callback(f, dbits);
		      });
}

// @brief ひとつのパタンで故障シミュレーションを行う．
void
Fsim::sppfp(
  const AssignList& assign_list,
  cbtype1 callback
)
{
  return mImpl->sppfp(assign_list,
		      [&](SizeType fid, const DiffBits& dbits){
			auto f = mNetwork.fault(fid);
			callback(f, dbits);
		      });
}

// @brief ひとつのパタンで故障シミュレーションを行う．
void
Fsim::xsppfp(
  const AssignList& assign_list,
  cbtype1 callback
)
{
  return mImpl->xsppfp(assign_list,
		      [&](SizeType fid, const DiffBits& dbits){
			auto f = mNetwork.fault(fid);
			callback(f, dbits);
		      });
}

// @brief 複数のパタンで故障シミュレーションを行う．
void
Fsim::ppsfp(
  const std::vector<TestVector>& tv_list,
  cbtype2 callback
)
{
  if ( tv_list.size() > PV_BITLEN ) {
    throw std::invalid_argument{"tv_list.size() > PV_BITLEN"};
  }
  return mImpl->ppsfp(tv_list,
		      [&](SizeType fid, const DiffBitsArray& dbits_array){
			auto f = mNetwork.fault(fid);
			callback(f, dbits_array);
		      });

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
