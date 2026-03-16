
/// @file FaultEnc_MFFC.cc
/// @brief FaultEnc_MFFC の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "FaultEnc_MFFC.h"
#include "MFFCEnc.h"
#include "dtpg/BdEngine.h"
#include "types/TpgNetwork.h"
#include "types/TpgMFFC.h"
#include "types/TpgFault.h"
#include "GateEnc.h"

//#define DEBUG_DTPG
#define DEBUG_OUT std::cout


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
FaultEnc_MFFC::FaultEnc_MFFC(
  BdEngine& engine,
  const TpgMFFC& mffc
) : mEnc{new MFFCEnc(mffc)}
{
  engine.add_subenc(std::unique_ptr<SubEnc>{mEnc});
}

// @brief 故障の活性化と伝搬条件の値割り当てを返す．
AssignList
FaultEnc_MFFC::fault_propagate_condition(
  const TpgFault& fault
)
{
  auto prop_cond = fault.ffr_propagate_condition();
  return prop_cond;
}

// @brief 故障検出のための補助的な条件を追加する．
void
FaultEnc_MFFC::add_aux_condition(
  const TpgFault& fault,
  std::vector<SatLiteral>& lit_list
)
{
  auto lits = mEnc->cvar_assumptions(fault);
  lit_list.insert(lit_list.end(), lits.begin(), lits.end());
  lit_list.push_back(mEnc->prop_var());
}

// @brief SATの解から十分条件を得る．
SuffCond
FaultEnc_MFFC::extract_sufficient_condition(
  BdEngine& engine,
  const TpgFault& fault,
  const SuffCond& cond,
  const SatModel& model
)
{
  // fault の FFR の根から MFFC の根までの伝搬条件
  auto cond2 = mEnc->extract_sufficient_condition(fault, model);
  auto main_cond = cond.main_cond();
  main_cond += cond2.main_cond();
  // fault から FFR の根までの伝搬条件
  main_cond += fault.ffr_propagate_condition();
  auto aux_cond = cond.aux_cond();
  aux_cond += cond2.aux_cond();
  for ( auto node: fault.ffr_aux_side_inputs() ) {
    aux_cond.add(engine.assign(node, 1, model));
  }
  return SuffCond(main_cond, aux_cond);
}

END_NAMESPACE_DRUID
