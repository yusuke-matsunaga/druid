
/// @file TpgDriver_FFR.cc
/// @brief TpgDriver_FFR の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "TpgDriver_FFR.h"
#include "DtpgFFR.h"
#include "TpgFault.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// TpgDriver_FFR
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
TpgDriver_FFR::TpgDriver_FFR(
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
TpgDriver_FFR::run()
{
  for ( auto ffr: mNetwork.ffr_list() ) {
    bool found = false;
    for ( auto fault: ffr.fault_list() ) {
      if ( fault->str() == "R2358_U435:I2:1" ) {
	found = true;
	break;
      }
    }
    if ( !found ) {
      continue;
    }
    DtpgFFR dtpg{mNetwork, mFaultType, ffr, mJustType, mSolverType};
    for ( auto fault: ffr.fault_list() ) {
      if ( fault_status_mgr().get(fault) == FaultStatus::Undetected ) {
	cout << fault << endl;
	auto result = dtpg.gen_pattern(fault);
	_update(fault, result);
	cout << endl;
      }
    }
    _merge_stats(dtpg.stats());
  }
}

END_NAMESPACE_DRUID
