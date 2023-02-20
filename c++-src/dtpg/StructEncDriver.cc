
/// @file StructEncDriver.cc
/// @brief StructEncDriver の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "StructEncDriver.h"
//#include "DtpgResult.h"
#include "StructEnc.h"
#include "TpgFault.h"
#include "ym/Timer.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス StructEncDriver
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
StructEncDriver::StructEncDriver(
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
StructEncDriver::gen_pattern(
  StructEnc& enc,
  const TpgFault* fault
)
{
  Timer timer;
  timer.start();

  // 故障が属している FFR の根のノード
  auto ffr_root = fault->tpg_onode()->ffr_root();

  // FFR より出力側の故障伝搬条件を assumptions に入れる．
  auto assumptions = enc.make_prop_condition(ffr_root);

  // FFR 内の故障伝搬条件を assign_list に入れる．
  auto assign_list = fault->ffr_propagate_condition(fault_type());

  // SAT問題を解く
  auto ans = enc.check_sat(assumptions, assign_list);

  timer.stop();
  auto sat_time = timer.get_time();

  if ( ans == SatBool3::True ) {
    // パタンが求まった．

    timer.reset();
    timer.start();

    const auto& model = enc.solver().model();

    // ffr_root より先の伝搬条件を求める．
    auto assign_list2 = enc.extract_prop_condition(ffr_root);
    assign_list.merge(assign_list2);

    // assign_list の条件を正当化する．
    auto testvect = justify(assign_list, enc.hvar_map(), enc.gvar_map(), model);

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
