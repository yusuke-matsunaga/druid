
/// @file NaiveDtpgEngine.cc
/// @brief NaiveDtpgEngine の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "dtpg/NaiveDtpgEngine.h"
#include "types/TpgFault.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
NaiveDtpgEngine::NaiveDtpgEngine(
  const TpgFault& fault,
  const ConfigParam& option
) : mEngine(fault.origin_node(), option)
{
  auto& solver = mEngine.solver();
  auto cond = fault.excitation_condition();
  auto lits = mEngine.conv_to_literal_list(cond);
  auto pvar = mEngine.prop_var();
  for ( auto lit: lits ) {
    solver.add_clause(lit);
  }
  solver.add_clause(pvar);
}

// @brief デストラクタ
NaiveDtpgEngine::~NaiveDtpgEngine()
{
}

// @brief SAT問題を解く
SatBool3
NaiveDtpgEngine::solve(
  const std::vector<SatLiteral>& assumptions
)
{
  return solver().solve(assumptions);
}

// @brief SATの解から十分条件を得る．
SuffCond
NaiveDtpgEngine::extract_sufficient_condition(
  const SatModel& model,
  const AssignList& assign_list
)
{
  auto cond = mEngine.extract_sufficient_condition(model, assign_list);
  return cond;
}

// @brief SATの解から十分条件を得る．
SuffCond
NaiveDtpgEngine::extract_sufficient_condition(
  SizeType pos,
  const SatModel& model,
  const AssignList& assign_list
)
{
  auto cond = mEngine.extract_sufficient_condition(pos, model, assign_list);
  return cond;
}

// @brief 与えられた割り当てを満足する外部入力の割り当てを求める．
AssignList
NaiveDtpgEngine::justify(
  const SuffCond& suff_cond,
  const SatModel& model,
  const AssignList& assign_list
)
{
  return mEngine.justify(suff_cond, model, assign_list);
}

// @brief 値割り当てを対応するリテラルに変換する．
SatLiteral
NaiveDtpgEngine::conv_to_literal(
  Assign assign
)
{
  return mEngine.conv_to_literal(assign);
}

// @brief 値割り当てのリストを対応するリテラルのリストに変換する．
std::vector<SatLiteral>
NaiveDtpgEngine::conv_to_literal_list(
  const AssignList& assign_list
)
{
  return mEngine.conv_to_literal_list(assign_list);
}

// @brief 現在の内部状態を得る．
SatStats
NaiveDtpgEngine::get_stats() const
{
  return mEngine.get_stats();
}

// @brief CNF の生成時間を返す．
double
NaiveDtpgEngine::cnf_time() const
{
  return mEngine.cnf_time();
}

END_NAMESPACE_DRUID
