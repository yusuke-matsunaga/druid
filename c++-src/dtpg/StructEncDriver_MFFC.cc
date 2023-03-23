
/// @file StructEncDriver_MFFC.cc
/// @brief StructEncDriver_MFFC の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "StructEncDriver_MFFC.h"
#include "StructEnc.h"
#include "TpgNetwork.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// StructEncDriver_MFFC
//////////////////////////////////////////////////////////////////////

// @brief テスト生成を行う．
void
StructEncDriver_MFFC::run()
{
  for ( auto mffc: network().mffc_list() ) {
    cnf_begin();
    StructEnc enc{network(), fault_type(), sat_type()};
    enc.add_mffc_cone(mffc, true);
    enc.make_cnf();
    cnf_end();
    for ( auto fault: fault_mgr().mffc_fault_list(mffc.id()) ) {
      if ( fault_mgr().get_status(fault) == FaultStatus::Undetected ) {
	gen_pattern(enc, fault);
      }
    }
    update_sat_stats(enc.solver().get_stats());
  }
}

END_NAMESPACE_DRUID
