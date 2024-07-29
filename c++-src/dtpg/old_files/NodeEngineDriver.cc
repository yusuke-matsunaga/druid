
/// @file NodeEngineDriver.cc
/// @brief NodeEngineDriver の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "NodeEngineDriver.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス NodeEngineDriver
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
NodeEngineDriver::NodeEngineDriver(
  const TpgNetwork& network,
  const TpgNode* node,
  const JsonValue& option
) : mEngine{network, node, option}
{
}

// @brief デストラクタ
NodeEngineDriver::~NodeEngineDriver()
{
}

// @brief 故障を検出する条件を求める．
SatBool3
NodeEngineDriver::solve(
  const TpgFault* fault
)
{
  return mEngine.solve(fault);
}

// @brief テストパタン生成を行う．
TestVector
NodeEngineDriver::gen_pattern(
  const TpgFault* fault
)
{
  return mEngine.gen_pattern(fault);
}

// @brief CNF の生成時間を返す．
double
NodeEngineDriver::cnf_time() const
{
  return mEngine.cnf_time();
}

// @brief SATの統計情報を返す．
SatStats
NodeEngineDriver::sat_stats() const
{
  return mEngine.sat_stats();
}

END_NAMESPACE_DRUID
