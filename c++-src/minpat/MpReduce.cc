
/// @file MpReduce.cc
/// @brief MpReduce の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "MpReduce.h"
#include "types/TpgFaultList.h"


BEGIN_NAMESPACE_DRUID

extern
TpgFaultList
ffr_reduction(
  const TpgFaultList& fault_list,
  const ConfigParam& option
);

//////////////////////////////////////////////////////////////////////
// クラス MpReduce
//////////////////////////////////////////////////////////////////////

// @brief 故障の削減を行う．
TpgFaultList
MpReduce::run(
  const TpgFaultList& init_fault_list,
  const ConfigParam& option
)
{
  auto fault_list = init_fault_list;

  auto do_ffr = option.get_bool_elem("ffr", false);
  if ( do_ffr ) {
    fault_list = ffr_reduction(fault_list, option);
  }

  return fault_list;
}

END_NAMESPACE_DRUID
