
/// @file FaultAnalyze.cc
/// @brief FaultAnalyze の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "FaultAnalyze.h"
#include "types/TpgFaultList.h"
#include "FFRAnalyze.h"
#include "Reducer.h"
#include "ym/Timer.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

void
global_reduction(
  FaultInfo& fault_info,
  const ConfigParam& option
)
{
  Timer timer;
  timer.start();

  auto reducer_option = option.get_param("reducer");
  auto verbose = option.get_bool_elem("verbose", false);

  if ( option.get_bool_elem("naive_reduction", false) ) {
    Reducer::naive_run(fault_info, reducer_option);
  }
  else {
    Reducer::run(fault_info, reducer_option);
  }

  timer.stop();
  if ( verbose ) {
    auto nf = fault_info.rep_fault_list().size();
    std::cout << "Global reduction end" << std::endl
	      << " # of faults:            " << nf << std::endl
	      << " CPU time:               " << timer.get_time() << "ms"
	      << std::endl;
  }
}

END_NONAMESPACE

// @brief 故障の解析を行う．
FaultInfo
FaultAnalyze::run(
  const TpgFaultList& fault_list,
  const ConfigParam& option
)
{
  auto fault_info = FaultInfo(fault_list);

  // FFR ごとに故障の検出状況を求める．
  FFRAnalyze::run(fault_info, option);

  // 異なる FFR の故障の間の支配関係を調べる．
  global_reduction(fault_info, option);

  return fault_info;
}

END_NAMESPACE_DRUID
