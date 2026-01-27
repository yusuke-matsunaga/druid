
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
#include "dtpg/DtpgResult.h"
#include "dtpg/DtpgStats.h"
#include "ym/SatStats.h"
#include "ym/Timer.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス DtpgDriver
//////////////////////////////////////////////////////////////////////

// @brief ノード単位をテスト生成を行うオブジェクトを生成する．
DtpgDriver
DtpgDriver::node_driver(
  DtpgMgr& mgr,
  const TpgNode& node,
  const JsonValue& option
)
{
  return DtpgDriver(mgr,
		    new DtpgDriver_NodeEnc(mgr.network(), node, option));
}

// @brief FFR単位でテスト生成を行うオブジェクトを生成する．
DtpgDriver
DtpgDriver::ffr_driver(
  DtpgMgr& mgr,
  const TpgFFR& ffr,
  const JsonValue& option
)
{
  return DtpgDriver(mgr,
		    new DtpgDriver_FFREnc(mgr.network(), ffr, option));
}

// @brief MFFC単位でテスト生成を行うオブジェクトを生成する．
DtpgDriver
DtpgDriver::mffc_driver(
  DtpgMgr& mgr,
  const TpgMFFC& mffc,
  const JsonValue& option
)
{
  return DtpgDriver(mgr,
		    new DtpgDriver_MFFCEnc(mgr.network(), mffc, option));
}

// @brief コンストラクタ
DtpgDriver::DtpgDriver(
  DtpgMgr& mgr,
  DtpgDriverImpl* impl
) : mMgr{mgr},
    mImpl{impl}
{
}

// @brief デストラクタ
DtpgDriver::~DtpgDriver()
{
}

// @brief 故障のテストパタンを求める．
void
DtpgDriver::gen_pattern(
  const TpgFault& fault,
  DtpgStats& stats,
  Callback_Det det_func,
  Callback_Undet untest_func,
  Callback_Undet abort_func
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
    auto testvect = mImpl->gen_pattern(fault);
    timer.stop();
    auto backtrace_time = timer.get_time();

    mMgr.add_testvector(testvect);
    mMgr.set_dtpg_result(fault, DtpgResult::detected(testvect));
    stats.update_det(sat_time, backtrace_time);
    det_func(mMgr, fault, testvect);
  }
  else if ( ans == SatBool3::False ) {
    // 検出不能と判定された．
    mMgr.set_dtpg_result(fault, DtpgResult::untestable());
    stats.update_untest(sat_time);
    untest_func(mMgr, fault);
  }
  else { // SatBool3::X
    // アボート
    stats.update_abort(sat_time);
    abort_func(mMgr, fault);
  }
}

// @brief CNF の生成時間を返す．
double
DtpgDriver::cnf_time() const
{
  return mImpl->cnf_time();
}

// @brief SATの統計情報を返す．
SatStats
DtpgDriver::sat_stats() const
{
  return mImpl->sat_stats();
}

END_NAMESPACE_DRUID
