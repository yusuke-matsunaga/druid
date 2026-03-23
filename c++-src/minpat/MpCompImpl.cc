
/// @file MpCompImpl.cc
/// @brief MpCompImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "MpCompImpl.h"
#include "MpComp_Simple.h"
#include "types/TpgNetwork.h"
#include "types/TpgFaultList.h"
#include "fsim/Fsim.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス MpCompImpl
//////////////////////////////////////////////////////////////////////

// @brief 派生クラスのオブジェクトを生成するクラスメソッド
std::unique_ptr<MpCompImpl>
MpCompImpl::new_obj(
  const ConfigParam& option
)
{
  auto type = option.get_string_elem("type", "simple");
  if ( type == "simple" ) {
    return std::unique_ptr<MpCompImpl>{new MpComp_Simple};
  }
  std::ostringstream buf;
  buf << type << ": Unknown type name";
  throw std::invalid_argument{buf.str()};
}

END_NAMESPACE_DRUID
