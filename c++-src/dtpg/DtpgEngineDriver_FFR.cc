
/// @file DtpgEngineDriver_FFR.cc
/// @brief DtpgEngineDriver_FFR の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgEngineDriver_FFR.h"
#include "FFREngine.h"
#include "TpgFault.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// DtpgEngineDriver_FFR
//////////////////////////////////////////////////////////////////////

// @brief テスト生成を行う．
void
DtpgEngineDriver_FFR::run()
{
  for ( auto ffr: network().ffr_list() ) {
    FFREngine engine{network(), fault_type(), ffr, sat_type()};
    cnf_begin();
    engine.make_cnf();
    cnf_end();
    for ( auto fault: ffr.fault_list() ) {
      if ( fault_status_mgr().get(fault) == FaultStatus::Undetected ) {
	gen_pattern(engine, fault);
      }
    }
    update_sat_stats(engine.sat_stats());
  }
}

END_NAMESPACE_DRUID
