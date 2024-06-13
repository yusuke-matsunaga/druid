
/// @file FFRDriver.cc
/// @brief FFRDriver の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "FFRDriver.h"
#include "FFREngineDriver.h"
#include "FFRStructEncDriver.h"
#include "FFREncDriver.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

DtpgDriverImpl*
gen_impl(
  const TpgNetwork& network,
  const TpgFFR* ffr,
  const JsonValue& option
)
{
  const char* keyword = "driver_type";
  if ( option.has_key(keyword) ) {
    auto value_obj = option.at(keyword);
    if ( value_obj.is_string() ) {
      auto value = value_obj.get_string();
      if ( value == "engine" ) {
	return new FFREngineDriver{network, ffr, option};
      }
      if ( value == "struct_enc" ) {
	return new FFRStructEncDriver{network, ffr, option};
      }
      if ( value == "enc" ) {
	return new FFREncDriver{network, ffr, option};
      }
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
  DtpgMgr& mgr,
  const TpgFFR* ffr,
  const JsonValue& option
) : DtpgDriver{mgr, gen_impl(mgr.network(), ffr, option)}
{
}

END_NAMESPACE_DRUID
