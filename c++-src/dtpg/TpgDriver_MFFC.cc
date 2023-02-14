
/// @file TpgDriver_MFFC.cc
/// @brief TpgDriver_MFFC の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "TpgDriver_MFFC.h"
#include "DtpgMFFC.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// TpgDriver_MFFC
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
TpgDriver_MFFC::TpgDriver_MFFC(
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
TpgDriver_MFFC::run()
{
  for ( auto mffc: mNetwork.mffc_list() ) {
    DtpgMFFC dtpg{mNetwork, mFaultType, mffc, mJustType, mSolverType};
    for ( auto fault: mffc.fault_list() ) {
      if ( fault_status_mgr().get(fault) == FaultStatus::Undetected ) {
	auto result = dtpg.gen_pattern(fault);
	_update(fault, result);
      }
    }
    _merge_stats(dtpg.stats());
  }
}

END_NAMESPACE_DRUID