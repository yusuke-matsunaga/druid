
/// @file FaultGroup.cc
/// @brief FaultGroup の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "FaultGroup.h"
#include "fsim/Fsim.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス FaultGroup
//////////////////////////////////////////////////////////////////////

// @brief テストベクタのリストから FaultGroup のリストを作る．
std::vector<FaultGroup>
FaultGroup::make_list(
  const std::vector<TestVector>& tv_list,
  const TpgFaultList& fault_list
)
{
  auto fsim_option = JsonValue::object();
  fsim_option.add("has_x", true);
  Fsim fsim(fault_list, fsim_option);

  auto network = fault_list.network();

  auto ntv = tv_list.size();
  std::vector<FaultGroup> fg_list;
  fg_list.reserve(ntv);
  for ( auto& tv: tv_list ) {
    auto res = fsim.sppfp(tv);
    auto fault_list = network.fault_list(res.fault_list(0));
    fg_list.push_back(FaultGroup(fault_list, tv));
  }
  return fg_list;
}

END_NAMESPACE_DRUID
