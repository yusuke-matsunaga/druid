
/// @file StructEncDriver_FFR.cc
/// @brief StructEncDriver_FFR の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "StructEncDriver_FFR.h"
#include "StructEnc.h"
#include "TpgNetwork.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// StructEncDriver_FFR
//////////////////////////////////////////////////////////////////////

// @brief テスト生成を行う．
void
StructEncDriver_FFR::run()
{
  for ( auto ffr: network().ffr_list() ) {
    cnf_begin();
    StructEnc enc{network(), fault_type(), sat_type()};
    enc.add_simple_cone(ffr.root(), true);
    enc.make_cnf();
    cnf_end();
    for ( auto fault: fault_mgr().ffr_fault_list(ffr.id()) ) {
      if ( fault_mgr().get_status(fault) == FaultStatus::Undetected ) {
	gen_pattern(enc, fault);
      }
    }
    update_sat_stats(enc.solver().get_stats());
  }
}

END_NAMESPACE_DRUID
