
/// @file MpInit.cc
/// @brief MpInit の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "MpInit.h"
#include "MpInit_Naive.h"


BEGIN_NAMESPACE_DRUID

// @brief 派生クラスのオブジェクトを生成するクラスメソッド
std::unique_ptr<MpInit>
MpInit::new_obj(
  const std::string& type,
  const TpgNetwork& network
)
{
  if ( type == "naive" ) {
    return std::unique_ptr<MpInit>{new MpInit_Naive(network)};
  }
  std::ostringstream buf;
  buf << type << ": Unknown type name";
  throw std::invalid_argument{buf.str()};
}

END_NAMESPACE_DRUID
