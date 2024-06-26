
/// @file MFFCEncDriver.cc
/// @brief MFFCEncDriver の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "MFFCEncDriver.h"
#include "TpgNetwork.h"
#include "TpgMFFC.h"
#include "TpgFault.h"
#include "TestVector.h"
#include "ym/Timer.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス MFFCEncDriver
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
MFFCEncDriver::MFFCEncDriver(
  const TpgNetwork& network,
  const TpgMFFC* mffc,
  const JsonValue& option
) : mBaseEnc{network, option}
{
  auto node = mffc->root();
  mBdEnc = new BoolDiffEnc{mBaseEnc, node, option};
  mMFFCEnc = new MFFCEnc{mBaseEnc, mffc};
  mBaseEnc.make_cnf({}, {node});
}

// @brief 故障を検出する条件を求める．
SatBool3
MFFCEncDriver::solve(
  const TpgFault* fault
)
{
  auto assign_list = fault->ffr_propagate_condition();
  auto assumptions = mBaseEnc.conv_to_literal_list(assign_list);
  assumptions.push_back(mBdEnc->prop_var());
  assumptions.push_back(mMFFCEnc->prop_var());
  auto assumptions1 = mMFFCEnc->cvar_assumptions(fault);
  assumptions.insert(assumptions.end(), assumptions1.begin(), assumptions1.end());
  return mBaseEnc.solver().solve(assumptions);
}

// @brief テストパタン生成を行う．
TestVector
MFFCEncDriver::gen_pattern(
  const TpgFault* fault
)
{
  auto assign_list = mBdEnc->extract_sufficient_condition();
  auto mffc_cond = mMFFCEnc->extract_sufficient_condition(fault);
  assign_list.merge(mffc_cond);
  auto prop_cond = fault->ffr_propagate_condition();
  assign_list.merge(prop_cond);
  auto pi_assign_list = mBaseEnc.justify(assign_list);
  return TestVector{mBaseEnc.network(), pi_assign_list};
}

// @brief CNF の生成時間を返す．
double
MFFCEncDriver::cnf_time() const
{
  return mBaseEnc.cnf_time();
}

// @brief SATの統計情報を返す．
SatStats
MFFCEncDriver::sat_stats() const
{
  return mBaseEnc.solver().get_stats();
}

END_NAMESPACE_DRUID
