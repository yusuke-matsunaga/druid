
/// @file TpgDriver.cc
/// @brief TpgDriver の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "TpgDriver.h"
#include "TpgFFRDriver.h"
#include "TpgMFFCDriver.h"


BEGIN_NAMESPACE_DRUID

TpgDriver*
TpgDriver::new_driver(
  TpgMgr& mgr,
  const string& dtpg_type,
  const TpgNetwork& network,
  FaultType fault_type,
  const string& just_type,
  const SatSolverType& solver_type
)
{
  if ( dtpg_type == "ffr" ) {
    return new TpgFFRDriver{mgr, network, fault_type, just_type, solver_type};
  }
  if ( dtpg_type == "mffc" ) {
    return new TpgMFFCDriver{mgr, network, fault_type, just_type, solver_type};
  }
  // デフォルトフォールバック
  return new TpgFFRDriver{mgr, network, fault_type, just_type, solver_type};
}

// @brief テストパタン生成の結果を更新する．
void
TpgDriver::_update(
  const TpgFault* fault,
  const DtpgResult& result
)
{
  mgr()._update(fault, result);
}

// @breif DTPG の統計情報をマージする．
void
TpgDriver::_merge_stats(
  const DtpgStats& stats
)
{
  mgr()._merge_stats(stats);
}

END_NAMESPACE_DRUID
