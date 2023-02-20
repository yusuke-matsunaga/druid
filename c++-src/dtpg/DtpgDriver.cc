
/// @file DtpgDriver.cc
/// @brief DtpgDriver の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgDriver.h"
#include "DtpgEngineDriver_FFR.h"
#include "DtpgEngineDriver_MFFC.h"
#include "StructEncDriver_FFR.h"
#include "StructEncDriver_MFFC.h"


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
    return new DtpgEngineDriver_FFR{mgr, network, fault_type, just_type, solver_type};
  }
  if ( dtpg_type == "mffc" ) {
    return new DtpgEngineDriver_MFFC{mgr, network, fault_type, just_type, solver_type};
  }
  if ( dtpg_type == "ffr_se" ) {
    return new StructEncDriver_FFR{mgr, network, fault_type, just_type, solver_type};
  }
  if ( dtpg_type == "mffc_se" ) {
    return new StructEncDriver_MFFC{mgr, network, fault_type, just_type, solver_type};
  }
  // デフォルトフォールバック
  return new DtpgEngineDriver_FFR{mgr, network, fault_type, just_type, solver_type};
}

// @brief 正当化を行う．
TestVector
DtpgDriver::justify(
  const NodeValList& assign_list,
  const VidMap& hvar_map,
  const VidMap& gvar_map,
  const SatModel& sat_model
)
{
  return mJustifier(mFaultType, assign_list,
		    hvar_map, gvar_map, sat_model);
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
