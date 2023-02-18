
/// @file DtpgDriver_FFR_se.cc
/// @brief DtpgDriver_FFR_se の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgDriver_FFR_se.h"
#include "StructEnc.h"
#include "TpgNetwork.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// DtpgDriver_FFR_se
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
DtpgDriver_FFR_se::DtpgDriver_FFR_se(
  TpgMgr& mgr,
  const TpgNetwork& network,
  FaultType fault_type,
  const string& just_type,
  const SatSolverType& solver_type
) : DtpgDriver_se{mgr, network, fault_type, just_type, solver_type}
{
}

// @brief テスト生成を行う．
void
DtpgDriver_FFR_se::run()
{
  for ( auto ffr: mNetwork.ffr_list() ) {
    StructEnc enc{mNetwork, mFaultType, mSolverType};
    enc.add_simple_cone(ffr.root(), true);
    enc.make_vars();
    enc.make_cnf();
    for ( auto fault: ffr.fault_list() ) {
      if ( fault_status_mgr().get(fault) == FaultStatus::Undetected ) {
	gen_pattern(enc, fault);
      }
    }
    update_sat_stats(enc.solver().get_stats());
  }
}

END_NAMESPACE_DRUID
