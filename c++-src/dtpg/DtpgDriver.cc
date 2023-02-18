
/// @file DtpgDriver.cc
/// @brief DtpgDriver の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgDriver.h"
#include "DtpgDriver_FFR.h"
#include "DtpgDriver_MFFC.h"
#include "DtpgDriver_FFR_se.h"
#include "DtpgDriver_MFFC_se.h"


BEGIN_NAMESPACE_DRUID

DtpgDriver*
DtpgDriver::new_driver(
  TpgMgr& mgr,
  const string& dtpg_type,
  const TpgNetwork& network,
  FaultType fault_type,
  const string& just_type,
  const SatSolverType& solver_type
)
{
  if ( dtpg_type == "ffr" ) {
    return new DtpgDriver_FFR{mgr, network, fault_type, just_type, solver_type};
  }
  if ( dtpg_type == "mffc" ) {
    return new DtpgDriver_MFFC{mgr, network, fault_type, just_type, solver_type};
  }
  if ( dtpg_type == "ffr_se" ) {
    return new DtpgDriver_FFR_se{mgr, network, fault_type, just_type, solver_type};
  }
  if ( dtpg_type == "mffc_se" ) {
    return new DtpgDriver_MFFC_se{mgr, network, fault_type, just_type, solver_type};
  }
  // デフォルトフォールバック
  return new DtpgDriver_FFR{mgr, network, fault_type, just_type, solver_type};
}

// @brief テストパタン生成が成功した時の結果を更新する．
void
DtpgDriver::update_det(
  const TpgFault* fault,
  const TestVector& tv,
  double sat_time,
  double backtrace_time
)
{
  mgr().update_det(fault, tv, sat_time, backtrace_time);
}

// @brief 冗長故障の特定が行えた時の結果を更新する．
void
DtpgDriver::update_untest(
  const TpgFault* fault,
  double sat_time
)
{
  mgr().update_untest(fault, sat_time);
}

// @brief アボートした時の結果を更新する．
void
DtpgDriver::update_abort(
  const TpgFault* fault,
  double sat_time
)
{
  mgr().update_abort(fault, sat_time);
}

// @brief SATの統計情報を更新する．
void
DtpgDriver::update_sat_stats(
  const SatStats& sat_stats
)
{
  mgr().update_sat_stats(sat_stats);
}

void
DtpgDriver::_update(
  const TpgFault* fault,
  const DtpgResult& result
)
{
  mgr()._update(fault, result);
}

// @breif DTPG の統計情報をマージする．
void
DtpgDriver::_merge_stats(
  const DtpgStats& stats
)
{
  mgr()._merge_stats(stats);
}

END_NAMESPACE_DRUID
