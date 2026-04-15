
/// @file FFR2Engine.cc
/// @brief FFR2Engine の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "dtpg/FFR2Engine.h"
#include "BoolDiffEnc.h"
#include "FFREnc.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス FFR2Engine
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
FFR2Engine::FFR2Engine(
  const TpgFFR& ffr1,
  const TpgFFR& ffr2,
  const TpgFaultList& fault_list1,
  const TpgFaultList& fault_list2,
  const ConfigParam& option
) : StructEngine(ffr1.network(), option)
{
  auto enc_option = option;
  enc_option.add("has_ulit", true);
  mEnc1 = new FFREnc(ffr1, fault_list1, enc_option);
  mEnc2 = new FFREnc(ffr2, fault_list2, enc_option);
  add_subenc(std::unique_ptr<SubEnc>{mEnc1});
  add_subenc(std::unique_ptr<SubEnc>{mEnc2});
  mEnc1->make_cond();
  mEnc2->make_cond();
}

// @brief 2つの故障の検出条件を調べる．
SatBool3
FFR2Engine::solve(
  const TpgFault& fault1,
  const TpgFault& fault2,
  bool det1,
  bool det2
)
{
  if ( det1 ) {
    auto& dlits1 = mEnc1->dlits(fault1);
    if ( det2 ) {
      auto& dlits2 = mEnc2->dlits(fault2);
      auto tmp_lits = concat_lits(dlits1, dlits2);
      return solver().solve(tmp_lits);
    }
    else {
      auto ulit2 = mEnc2->ulit(fault2);
      auto tmp_lits = dlits1;
      tmp_lits.push_back(ulit2);
      return solver().solve(tmp_lits);
    }
  }
  else {
    auto ulit1 = mEnc1->ulit(fault1);
    if ( det2 ) {
      auto& dlits2 = mEnc2->dlits(fault2);
      auto tmp_lits = dlits2;
      tmp_lits.push_back(ulit1);
      return solver().solve(tmp_lits);
    }
    else {
      auto ulit2 = mEnc2->ulit(fault2);
      std::vector<SatLiteral> tmp_lits{ulit1, ulit2};
      return solver().solve(tmp_lits);
    }
  }
}

END_NAMESPACE_DRUID
