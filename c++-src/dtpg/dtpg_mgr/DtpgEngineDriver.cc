
/// @file DtpgEngineDriver.cc
/// @brief DtpgEngineDriver の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023, 2024 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgEngineDriver.h"
#include "DtpgEngine.h"
#include "FFREngine.h"
#include "MFFCEngine.h"
#include "TpgFault.h"
#include "TpgNode.h"
#include "NodeValList.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス DtpgEngineDriver
//////////////////////////////////////////////////////////////////////

// @brief テスト生成を行なう．
void
DtpgEngineDriver::gen_pattern(
  DtpgEngine& engine,
  const TpgFault* fault
)
{
  Timer timer;
  timer.start();

  auto ans = engine.solve(fault);

  timer.stop();
  auto sat_time = timer.get_time();

  if ( ans == SatBool3::True ) {
    // パタンが求まった．
    timer.reset();
    timer.start();

    const auto& model = engine.solver().model();
    auto assign_list = engine.get_sufficient_condition(fault);
    auto testvect = justify(assign_list, engine.hvar_map(), engine.gvar_map(), model);

    timer.stop();
    auto backtrace_time = timer.get_time();
    update_det(fault, testvect, sat_time, backtrace_time);
  }
  else if ( ans == SatBool3::False ) {
    // 検出不能と判定された．
    update_untest(fault, sat_time);
  }
  else {
    // ans == SatBool3::X つまりアボート
    update_abort(fault, sat_time);
  }
}


//////////////////////////////////////////////////////////////////////
// DtpgEngineDriver_FFR
//////////////////////////////////////////////////////////////////////

// @brief テスト生成を行う．
void
DtpgEngineDriver_FFR::run()
{
  for ( auto ffr: network().ffr_list() ) {
    FFREngine engine{network(), has_prev_state(), ffr, sat_init_param()};
    cnf_begin();
    engine.make_cnf();
    cnf_end();
    for ( auto fault: fault_mgr().ffr_fault_list(ffr.id()) ) {
      if ( fault_mgr().get_status(fault) == FaultStatus::Undetected ) {
	gen_pattern(engine, fault);
      }
    }
    update_sat_stats(engine.sat_stats());
  }
}


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
      FFREngine engine{network(), has_prev_state(), ffr, sat_init_param()};
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
      MFFCEngine engine{network(), has_prev_state(), mffc, sat_init_param()};
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
