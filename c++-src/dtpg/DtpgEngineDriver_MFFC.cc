
/// @file DtpgEngineDriver_MFFC.cc
/// @brief DtpgEngineDriver_MFFC の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgEngineDriver_MFFC.h"
#include "FFREngine.h"
#include "MFFCEngine.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// DtpgEngineDriver_MFFC
//////////////////////////////////////////////////////////////////////

// @brief テスト生成を行う．
void
DtpgEngineDriver_MFFC::run()
{
  for ( auto mffc: network().mffc_list() ) {
    if ( mffc.ffr_num() == 1 ) {
      auto ffr = mffc.ffr(0);
      FFREngine engine{network(), fault_type(), ffr, sat_type()};
      cnf_begin();
      engine.make_cnf();
      cnf_end();
      for ( auto fault: fault_mgr().ffr_fault_list(ffr.id()) ) {
	if ( fault_mgr().get_status(fault) == FaultStatus::Undetected ) {
	  gen_pattern(engine, fault);
	}
      }
      update_sat_stats(engine.solver().get_stats());
    }
    else {
      MFFCEngine engine{network(), fault_type(), mffc, sat_type()};
      cnf_begin();
      engine.make_cnf();
      cnf_end();
      for ( auto fault: fault_mgr().mffc_fault_list(mffc.id()) ) {
	if ( fault_mgr().get_status(fault) == FaultStatus::Undetected ) {
	  gen_pattern(engine, fault);
	}
      }
      update_sat_stats(engine.solver().get_stats());
    }
  }
}

END_NAMESPACE_DRUID
