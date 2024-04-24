
/// @file MFFCEngineDriver.cc
/// @brief MFFCEngineDriver の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "MFFCEngineDriver.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス MFFCEngineDriver
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
MFFCEngineDriver::MFFCEngineDriver(
  const TpgNetwork& network,
  const TpgMFFC* mffc,
  const JsonValue& option
) : mEngine{network, mffc, option}
{
}

// @brief デストラクタ
MFFCEngineDriver::~MFFCEngineDriver()
{
}

// @brief 故障を検出する条件を求める．
SatBool3
MFFCEngineDriver::solve(
  const TpgFault* fault
)
{
  return mEngine.solve(fault);
}

// @brief テストパタン生成を行う．
TestVector
MFFCEngineDriver::gen_pattern(
  const TpgFault* fault
)
{
  return mEngine.gen_pattern(fault);
}

// @brief SATの統計情報を返す．
SatStats
MFFCEngineDriver::sat_stats() const
{
  return mEngine.sat_stats();
}

END_NAMESPACE_DRUID
