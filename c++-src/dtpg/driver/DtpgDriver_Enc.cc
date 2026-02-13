
/// @file DtpgDriver_Enc.cc
/// @brief DtpgDriver_Enc の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgDriver_Enc.h"
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

// @brief テストパタン生成を行う．
TestVector
DtpgDriver_Enc::gen_pattern(
  const TpgFault& fault
)
{
  auto assign_list = mEngine.extract_sufficient_condition();
  auto prop_cond = fault_prop_condition(fault);
  assign_list.merge(prop_cond);
  add_extra_assignments(fault, assign_list);
  auto pi_assign_list = mEngine.justify(assign_list);
  return TestVector(pi_assign_list);
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

END_NAMESPACE_DRUID
