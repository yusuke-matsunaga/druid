
/// @file DtpgDriver_FFR.cc
/// @brief DtpgDriver_FFR の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgDriver_FFR.h"
#include "DtpgFFR.h"
#include "TpgFault.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// DtpgDriver_FFR
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
DtpgDriver_FFR::DtpgDriver_FFR(
  TpgMgr& mgr,
  const TpgNetwork& network,
  FaultType fault_type,
  const string& just_type,
  const SatSolverType& solver_type
) : DtpgDriver{mgr},
    mNetwork{network},
    mFaultType{fault_type},
    mJustType{just_type},
    mSolverType{solver_type}
{
}

// @brief テスト生成を行う．
void
DtpgDriver_FFR::run()
{
  for ( auto ffr: mNetwork.ffr_list() ) {
    DtpgFFR dtpg{mNetwork, mFaultType, ffr, mJustType, mSolverType};
    for ( auto fault: ffr.fault_list() ) {
      if ( fault_status_mgr().get(fault) == FaultStatus::Undetected ) {
	auto result = dtpg.gen_pattern(fault);
	_update(fault, result);
      }
    }
    //_merge_stats(dtpg.stats());
  }
}

END_NAMESPACE_DRUID
