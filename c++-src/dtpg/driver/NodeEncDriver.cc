
/// @file NodeEncDriver.cc
/// @brief NodeEncDriver の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "NodeEncDriver.h"
#include "TpgFault.h"
#include "TestVector.h"


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
  mBdEnc = new BoolDiffEnc{mBaseEnc, node, option};
  mBaseEnc.make_cnf({}, {node});
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
  assumptions.push_back(mBdEnc->prop_var());
  return mBaseEnc.solver().solve(assumptions);
}

// @brief テストパタン生成を行う．
TestVector
NodeEncDriver::gen_pattern(
  const TpgFault* fault
)
{
  auto assign_list = mBdEnc->extract_sufficient_condition();
  auto ex_cond = fault->excitation_condition();
  assign_list.merge(ex_cond);
  auto pi_assign_list = mBaseEnc.justify(assign_list);
  return TestVector{mBaseEnc.network(), pi_assign_list};
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
  return mBaseEnc.solver().get_stats();
}

END_NAMESPACE_DRUID
