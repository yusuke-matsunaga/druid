
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
#include "ConeInfo.h"
#include "BSInfo.h"
#include "ConeEnc.h"
#include "BSEnc.h"
#include "extract.h"
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
) : mSolver{option.get("sat_param")},
    mNetwork{network},
    mGvarMap{network.node_num()},
    mFvarMap{network.node_num()},
    mHvarMap{network.node_num()},
    mJustifier{network, option.get("justifier")},
    mExOpt{option.get("extractor")}
{
  Timer timer;
  timer.start();
  ConeInfo cone_info{network, ffr->root()};
  ConeEnc::encode(mSolver, cone_info, mGvarMap, mFvarMap);
  if ( network.fault_type() == FaultType::TransitionDelay ) {
    BSInfo bs_info{cone_info};
    BSEnc::encode(mSolver, cone_info, bs_info, mGvarMap, mHvarMap);
  }
  timer.stop();
  mCnfTime = timer.get_time();
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
  auto assign_list = fault->ffr_propagate_condition();
  auto assumptions = conv_to_literal_list(assign_list);
  return mSolver.solve(assumptions);
}

// @brief テストパタン生成を行う．
TestVector
FFREncDriver::gen_pattern(
  const TpgFault* fault
)
{
  auto& model = mSolver.model();
  auto suf_cond = extract_sufficient_condition(fault->ffr_root(),
					       mGvarMap, mFvarMap,
					       model, mExOpt);
  auto ffr_cond = fault->ffr_propagate_condition();
  suf_cond.merge(ffr_cond);
  auto pi_assign_list = mJustifier(suf_cond, mHvarMap, mGvarMap, model);
  return TestVector{mNetwork, pi_assign_list};
}

// @brief CNF の生成時間を返す．
double
FFREncDriver::cnf_time() const
{
  return mCnfTime;
}

// @brief SATの統計情報を返す．
SatStats
FFREncDriver::sat_stats() const
{
  return mSolver.get_stats();
}

// @brief 値割り当てをリテラルに変換する．
SatLiteral
FFREncDriver::conv_to_literal(
  NodeVal node_val
)
{
  auto node = node_val.node();
  bool inv = !node_val.val(); // 0 の時が inv = true
  auto vid = (node_val.time() == 0) ? mHvarMap(node) : mGvarMap(node);
  return vid * inv;
}

// @brief 値割り当てをリテラルのリストに変換する．
void
FFREncDriver::add_to_literal_list(
  const NodeValList& assign_list,
  vector<SatLiteral>& lit_list
)
{
  SizeType n0 = lit_list.size();
  SizeType n = assign_list.size();
  lit_list.reserve(n + n0);
  for ( auto nv: assign_list ) {
    auto lit = conv_to_literal(nv);
    lit_list.push_back(lit);
  }
}

END_NAMESPACE_DRUID
