
/// @file FFREncDriver2.cc
/// @brief FFREncDriver2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "FFREncDriver2.h"
#include "TpgNetwork.h"
#include "TpgFFR.h"
#include "TpgFault.h"
#include "TestVector.h"
#include "ym/Timer.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス FFREncDriver2
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
FFREncDriver2::FFREncDriver2(
  const TpgNetwork& network,
  const TpgFFR* ffr,
  const JsonValue& option
) : mBaseEnc{network, option}
{
  auto node = ffr->root();
  mBdEnc = new BoolDiffEnc{mBaseEnc, node, option};
  mFFREnc = new FFREnc{mBaseEnc, ffr};
  mBaseEnc.make_cnf({}, {node});
}

// @brief デストラクタ
FFREncDriver2::~FFREncDriver2()
{
}

// @brief 故障を検出する条件を求める．
SatBool3
FFREncDriver2::solve(
  const TpgFault* fault
)
{
  auto ex_cond = fault->excitation_condition();
  auto assumptions = mBaseEnc.conv_to_literal_list(ex_cond);
  assumptions.push_back(mBdEnc->prop_var());
  assumptions.push_back(mFFREnc->prop_var(fault->origin_node()));
  return mBaseEnc.solver().solve(assumptions);
}

// @brief テストパタン生成を行う．
TestVector
FFREncDriver2::gen_pattern(
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
FFREncDriver2::cnf_time() const
{
  return mBaseEnc.cnf_time();
}

// @brief SATの統計情報を返す．
SatStats
FFREncDriver2::sat_stats() const
{
  return mBaseEnc.solver().get_stats();
}

END_NAMESPACE_DRUID
