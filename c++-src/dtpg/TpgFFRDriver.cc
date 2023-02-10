
/// @file TpgFFRDriver.cc
/// @brief TpgFFRDriver の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "TpgFFRDriver.h"
#include "DtpgFFR.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// TpgFFRDriver
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
TpgFFRDriver::TpgFFRDriver(
  TpgMgr& mgr,
  const TpgNetwork& network,
  FaultType fault_type,
  const string& just_type,
  const SatSolverType& solver_type
) : TpgDriver{mgr},
    mNetwork{network},
    mFaultType{fault_type},
    mJustType{just_type},
    mSolverType{solver_type}
{
}

// @brief テスト生成を行う．
void
TpgFFRDriver::run()
{
  for ( auto ffr: mNetwork.ffr_list() ) {
    DtpgFFR dtpg(mNetwork, mFaultType, ffr, mJustType, mSolverType);
    for ( auto fault: ffr.fault_list() ) {
      if ( fault_status_mgr().get(fault) == FaultStatus::Undetected ) {
	auto result = dtpg.gen_pattern(fault);
	_update(fault, result);
      }
    }
    _merge_stats(dtpg.stats());
  }
}

END_NAMESPACE_DRUID
