
/// @file FFREngine.cc
/// @brief FFREngine の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "FFREngine.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
FFREngine::FFREngine(
  const TpgNetwork& network,
  bool has_prev_state,
  const TpgFFR& ffr,
  const SatSolverType& solver_type
) : DtpgEngine{network, has_prev_state, ffr.root(), true, solver_type}
{
}

// @brief デストラクタ
FFREngine::~FFREngine()
{
}

// @brief gen_pattern() で用いる検出条件を作る．
vector<SatLiteral>
FFREngine::gen_assumptions(
  const TpgFault& fault
)
{
  return {};
}

END_NAMESPACE_DRUID
