
/// @file DtpgDriver.cc
/// @brief DtpgDriver の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023, 2024 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgEngineDriver.h"
#include "StructEncDriver.h"


BEGIN_NAMESPACE_DRUID

DtpgDriver*
DtpgDriver::new_driver(
  DtpgMgr& mgr,
  const TpgNetwork& network,
  const JsonValue& option
)
{
  string dtpg_type;
  string just_type;
  SatInitParam init_param;
  if ( option.is_object() ) {
    if ( option.has_key("dtpg_type") ) {
      auto dtpg_type_obj = option.at("dtpg_type");
      if ( dtpg_type_obj.is_string() ) {
	dtpg_type = dtpg_type_obj.get_string();
      }
      else {
	throw std::invalid_argument{"'dtpg_type' should be a string"};
      }
    }
    if ( option.has_key("just_type") ) {
      auto just_type_obj = option.at("just_type");
      if ( just_type_obj.is_string() ) {
	just_type = just_type_obj.get_string();
      }
      else {
	throw std::invalid_argument{"'just_type' should be a string"};
      }
    }
    if ( option.has_key("sat_param") ) {
      init_param = SatInitParam{option.at("sat_param")};
    }
  }

  if ( dtpg_type == "ffr" ) {
    return new DtpgEngineDriver_FFR{network, just_type, init_param};
  }
  if ( dtpg_type == "mffc" ) {
    return new DtpgEngineDriver_MFFC{network, just_type, init_param};
  }
  if ( dtpg_type == "ffr_se" ) {
    return new StructEncDriver_FFR{network, just_type, init_param};
  }
  if ( dtpg_type == "mffc_se" ) {
    return new StructEncDriver_MFFC{network, just_type, init_param};
  }
  // デフォルトフォールバック
  return new DtpgEngineDriver_FFR{network, just_type, init_param};
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
