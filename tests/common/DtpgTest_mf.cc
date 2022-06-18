
/// @file DtpgTest_ffr.cc
/// @brief DtpgTest_ffr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgTest_ffr.h"
#include "Dtpg_se.h"
#include "TpgFFR.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
DtpgTest_ffr::DtpgTest_ffr(
  const TpgNetwork& network,
  FaultType fault_type,
  const string& just_type,
  const SatSolverType& solver_type
) : DtpgTest{network, fault_type, just_type, solver_type}
{
}

// @brief デストラクタ
DtpgTest_ffr::~DtpgTest_ffr()
{
}

// @brief テストを行う．
// @return 検出故障数と冗長故障数を返す．
void
DtpgTest_ffr::_main_body()
{
  for ( auto& ffr: mNetwork.ffr_list() ) {
    Dtpg_se dtpg{mNetwork, mFaultType, ffr, mJustType, mSolverType};
    for ( auto fault: ffr.fault_list() ) {
      if ( mFaultMgr.get(fault) == FaultStatus::Undetected ) {
	auto result = dtpg.dtpg(fault);
	_update(fault, result);
      }
    }
    _merge_stats(dtpg.stats());
  }
}

END_NAMESPACE_DRUID
