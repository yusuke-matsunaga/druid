
/// @file DtpgEngineDriver.cc
/// @brief DtpgEngineDriver の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgEngineDriver.h"
#include "DtpgEngine.h"
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

    auto assign_list = engine.get_sufficient_condition(fault);
    const auto& model = engine.solver().model();
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

END_NAMESPACE_DRUID
