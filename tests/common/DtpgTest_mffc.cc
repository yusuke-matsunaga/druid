
/// @file DtpgTest_mffc.cc
/// @brief DtpgTest_mffc の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgTest_mffc.h"
#include "Dtpg_se.h"
#include "TpgMFFC.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
DtpgTest_mffc::DtpgTest_mffc(
  const TpgNetwork& network,
  FaultType fault_type,
  const string& just_type,
  const SatSolverType& solver_type
) : DtpgTest{network, fault_type, just_type, solver_type}
{
}

// @brief デストラクタ
DtpgTest_mffc::~DtpgTest_mffc()
{
}

// @brief テストを行う．
// @return 検出故障数と冗長故障数を返す．
void
DtpgTest_mffc::_main_body()
{
  for ( auto mffc: mNetwork.mffc_list() ) {
    Dtpg_se dtpg{mNetwork, mFaultType, mffc, mJustType, mSolverType};
    for ( auto fault: mffc.fault_list() ) {
      if ( mFaultMgr.get(fault) == FaultStatus::Undetected ) {
	// 故障に対するテスト生成を行なう．
	auto result = dtpg.gen_pattern(fault);
	_update(fault, result);
      }
    }
    _merge_stats(dtpg.stats());
  }
}

END_NAMESPACE_DRUID
