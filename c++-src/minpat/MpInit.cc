
/// @file MpInit.cc
/// @brief MpInit の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "MpInit.h"
#include "dtpg/DtpgMgr.h"
#include "ym/Timer.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
MpInit::MpInit(
  const TpgNetwork& network,
  const TpgFaultList& fault_list,
  const ConfigParam& option
) : mNetwork{network}
{
  Timer timer;
  timer.start();

  // 検出可能な故障を求める．
  auto res = DtpgMgr::run(fault_list, option);
  mTvList.reserve(fault_list.size());
  for ( auto fault: fault_list ) {
    auto status = res.status(fault);
    switch ( status ) {
    case FaultStatus::Detected:
      add_det_fault(fault);
      mTvList.push_back(res.testvector(fault));
      break;
    case FaultStatus::Untestable:
      add_untest_fault(fault);
      break;
    case FaultStatus::Undetected:
      add_undet_fault(fault);
      break;
    }
  }

  timer.stop();
  {
    std::cout << std::left << std::setw(20)
	      << "DTPG end: " << timer.get_time() << std::endl
	      << std::left << std::setw(20)
	      << "# of initial faults: " << det_fault_list().size() << std::endl;
  }
}

END_NAMESPACE_DRUID
