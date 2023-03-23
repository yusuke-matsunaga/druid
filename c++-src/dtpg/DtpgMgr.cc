
/// @file DtpgMgr.cc
/// @brief DtpgMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgMgr.h"
#include "DtpgDriver.h"
#include "DtpgResult.h"
#include "DetectOp.h"
#include "UntestOp.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス DtpgMgr
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
DtpgMgr::DtpgMgr(
  const TpgNetwork& network,
  FaultType fault_type,
  const string& dtpg_type,
  const string& just_type,
  const SatSolverType& solver_type
) : mNetwork{network},
    mDriver{DtpgDriver::new_driver(*this, dtpg_type,
				   network, fault_type == FaultType::TransitionDelay,
				   just_type, solver_type)}
{
  mFaultMgr.gen_fault_list(network, fault_type);
  mFsim.initialize(network, mFaultMgr, true);
}

// @brief デストラクタ
DtpgMgr::~DtpgMgr()
{
  delete mDriver;
}

// @brief テスト生成を行う．
void
DtpgMgr::run()
{
  mStats.clear();

  mDriver->run();
}

// @brief 'base' タイプの DetectOp を登録する．
void
DtpgMgr::add_base_dop()
{
  add_dop(new_DopBase(fault_mgr()));
}

// @brief 'drop' タイプの DetectOp を登録する．
void
DtpgMgr::add_drop_dop()
{
  add_dop(new_DopDrop(fault_mgr(), fsim()));
}

// @brief 'tvlist' タイプの DetectOp を登録する．
void
DtpgMgr::add_tvlist_dop()
{
  add_dop(new_DopTvList(mTVList));
}

// @brief 'verify' タイプの DetectOp を登録する．
void
DtpgMgr::add_verify_dop()
{
  add_dop(new_DopVerify(fsim(), mVerifyResult));
}

// @brief 'base' タイプの UntestOp を登録する．
void
DtpgMgr::add_base_uop()
{
  add_uop(new_UopBase(fault_mgr()));
}

// @brief テストパタン生成が成功した時の結果を更新する．
void
DtpgMgr::update_det(
  const TpgFault& fault,
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
DtpgMgr::update_untest(
  const TpgFault& fault,
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
DtpgMgr::update_abort(
  const TpgFault& fault,
  double sat_time
)
{
  mStats.update_abort(sat_time);
}

// @brief CNF 生成に関する情報を更新する．
void
DtpgMgr::update_cnf(
  double time
)
{
  mStats.update_cnf(time);
}

// @brief SATの統計情報を更新する．
void
DtpgMgr::update_sat_stats(
  const SatStats& sat_stats
)
{
  mStats.update_sat_stats(sat_stats);
}

END_NAMESPACE_DRUID
