
/// @file DtpgDriver_MFFCEnc.cc
/// @brief DtpgDriver_MFFCEnc の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgDriver_MFFCEnc.h"
#include "types/TpgNetwork.h"
#include "types/TpgMFFC.h"
#include "types/TpgFault.h"
#include "types/TestVector.h"
#include "ym/Timer.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス DtpgDriver_MFFCEnc
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
DtpgDriver_MFFCEnc::DtpgDriver_MFFCEnc(
  const TpgNetwork& network,
  const TpgMFFC& mffc,
  const JsonValue& option
) : DtpgDriver_Enc(network, mffc.root(), option),
    mMFFCEnc{new MFFCEnc(mffc)}
{
  add_subenc(std::unique_ptr<SubEnc>{mMFFCEnc});
}

// @brief 故障の伝搬条件を得る．
AssignList
DtpgDriver_MFFCEnc::fault_prop_condition(
  const TpgFault& fault
)
{
  return fault.ffr_propagate_condition();
}

// @brief 追加の条件を加える．
void
DtpgDriver_MFFCEnc::add_extra_assumptions(
  const TpgFault& fault,
  std::vector<SatLiteral>& assumptions
)
{
  auto assumptions1 = mMFFCEnc->cvar_assumptions(fault);
  assumptions1.push_back(mMFFCEnc->prop_var());
  assumptions.insert(assumptions.end(), assumptions1.begin(), assumptions1.end());
}

// @brief 追加の割り当てを加える．
void
DtpgDriver_MFFCEnc::add_extra_assignments(
  const TpgFault& fault,
  AssignList& assign_list
)
{
  auto mffc_cond = mMFFCEnc->extract_sufficient_condition(fault);
  assign_list.merge(mffc_cond);
}

END_NAMESPACE_DRUID
