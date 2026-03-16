
/// @file DtpgDriver.cc
/// @brief DtpgDriver の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgDriver.h"
#include "types/TestVector.h"
#include "ym/SatStats.h"
#include "ym/Timer.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス DtpgDriver
//////////////////////////////////////////////////////////////////////

// @brief ノード単位をテスト生成を行うコンストラクタ
DtpgDriver::DtpgDriver(
  const TpgNode& node,
  const TpgFaultList& fault_list,
  const JsonValue& option
) : mEngine{node, option},
    mFaultList{fault_list}
{
}

// @brief FFR単位でテスト生成を行うコンストラクタ
DtpgDriver::DtpgDriver(
  const TpgFFR& ffr,
  const TpgFaultList& fault_list,
  const JsonValue& option
) : mEngine{ffr, option},
    mFaultList{fault_list}
{
}

// @brief MFFC単位でテスト生成を行うコンストラクタ
DtpgDriver::DtpgDriver(
  const TpgMFFC& mffc,
  const TpgFaultList& fault_list,
  const JsonValue& option
) : mEngine{mffc, option},
    mFaultList{fault_list}
{
}

// @brief デストラクタ
DtpgDriver::~DtpgDriver()
{
}

// @brief 実行する．
void
DtpgDriver::run()
{
  for ( auto fault: mFaultList ) {
    gen_pattern(fault);
  }
  auto cnf_time = mEngine.cnf_time();
  mResults.update_cnf(cnf_time);
  auto sat_stats = mEngine.get_stats();
  mResults.update_sat_stats(sat_stats);
}

// @brief 故障のテストパタンを求める．
void
DtpgDriver::gen_pattern(
  const TpgFault& fault
)
{
  Timer timer;
  timer.start();
  auto lits = mEngine.make_detect_condition(fault);
  auto ans = mEngine.solver().solve(lits);
  timer.stop();
  auto sat_time = timer.get_time();

  if ( ans == SatBool3::True ) {
    // パタンが求まった．
    timer.reset();
    timer.start();
    auto model = mEngine.solver().model();
    auto cond = mEngine.extract_sufficient_condition(fault, model);
    auto pi_assign_list = mEngine.justify(cond, model);
    auto tv = TestVector(pi_assign_list);
    mResults.set_detected(fault, cond, tv);
    timer.stop();
    auto backtrace_time = timer.get_time();
    mResults.update_det(sat_time, backtrace_time);
  }
  else if ( ans == SatBool3::False ) {
    // 検出不能と判定された．
    mResults.set_untestable(fault);
    mResults.update_untest(sat_time);
  }
  else { // SatBool3::X
    // アボート
    mResults.update_abort(sat_time);
  }
}

END_NAMESPACE_DRUID
