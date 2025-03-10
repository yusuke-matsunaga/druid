
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
) : mEngine(network, option)
{
  mBdEnc = new BoolDiffEnc(node, option);
  mEngine.add_subenc(std::unique_ptr<SubEnc>(mBdEnc));
  mEngine.add_prev_node(node);
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
  auto assumptions = mEngine.conv_to_literal_list(ex_cond);
  assumptions.push_back(mBdEnc->prop_var());
  return mEngine.solve(assumptions);
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
  auto pi_assign_list = mEngine.justify(assign_list);
  return TestVector(mEngine.network(), pi_assign_list);
}

// @brief CNF の生成時間を返す．
double
NodeEncDriver::cnf_time() const
{
  return mEngine.cnf_time();
}

// @brief SATの統計情報を返す．
SatStats
NodeEncDriver::sat_stats() const
{
  return mEngine.get_stats();
}

END_NAMESPACE_DRUID
