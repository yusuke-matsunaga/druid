
/// @file NodeDriver.cc
/// @brief NodeDriver の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "NodeDriver.h"
#include "NodeEncDriver.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

DtpgDriverImpl*
gen_impl(
  const TpgNetwork& network,
  const TpgNode* node,
  const JsonValue& option
)
{
  const char* keyword = "driver_type";
  if ( option.has_key(keyword) ) {
    auto value_obj = option.at(keyword);
    if ( value_obj.is_string() ) {
      auto value = value_obj.get_string();
      if ( value == "enc" ) {
	return new NodeEncDriver{network, node, option};
      }
    }
  }
  // デフォルトフォールバック
  return new NodeEncDriver{network, node, option};
}

END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// NodeDriver
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
NodeDriver::NodeDriver(
  DtpgMgr& mgr,
  const TpgNode* node,
  const JsonValue& option
) : DtpgDriver(mgr, gen_impl(mgr.network(), node, option))
{
}

END_NAMESPACE_DRUID
