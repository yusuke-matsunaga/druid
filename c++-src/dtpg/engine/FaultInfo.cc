
/// @file FaultInfo.cc
/// @brief FaultInfo の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "dtpg/FaultInfo.h"
#include "types/TpgFault.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

inline
SizeType
get_max_id(
  const TpgFaultList& fault_list
)
{
  SizeType max_id = 0;
  for ( auto fault: fault_list ) {
    max_id = std::max(max_id, fault.id());
  }
  return max_id + 1;
}

END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// クラス FaultInfo
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
FaultInfo::FaultInfo(
  const TpgFaultList& fault_list
) : mFaultList{fault_list},
    mCellArray(get_max_id(fault_list))
{
}

// @brief 対象のネットワークを返す．
TpgNetwork
FaultInfo::network() const
{
  return mFaultList.network();
}

// @brief 検出可能で他の故障に支配されていない故障のリストを返す．
TpgFaultList
FaultInfo::rep_fault_list() const
{
  TpgFaultList fault_list;
  for ( auto fault: mFaultList ) {
    if ( is_rep(fault) ) {
      fault_list.push_back(fault);
    }
  }
  return fault_list;
}

// @brief 故障の検出状況を調べる．
FaultStatus
FaultInfo::status(
  const TpgFault& fault
) const
{
  auto& c = _cell(fault);
  return c.status;
}

// @brief 検出可能で他の故障に支配されていない時 true を返す．
bool
FaultInfo::is_rep(
  const TpgFault& fault
) const
{
  auto& c = _cell(fault);
  return c.status == FaultStatus::Detected && !c.dominator.is_valid();
}

// @brief 支配されている故障の時 true を返す．
bool
FaultInfo::is_dominated(
  const TpgFault& fault
) const
{
  return dominator(fault).is_valid();
}

// @brief 支配されている故障の時 true を返す．
TpgFault
FaultInfo::dominator(
  const TpgFault& fault
) const
{
  auto& c = _cell(fault);
  return c.dominator;
}

// @brief 検出可能な場合の検出条件(拡張テストキューブ)を求める．
const AssignList&
FaultInfo::detect_cond(
  const TpgFault& fault
) const
{
  auto& c = _cell(fault);
  return c.detect_cond;
}

// @brief 検出可能な場合の必須条件(拡張テストキューブ)を求める．
const AssignList&
FaultInfo::mandatory_cond(
  const TpgFault& fault
) const
{
  auto& c = _cell(fault);
  return c.mandatory_cond;
}

// @brief 検出可能な場合のテストキューブを求める．
const TestVector&
FaultInfo::testvector(
  const TpgFault& fault
) const
{
  auto& c = _cell(fault);
  return c.testvector;
}

// @brief 検出情報をセットする．
void
FaultInfo::set_detected(
  const TpgFault& fault,
  const AssignList& detect_condition,
  const TestVector& tv
)
{
  auto& c = _cell(fault);
  c.status = FaultStatus::Detected;
  c.detect_cond = detect_condition;
  c.testvector = tv;
}

// @brief 検出の必須条件をセットする．
void
FaultInfo::set_mandatory_condition(
  const TpgFault& fault,
  const AssignList& cond
)
{
  auto& c = _cell(fault);
  c.mandatory_cond = cond;
}

// @brief 検出不能の情報をセットする．
void
FaultInfo::set_untestable(
  const TpgFault& fault
)
{
  auto& c = _cell(fault);
  c.status = FaultStatus::Untestable;
}

// @brief 支配故障をセットする．
void
FaultInfo::set_dominator(
  const TpgFault& fault,
  const TpgFault& dominator
)
{
  auto& c = _cell(fault);
  c.dominator = dominator;
}

// 故障に対応する Cell を取り出す．
const FaultInfo::Cell&
FaultInfo::_cell(
  const TpgFault& fault
) const
{
  auto fid = fault.id();
  _check_fid(fid);
  return mCellArray[fid];
}

// 故障に対応する Cell を取り出す．
FaultInfo::Cell&
FaultInfo::_cell(
  const TpgFault& fault
)
{
  auto fid = fault.id();
  _check_fid(fid);
  return mCellArray[fid];
}

// @brief 故障番号のチェックを行う．
void
FaultInfo::_check_fid(
  SizeType fid
) const
{
  if ( fid >= mCellArray.size() ) {
    throw std::out_of_range{"fault is not registered"};
  }
}

END_NAMESPACE_DRUID
