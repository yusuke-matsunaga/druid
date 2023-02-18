
/// @file DtpgDriver_se.cc
/// @brief DtpgDriver_se の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgDriver_se.h"
#include "DtpgResult.h"
#include "StructEnc.h"
#include "TpgFault.h"
#include "ym/Timer.h"
//#include "TpgNetwork.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス DtpgDriver_se
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
DtpgDriver_se::DtpgDriver_se(
  TpgMgr& mgr,
  const TpgNetwork& network,
  FaultType fault_type,
  const string& just_type,
  const SatSolverType& solver_type
) : DtpgDriver{mgr},
    mNetwork{network},
    mFaultType{fault_type},
    mJustifier{just_type, network},
    mSolverType{solver_type}
{
}

// @brief テスト生成を行なう．
void
DtpgDriver_se::gen_pattern(
  StructEnc& enc,
  const TpgFault* fault
)
{
  Timer timer;
  timer.start();

  // 故障が属している FFR の根のノード
  auto ffr_root = fault->tpg_onode()->ffr_root();

  // FFR より出力側の故障伝搬条件を assumptions に入れる．
  auto assumptions = enc.make_prop_condition(ffr_root, 0);

  // FFR 内の故障伝搬条件を assign_list に入れる．
  auto assign_list = fault->ffr_propagate_condition(mFaultType);

  // assign_list を変換して assumptions に追加する．
  auto as2 = enc.conv_to_literal_list(assign_list);
  assumptions.insert(assumptions.end(), as2.begin(), as2.end());

  // SAT問題を解く
  auto ans = enc.solver().solve(assumptions);

  timer.stop();
  auto sat_time = timer.get_time();

  DtpgResult result;
  if ( ans == SatBool3::True ) {
    // パタンが求まった．

    timer.reset();
    timer.start();

    const auto& model = enc.solver().model();

    // ffr_root より先の伝搬条件を求める．
    auto assign_list2 = enc.extract_prop_condition(ffr_root, 0, model);
    assign_list.merge(assign_list2);

    // assign_list の条件を正当化する．
    auto testvect = mJustifier(mFaultType, assign_list,
			       enc.hvar_map(), enc.gvar_map(), model);

    timer.stop();
    auto backtrace_time = timer.get_time();
    update_det(fault, testvect, sat_time, backtrace_time);
  }
  else if ( ans == SatBool3::False ) {
    // 検出不能と判定された．
    update_untest(fault, sat_time);;
  }
  else {
    // ans == SatBool3::X つまりアボート
    update_abort(fault, sat_time);
  }
}

END_NAMESPACE_DRUID
