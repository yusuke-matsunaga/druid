
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

// @brief コンストラクタ
DtpgEngineDriver::DtpgEngineDriver(
  TpgMgr& mgr,
  const TpgNetwork& network,
  FaultType fault_type,
  const string& just_type,
  const SatSolverType& solver_type
) : DtpgDriver{mgr, network, fault_type, just_type, solver_type}
{
}

// @brief テスト生成を行なう．
void
DtpgEngineDriver::gen_pattern(
  DtpgEngine& engine,
  const TpgFault* fault
)
{
  Timer timer;
  timer.start();

  // 故障が属している FFR の根のノード
  auto ffr_root = fault->tpg_onode()->ffr_root();

  // fault の活性化条件を求める．
  auto assumptions = engine.gen_assumptions(fault);

  // FFR 内の故障伝搬条件を求める．
  auto assign_list = fault->ffr_propagate_condition(fault_type());

  engine.add_to_literal_list(assign_list, assumptions);

  // SAT問題を解く
  auto ans = engine.check(assumptions);

  timer.stop();
  auto sat_time = timer.get_time();

  if ( ans == SatBool3::True ) {
    // パタンが求まった．

    timer.reset();
    timer.start();


    const auto& model = engine.solver().model();

    // ffr_root より先の伝搬条件を求める．
    auto assign_list2 = engine.get_sufficient_condition(ffr_root);
    assign_list.merge(assign_list2);

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
