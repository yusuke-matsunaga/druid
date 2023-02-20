
/// @file FFREngine.cc
/// @brief FFREngine の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "FFREngine.h"

//#include "TpgFault.h"
//#include "TpgFFR.h"
//#include "NodeValList.h"
//#include "TestVector.h"
//#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
FFREngine::FFREngine(
  const TpgNetwork& network,
  FaultType fault_type,
  const TpgFFR& ffr,
  const SatSolverType& solver_type
) : DtpgEngine{network, fault_type, ffr.root(), true, solver_type}
{
}

// @brief デストラクタ
FFREngine::~FFREngine()
{
}

#if 0
// @brief make_cnf() の追加処理
void
FFREngine::opt_make_cnf()
{
}

// @brief gen_pattern() で用いる検出条件を作る．
vector<SatLiteral>
FFREngine::gen_assumptions(
  const TpgFault* fault ///< [in] 対象の故障
)
{
  return {};
}
#endif

END_NAMESPACE_DRUID
