
/// @file FFREncDriver.cc
/// @brief FFREncDriver の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "FFREncDriver.h"
#include "TpgNetwork.h"
#include "TpgFFR.h"
#include "TpgFault.h"
#include "TestVector.h"
#include "ym/Timer.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス FFREncDriver
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
FFREncDriver::FFREncDriver(
  const TpgNetwork& network,
  const TpgFFR* ffr,
  const JsonValue& option
) : mBaseEnc{network, option}
{
  auto node = ffr->root();
  mBdEnc = new BoolDiffEnc{mBaseEnc, node, option};
  mBaseEnc.reg_subenc(mBdEnc);
  mBaseEnc.make_cnf({}, {node});
}

// @brief デストラクタ
FFREncDriver::~FFREncDriver()
{
}

// @brief 故障を検出する条件を求める．
SatBool3
FFREncDriver::solve(
  const TpgFault* fault
)
{
  auto prop_cond = fault->ffr_propagate_condition();
  auto assumptions = mBaseEnc.conv_to_literal_list(prop_cond);
  assumptions.push_back(mBdEnc->prop_var());
  return mBaseEnc.solver().solve(assumptions);
}

// @brief テストパタン生成を行う．
TestVector
FFREncDriver::gen_pattern(
  const TpgFault* fault
)
{
  auto assign_list = mBdEnc->extract_sufficient_condition();
  auto prop_cond = fault->ffr_propagate_condition();
  assign_list.merge(prop_cond);
  auto pi_assign_list = mBaseEnc.justify(assign_list);
  return TestVector{mBaseEnc.network(), pi_assign_list};
}

// @brief CNF の生成時間を返す．
double
FFREncDriver::cnf_time() const
{
  return mBaseEnc.cnf_time();
}

// @brief SATの統計情報を返す．
SatStats
FFREncDriver::sat_stats() const
{
  return mBaseEnc.solver().get_stats();
}

END_NAMESPACE_DRUID
