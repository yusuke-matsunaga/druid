
/// @file TpgMgr.cc
/// @brief TpgMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "TpgMgr.h"
#include "TpgDriver.h"
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
    mDriver{TpgDriver::new_driver(*this, dtpg_type,
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
  mDetCount = 0;
  mUntestCount = 0;
  mAbortCount = 0;

  mDriver->run();
}

// @brief テストパタン生成後の情報の更新を行う．
void
TpgMgr::_update(
  const TpgFault* fault,
  const DtpgResult& result
)
{
  switch ( result.status() ) {
  case FaultStatus::Detected:
    for ( auto dop: mDopList ) {
      (*dop)(fault, result.testvector());
    }
    ++ mDetCount;
    break;

  case FaultStatus::Untestable:
    for ( auto uop: mUopList ) {
      (*uop)(fault);
    }
    ++ mUntestCount;
    break;

  case FaultStatus::Undetected:
    ++ mAbortCount;
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
