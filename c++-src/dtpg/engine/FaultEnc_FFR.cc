
/// @file FaultEnc_FFR.cc
/// @brief FaultEnc_FFR の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "FaultEnc_FFR.h"
#include "dtpg/BdEngine.h"
#include "types/TpgFault.h"
#include "types/Val3.h"


BEGIN_NAMESPACE_DRUID

// @brief 故障の活性化と伝搬条件の値割り当てを返す．
AssignList
FaultEnc_FFR::fault_propagate_condition(
  const TpgFault& fault
)
{
  auto prop_cond = fault.ffr_propagate_condition();
  return prop_cond;
}

// @brief 故障検出のための補助的な条件を追加する．
void
FaultEnc_FFR::add_aux_condition(
  const TpgFault& fault,
  std::vector<SatLiteral>& lit_list
)
{
  // なにもしない．
}

// @brief SATの解から十分条件を得る．
SuffCond
FaultEnc_FFR::extract_sufficient_condition(
  BdEngine& engine,
  const TpgFault& fault,
  const SuffCond& cond,
  const SatModel& model
)
{
  auto main_cond = cond.main_cond();
  main_cond.merge(fault.ffr_propagate_condition());
  auto aux_cond = cond.aux_cond();
  for ( auto node: fault.ffr_aux_side_inputs() ) {
    aux_cond.add(engine.assign(node, 1, model));
  }
  return SuffCond(main_cond, aux_cond);
}

END_NAMESPACE_DRUID
