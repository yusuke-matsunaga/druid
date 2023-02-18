
/// @file DtpgDriver_MFFC_se.cc
/// @brief DtpgDriver_MFFC_se の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgDriver_MFFC_se.h"
#include "StructEnc.h"
#include "TpgNetwork.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// DtpgDriver_MFFC_se
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
DtpgDriver_MFFC_se::DtpgDriver_MFFC_se(
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
DtpgDriver_MFFC_se::run()
{
  for ( auto mffc: mNetwork.mffc_list() ) {
    cnf_begin();
    StructEnc enc{mNetwork, mFaultType, mSolverType};
    if ( mffc.ffr_num() > 1 ) {
      enc.add_mffc_cone(mffc, true);
    }
    else {
      enc.add_simple_cone(mffc.root(), true);
    }
    enc.make_vars();
    enc.make_cnf();
    cnf_end();
    for ( auto fault: mffc.fault_list() ) {
      if ( fault_status_mgr().get(fault) == FaultStatus::Undetected ) {
	gen_pattern(enc, fault);
      }
    }
    update_sat_stats(enc.solver().get_stats());
  }
}

END_NAMESPACE_DRUID
