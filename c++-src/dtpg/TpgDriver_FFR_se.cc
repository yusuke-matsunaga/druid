
/// @file TpgDriver_FFR_se.cc
/// @brief TpgDriver_FFR_se の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "TpgDriver_FFR_se.h"
#include "Dtpg_se.h"
#include "TpgNetwork.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// TpgDriver_FFR_se
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
TpgDriver_FFR_se::TpgDriver_FFR_se(
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
TpgDriver_FFR_se::run()
{
  for ( auto ffr: mNetwork.ffr_list() ) {
    Dtpg_se dtpg{mNetwork, mFaultType, ffr, mJustType, mSolverType};
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
