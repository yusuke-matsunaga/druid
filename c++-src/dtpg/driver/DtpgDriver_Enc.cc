
/// @file DtpgDriver_Enc.cc
/// @brief DtpgDriver_Enc の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgDriver_Enc.h"
#include "dtpg/DtpgResults.h"
#include "types/TpgFault.h"
#include "types/TestVector.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス DtpgDriver_Enc
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
DtpgDriver_Enc::DtpgDriver_Enc(
  const TpgNode& node,
  const JsonValue& option
) : mEngine(node.network(), node, option)
{
  mEngine.add_prev_node(node);
}

// @brief デストラクタ
DtpgDriver_Enc::~DtpgDriver_Enc()
{
}

// @brief 故障を検出する条件を求める．
SatBool3
DtpgDriver_Enc::solve(
  const TpgFault& fault
)
{
  auto prop_cond = fault_prop_condition(fault);
  auto assumptions = mEngine.conv_to_literal_list(prop_cond);
  assumptions.push_back(mEngine.prop_var());
  add_extra_assumptions(fault, assumptions);
  return mEngine.solve(assumptions);
}

// @brief 故障に対する処理を行う．
void
DtpgDriver_Enc::fault_op(
  const TpgFault& fault,
  DtpgResults& results
)
{
  auto assign_pair = mEngine.extract_sufficient_condition();
  auto& assign_list = assign_pair.first;
  auto prop_cond = fault_prop_condition(fault);
  assign_list.merge(prop_cond);
  add_extra_assignments(fault, assign_list);
  auto& aux_side_inputs = assign_pair.second;
  auto aux_side_inputs1 = get_aux_side_inputs(fault);
  aux_side_inputs.merge(aux_side_inputs1);
  auto pi_assign_list = mEngine.justify(assign_list, aux_side_inputs);
  auto tv = TestVector(pi_assign_list);
  results.set_detected(fault, assign_list, aux_side_inputs, tv);
}

// @brief CNF の生成時間を返す．
double
DtpgDriver_Enc::cnf_time() const
{
  return mEngine.cnf_time();
}

// @brief SATの統計情報を返す．
SatStats
DtpgDriver_Enc::sat_stats() const
{
  return mEngine.get_stats();
}

// @brief 追加の条件を加える．
void
DtpgDriver_Enc::add_extra_assumptions(
  const TpgFault& fault,
  std::vector<SatLiteral>& assumptions
)
{
}

// @brief 追加の割り当てを加える．
void
DtpgDriver_Enc::add_extra_assignments(
  const TpgFault& fault,
  AssignList& assign_list
)
{
}

// @biref 値割当が必要なノードのリストを返す．
AssignList
DtpgDriver_Enc::get_aux_side_inputs(
  const TpgFault& fault
)
{
  return AssignList();
}

// @brief 値割り当てを返す．
Assign
DtpgDriver_Enc::get_assign(
  const TpgNode& node,
  int time
)
{
  return Assign(node, time, mEngine.val(node, time));
}

END_NAMESPACE_DRUID
