
/// @file DtpgDriver_MFFC.cc
/// @brief DtpgDriver_MFFC の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgDriver_MFFC.h"
#include "DtpgMFFC.h"
#include "DtpgFFR.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// DtpgDriver_MFFC
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
DtpgDriver_MFFC::DtpgDriver_MFFC(
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
DtpgDriver_MFFC::run()
{
  for ( auto mffc: mNetwork.mffc_list() ) {
    if ( mffc.ffr_num() == 1 ) {
      auto ffr = mffc.ffr(0);
      DtpgFFR dtpg{mNetwork, mFaultType, ffr, mJustType, mSolverType};
      for ( auto fault: ffr.fault_list() ) {
	if ( fault_status_mgr().get(fault) == FaultStatus::Undetected ) {
	  auto result = dtpg.gen_pattern(fault);
	  _update(fault, result);
	}
      }
      //_merge_stats(dtpg.stats());
    }
    else {
      DtpgMFFC dtpg{mNetwork, mFaultType, mffc, mJustType, mSolverType};
      for ( auto fault: mffc.fault_list() ) {
	if ( fault_status_mgr().get(fault) == FaultStatus::Undetected ) {
	  auto result = dtpg.gen_pattern(fault);
	  _update(fault, result);
	}
      }
      //_merge_stats(dtpg.stats());
    }
  }
}

END_NAMESPACE_DRUID
