
/// @file MpInit_Naive.cc
/// @brief MpInit_Naive の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "MpInit_Naive.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス MpInit_Naive
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
MpInit_Naive::MpInit_Naive(
  const TpgNetwork& network
) : MpInit(network)
{
}

// @brief 対象の故障リストとテストパタンのリストを求める．
std::pair<TpgFaultList, std::vector<TestVector>>
MpInit_Naive::run(
  const TpgFaultList& fault_list,
  const JsonValue& option
)
{
  auto tv_list = init(fault_list, option);
  return std::make_pair(det_fault_list(), tv_list);
}

END_NAMESPACE_DRUID
