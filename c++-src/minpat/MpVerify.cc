
/// @file MpVerify.cc
/// @brief MpVerify の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "MpVerify.h"
#include "types/TpgNetwork.h"
#include "types/TpgFaultList.h"
#include "types/TestVector.h"
#include "fsim/Fsim.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス MpVerify
//////////////////////////////////////////////////////////////////////

// @brief tv_list が fault_list の全ての故障を検出するか調べる．
TpgFaultList
MpVerify::run(
  const std::vector<TestVector>& tv_list,
  const TpgFaultList& fault_list,
  const ConfigParam& option
)
{
  auto fsim_option = option.get_param("fsim");
  fsim_option.add("has_x", true);
  Fsim fsim(fault_list, fsim_option);
  std::unordered_set<SizeType> det_mark;
  auto network = fault_list.network();
  for ( auto& tv: tv_list ) {
    auto res = fsim.sppfp(tv);
    for ( auto fid: res.fault_list(0) ) {
      det_mark.insert(fid);
      auto fault = network.fault(fid);
      fsim.set_skip(fault);
    }
  }

  TpgFaultList undet_list;
  for ( auto fault: fault_list ) {
    if ( det_mark.count(fault.id()) == 0 ) {
      undet_list.push_back(fault);
    }
  }
  return undet_list;
}

END_NAMESPACE_DRUID
