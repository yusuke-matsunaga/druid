
/// @file DtpgEngine.cc
/// @brief DtpgEngine の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgEngine.h"

#include "TpgNetwork.h"
#include "TpgFault.h"
#include "GateEnc.h"
#include "NodeValList.h"
#include "TestVector.h"
#include "TpgNodeSet.h"
#include "extract.h"

#include "ym/SatSolver.h"
#include "ym/SatStats.h"
#include "ym/Range.h"
#include "ym/Timer.h"

//#define DEBUG_DTPG

#define DEBUG_OUT cout

BEGIN_NONAMESPACE
#ifdef DEBUG_DTPG
const int debug_dtpg = 1;
#else
const int debug_dtpg = 0;
#endif
END_NONAMESPACE


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
DtpgEngine::DtpgEngine(
  const TpgNetwork& network,
  const TpgNode* root,
  const JsonValue& option
) : BoolDiffEngine{network, root, option}
{
}

// @brief デストラクタ
DtpgEngine::~DtpgEngine()
{
}

// @brief テストパタン生成を行う．
SatBool3
DtpgEngine::solve(
  const TpgFault* fault
)
{
  // fault 用の追加条件
  auto assumptions = extra_assumptions(fault);
  // fault の活性化条件
  auto fault_cond = fault_condition(fault);
  add_to_literal_list(fault_cond, assumptions);
  return check(assumptions);
}

// @brief solve() が成功した時にテストパタンを生成する．
TestVector
DtpgEngine::gen_pattern(
  const TpgFault* fault
)
{
  auto assign_list = get_sufficient_condition(fault);
  auto pi_assign_list = justify(assign_list);
  return TestVector{network(), pi_assign_list};
}

// @brief 十分条件を取り出す．
NodeValList
DtpgEngine::get_sufficient_condition(
  const TpgFault* fault
)
{
  // FFR の根の先の伝搬条件
  auto suf_cond = extract_sufficient_condition();

  // 故障の活性化条件
  auto fault_cond = fault_condition(fault);
  suf_cond.merge(fault_cond);
  return suf_cond;
}

// @brief gen_pattern() で用いる追加の検出条件
vector<SatLiteral>
DtpgEngine::extra_assumptions(
  const TpgFault* fault
)
{
  return {};
}

END_NAMESPACE_DRUID
