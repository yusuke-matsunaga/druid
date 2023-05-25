
/// @file DtpgDriver.cc
/// @brief DtpgDriver の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgEngineDriver.h"
#include "StructEncDriver.h"


BEGIN_NAMESPACE_DRUID

DtpgDriver*
DtpgDriver::new_driver(
  DtpgMgr& mgr,
  const string& dtpg_type,
  const TpgNetwork& network,
  bool has_prev_state,
  const string& just_type,
  const SatInitParam& init_param
)
{
  if ( dtpg_type == "ffr" ) {
    return new DtpgEngineDriver_FFR{mgr, network, has_prev_state, just_type, init_param};
  }
  if ( dtpg_type == "mffc" ) {
    return new DtpgEngineDriver_MFFC{mgr, network, has_prev_state, just_type, init_param};
  }
  if ( dtpg_type == "ffr_se" ) {
    return new StructEncDriver_FFR{mgr, network, has_prev_state, just_type, init_param};
  }
  if ( dtpg_type == "mffc_se" ) {
    return new StructEncDriver_MFFC{mgr, network, has_prev_state, just_type, init_param};
  }
  // デフォルトフォールバック
  return new DtpgEngineDriver_FFR{mgr, network, has_prev_state, just_type, init_param};
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
  return mJustifier(mHasPrevState, assign_list,
		    hvar_map, gvar_map, sat_model);
}

END_NAMESPACE_DRUID
