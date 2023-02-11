
/// @file TpgDriver_MFFC_se.cc
/// @brief TpgDriver_MFFC_se の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "TpgDriver_MFFC_se.h"
#include "Dtpg_se.h"
#include "TpgNetwork.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// TpgDriver_MFFC_se
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
TpgDriver_MFFC_se::TpgDriver_MFFC_se(
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
TpgDriver_MFFC_se::run()
{
  for ( auto mffc: mNetwork.mffc_list() ) {
    Dtpg_se dtpg{mNetwork, mFaultType, mffc, mJustType, mSolverType};
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
