
/// @file DtpgDriver.cc
/// @brief DtpgDriver の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgDriver.h"
#include "DtpgDriver_NodeEnc.h"
#include "DtpgDriver_FFREnc.h"
#include "DtpgDriver_MFFCEnc.h"
#include "types/TestVector.h"
#include "dtpg/DtpgResults.h"
#include "ym/SatStats.h"
#include "ym/Timer.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス DtpgDriver
//////////////////////////////////////////////////////////////////////

// @brief ノード単位をテスト生成を行うオブジェクトを生成する．
DtpgDriver
DtpgDriver::node_driver(
  const TpgNode& node,
  const TpgFaultList& fault_list,
  const JsonValue& option
)
{
  return DtpgDriver(new DtpgDriver_NodeEnc(node, option),
		    fault_list);
}

// @brief FFR単位でテスト生成を行うオブジェクトを生成する．
DtpgDriver
DtpgDriver::ffr_driver(
  const TpgFFR& ffr,
  const TpgFaultList& fault_list,
  const JsonValue& option
)
{
  return DtpgDriver(new DtpgDriver_FFREnc(ffr, option),
		    fault_list);
}

// @brief MFFC単位でテスト生成を行うオブジェクトを生成する．
DtpgDriver
DtpgDriver::mffc_driver(
  const TpgMFFC& mffc,
  const TpgFaultList& fault_list,
  const JsonValue& option
)
{
  return DtpgDriver(new DtpgDriver_MFFCEnc(mffc, option),
		    fault_list);
}

// @brief コンストラクタ
DtpgDriver::DtpgDriver(
  DtpgDriverImpl* impl,
  const TpgFaultList& fault_list
) : mImpl{impl},
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
  auto cnf_time = mImpl->cnf_time();
  mResults.update_cnf(cnf_time);
  auto sat_stats = mImpl->sat_stats();
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
  auto ans = mImpl->solve(fault);
  timer.stop();
  auto sat_time = timer.get_time();

  if ( ans == SatBool3::True ) {
    // パタンが求まった．
    timer.reset();
    timer.start();
    mImpl->fault_op(fault, mResults);
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
