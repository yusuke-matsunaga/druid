
/// @file MFFCDriver.cc
/// @brief MFFCDriver の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "MFFCDriver.h"
#include "MFFCEncDriver.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

DtpgDriverImpl*
gen_impl(
  const TpgNetwork& network,
  const TpgMFFC* mffc,
  const JsonValue& option
)
{
  auto has_prev_state = network.fault_type() == FaultType::TransitionDelay;
  const char* keyword = "driver_type";
  if ( option.is_object() && option.has_key(keyword) ) {
    auto value_obj = option.at(keyword);
    if ( value_obj.is_string() ) {
      auto value = value_obj.get_string();
      if ( value == "enc" ) {
	return new MFFCEncDriver{network, mffc, option};
      }
    }
  }
  // デフォルトフォールバック
  return new MFFCEncDriver{network, mffc, option};
}

END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// MFFCDriver
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
MFFCDriver::MFFCDriver(
  DtpgMgr& mgr,
  const TpgMFFC* mffc,
  const JsonValue& option
) : DtpgDriver{mgr, gen_impl(mgr.network(), mffc, option)}
{
}

END_NAMESPACE_DRUID
