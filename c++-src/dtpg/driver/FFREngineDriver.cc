
/// @file FFREngineDriver.cc
/// @brief FFREngineDriver の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "FFREngineDriver.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス FFREngineDriver
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
FFREngineDriver::FFREngineDriver(
  const TpgNetwork& network,
  const TpgFFR* ffr,
  const JsonValue& option
) : mEngine{network, ffr, option}
{
}

// @brief デストラクタ
FFREngineDriver::~FFREngineDriver()
{
}

// @brief 故障を検出する条件を求める．
SatBool3
FFREngineDriver::solve(
  const TpgFault* fault
)
{
  return mEngine.solve(fault);
}

// @brief テストパタン生成を行う．
TestVector
FFREngineDriver::gen_pattern(
  const TpgFault* fault
)
{
  return mEngine.gen_pattern(fault);
}

// @brief SATの統計情報を返す．
SatStats
FFREngineDriver::sat_stats() const
{
  return mEngine.sat_stats();
}

END_NAMESPACE_DRUID
