
/// @file TpgMgr.cc
/// @brief TpgMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "TpgMgr.h"
#include "DtpgDriver.h"
#include "DtpgResult.h"
#include "DetectOp.h"
#include "UntestOp.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス TpgMgr
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
TpgMgr::TpgMgr(
  const TpgNetwork& network,
  FaultType fault_type,
  const string& dtpg_type,
  const string& just_type,
  const SatSolverType& solver_type
) : mNetwork{network},
    mFaultStatusMgr{network},
    mFsim{network, fault_type, true},
    mDriver{DtpgDriver::new_driver(*this, dtpg_type,
				   network, fault_type,
				   just_type, solver_type)}
{
}

// @brief デストラクタ
TpgMgr::~TpgMgr()
{
  delete mDriver;
}

// @brief テスト生成を行う．
void
TpgMgr::run()
{
  mStats.clear();

  mDriver->run();
}

// @brief 'base' タイプの DetectOp を登録する．
void
TpgMgr::add_base_dop()
{
  add_dop(new_DopBase(fault_status_mgr()));
}

// @brief 'drop' タイプの DetectOp を登録する．
void
TpgMgr::add_drop_dop()
{
  add_dop(new_DopDrop(fault_status_mgr(), fsim()));
}

// @brief 'tvlist' タイプの DetectOp を登録する．
void
TpgMgr::add_tvlist_dop()
{
  add_dop(new_DopTvList(mTVList));
}

// @brief 'verify' タイプの DetectOp を登録する．
void
TpgMgr::add_verify_dop(
  DopVerifyResult& verify_result
)
{
  add_dop(new_DopVerify(fsim(), verify_result));
}

// @brief 'base' タイプの UntestOp を登録する．
void
TpgMgr::add_base_uop()
{
  add_uop(new_UopBase(fault_status_mgr()));
}

// @brief テストパタン生成が成功した時の結果を更新する．
void
TpgMgr::update_det(
  const TpgFault* fault,
  const TestVector& tv,
  double sat_time,
  double backtrace_time
)
{
  for ( auto dop: mDopList ) {
    (*dop)(fault, tv);
  }
  mStats.update_det(sat_time, backtrace_time);
}

// @brief 冗長故障の特定が行えた時の結果を更新する．
void
TpgMgr::update_untest(
  const TpgFault* fault,
  double sat_time
)
{
  for ( auto uop: mUopList ) {
    (*uop)(fault);
  }
  mStats.update_untest(sat_time);
}

// @brief アボートした時の結果を更新する．
void
TpgMgr::update_abort(
  const TpgFault* fault,
  double sat_time
)
{
  mStats.update_abort(sat_time);
}

// @brief CNF 生成に関する情報を更新する．
void
TpgMgr::update_cnf(
  double time
)
{
  mStats.update_cnf(time);
}

// @brief SATの統計情報を更新する．
void
TpgMgr::update_sat_stats(
  const SatStats& sat_stats
)
{
  mStats.update_sat_stats(sat_stats);
}

void
TpgMgr::_update(
  const TpgFault* fault,
  const DtpgResult& result
)
{
  switch ( result.status() ) {
  case FaultStatus::Detected:
    update_det(fault, result.testvector(), 0.0, 0.0);
    break;
  case FaultStatus::Untestable:
    update_untest(fault, 0.0);
    break;
  case FaultStatus::Undetected:
    update_abort(fault, 0.0);
    break;
  }
}

// @brief DTPG の統計情報をマージする．
void
TpgMgr::_merge_stats(
  const DtpgStats& stats
)
{
  mStats.merge(stats);
}

END_NAMESPACE_DRUID
