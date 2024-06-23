
/// @file NodeEncDriver.cc
/// @brief NodeEncDriver の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "NodeEncDriver.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス NodeEncDriver
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
NodeEncDriver::NodeEncDriver(
  const TpgNetwork& network,
  const TpgNode* node,
  const JsonValue& option
) : mBaseEnc{network, option}
{
  auto bd = new BoolDiffEnc{mBaseEnc, node};
  mBaseEnc.reg_subenc(bd);
  mBaseEnc.make_cnf();
  mBaseEnc.solver().add_clause(bd->prop_var());
}

// @brief デストラクタ
NodeEncDriver::~NodeEncDriver()
{
}

// @brief 故障を検出する条件を求める．
SatBool3
NodeEncDriver::solve(
  const TpgFault* fault
)
{
  auto ex_cond = fault->excitation_condition();
  auto assumptions = mBaseEnc.conv_to_literal_list(ex_cond);
  return mBaseEnc.solver().solve(assumptions);
}

// @brief テストパタン生成を行う．
TestVector
NodeEncDriver::gen_pattern(
  const TpgFault* fault
)
{
  return mEngine.gen_pattern(fault);
}

// @brief CNF の生成時間を返す．
double
NodeEncDriver::cnf_time() const
{
  return mBaseEnc.cnf_time();
}

// @brief SATの統計情報を返す．
SatStats
NodeEncDriver::sat_stats() const
{
  return mBaseEnc.solver().sat_stats();
}

END_NAMESPACE_DRUID
