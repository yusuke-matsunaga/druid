
/// @file FFRDriver.cc
/// @brief FFRDriver の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "FFRDriver.h"
#include "FFREngineDriver.h"
#include "FFRStructEncDriver.h"
//#include "FFREnc.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

DtpgDriverImpl*
gen_impl(
  const TpgNetwork& network,
  const TpgFFR* ffr,
  const JsonValue& option
)
{
  auto has_prev_state = network.fault_type() == FaultType::TransitionDelay;
  const char* keyword = "driver_type";
  if ( option.has_key(keyword) ) {
    auto value_obj = option.at(keyword);
    if ( value_obj.is_string() ) {
      auto value = value_obj.get_string();
      if ( value == "engine" ) {
	return new FFREngineDriver{network, ffr, option};
      }
      if ( value == "se" ) {
	return new FFRStructEncDriver{network, ffr, option};
      }
#if 0
      if ( value == "ffr_enc" ) {
	if ( has_prev_state ) {
	  return new FFREncDriver{network, ffr, option};
	}
	return new FFREncBSDriver{network, ffr, option};
      }
#endif
    }
  }
  // デフォルトフォールバック
  return new FFREngineDriver{network, ffr, option};
}

END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// FFRDriver
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
FFRDriver::FFRDriver(
  const TpgNetwork& network,
  const TpgFFR* ffr,
  const JsonValue& option
) : DtpgDriver{gen_impl(network, ffr, option)}
{
}

END_NAMESPACE_DRUID
