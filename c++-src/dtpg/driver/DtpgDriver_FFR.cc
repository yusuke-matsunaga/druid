
/// @file DtpgDriver_FFR.cc
/// @brief DtpgDriver_FFR の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgDriver_FFR.h"
#include "types/TpgNetwork.h"
#include "types/TpgFFR.h"
#include "types/TpgFault.h"
#include "types/TestVector.h"
#include "ym/Timer.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス DtpgDriver_FFR
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
DtpgDriver_FFR::DtpgDriver_FFR(
  const TpgFFR& ffr,
  const JsonValue& option
) : mEngine(ffr, option)
{
}

// @brief デストラクタ
DtpgDriver_FFR::~DtpgDriver_FFR()
{
}

// @brief 故障を検出する条件を求める．
SatBool3
DtpgDriver_FFR::solve(
  const TpgFault& fault,
  const std::vector<SatLiteral>& assumptions
)
{
  auto prop_cond = fault.ffr_propagate_condition();
  auto assumptions = mEngine.conv_to_literal_list(prop_cond);
  assumptions.push_back(mEngine.prop_var());
  add_extra_assumptions(fault, assumptions);
  auto res = mEngine.solve(assumptions);
}

// @brief 故障に対する処理を行う．
void
DtpgDriver_FFR::fault_op(
  const TpgFault& fault,
  DtpgResults& result
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
DtpgDriver_FFR::cnf_time() const
{
  return mEngine.cnf_time();
}

// @brief SATの統計情報を返す．
SatStats
DtpgDriver_FFR::sat_stats() const
{
  return mEngine.get_stats();
}

// @biref 値割当が必要なノードのリストを返す．
AssignList
DtpgDriver_FFR::get_aux_side_inputs(
  const TpgFault& fault
)
{
  AssignList assign_list;
  for ( auto node: fault.aux_side_inputs() ) {
    assign_list.add(get_assign(node, 1));
  }
  return assign_list;
}

END_NAMESPACE_DRUID
