
/// @file MpReduce.cc
/// @brief MpReduce の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "MpReduce.h"
#include "minpat/FaultAnalyze.h"
#include "types/TpgFaultList.h"


BEGIN_NAMESPACE_DRUID

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
  auto fault_info = FaultAnalyze::run(init_fault_list, option);
  return fault_info.rep_fault_list();
}

END_NAMESPACE_DRUID
