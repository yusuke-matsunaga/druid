
/// @file FFREngine.cc
/// @brief FFREngine の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "dtpg/FFREngine.h"
#include "FFREnc.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス FFREngine
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
FFREngine::FFREngine(
  const TpgFFR& ffr,
  const TpgFaultList& fault_list,
  const ConfigParam& option
) : StructEngine(ffr.network(), option),
    mEnc{new FFREnc(ffr, fault_list, option)}
{
  add_subenc(std::unique_ptr<SubEnc>{mEnc});
  mEnc->make_cond();
}

// @brief root_node() から到達可能な外部出力のリストを返す．
const TpgNodeList&
FFREngine::output_list() const
{
  return mEnc->output_list();
}

// @brief root_node() から到達可能な外部出力の数を返す．
SizeType
FFREngine::output_num() const
{
  return mEnc->output_num();
}

// @brief root_node() から到達可能な外部出力を返す．
TpgNode
FFREngine::output(
  SizeType pos
) const
{
  return mEnc->output(pos);
}

// @brief 伝搬変数
SatLiteral
FFREngine::prop_var() const
{
  return mEnc->prop_var();
}

// @brief 微分結果を表す変数を返す．
SatLiteral
FFREngine::prop_var(
  SizeType pos
) const
{
  return mEnc->prop_var(pos);
}

// @brief 故障の検出条件を表すリテラルのリストを返す．
const std::vector<SatLiteral>&
FFREngine::dlits(
  const TpgFault& fault
)
{
  return mEnc->dlits(fault);
}

// @brief 故障の非検出条件を表すリテラルを返す．
SatLiteral
FFREngine::ulit(
  const TpgFault& fault
) const
{
  return mEnc->ulit(fault);
}

// @brief SAT問題の解から十分条件を求める．
SuffCond
FFREngine::extract_sufficient_condition(
  const TpgFault& fault,
  const SatModel& model,
  const AssignList& assign_list
)
{
  auto cond = mEnc->extract_sufficient_condition(model, assign_list);
  return _extract_sub(cond, fault, model);
}

// @brief SAT問題の解から十分条件を求める．
SuffCond
FFREngine::extract_sufficient_condition(
  const TpgFault& fault,
  SizeType pos,
  const SatModel& model,
  const AssignList& assign_list
)
{
  auto cond = mEnc->extract_sufficient_condition(pos, model, assign_list);
  return _extract_sub(cond, fault, model);
}

// @brief extract_sufficient_condition() の下請け関数
SuffCond
FFREngine::_extract_sub(
  const SuffCond& cond,
  const TpgFault& fault,
  const SatModel& model
)
{
  auto main_cond = cond.main_cond();
  main_cond.merge(fault.ffr_propagate_condition());
  auto aux_cond = cond.aux_cond();
  for ( auto node: fault.ffr_aux_side_inputs() ) {
    aux_cond.add(assign(node, 1, model));
  }
  return SuffCond(main_cond, aux_cond);
}

END_NAMESPACE_DRUID
