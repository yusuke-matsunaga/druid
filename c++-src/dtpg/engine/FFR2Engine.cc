
/// @file FFR2Engine.cc
/// @brief FFR2Engine の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "dtpg/FFR2Engine.h"


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
) : Bd2Engine(ffr1.root(), ffr2.root(), option),
    mFaultList1{fault_list1},
    mDLitsArray1(mFaultList1.size()),
    mULitArray1(mFaultList1.size()),
    mFaultList2{fault_list2},
    mDLitsArray2(mFaultList2.size()),
    mULitArray2(mFaultList2.size())
{
  auto nf1 = mFaultList1.size();
  for ( SizeType i = 0; i < nf1; ++ i ) {
    auto fault = mFaultList1[i];
    mFaultMap1.emplace(fault.id(), i);
    auto cond = fault.ffr_propagate_condition();
    auto lits = conv_to_literal_list(cond);
    auto pvar = prop_var1();
    lits.push_back(pvar);
    mDLitsArray1[i] = lits;
    auto ulit = solver().new_variable(true);
    mULitArray1[i] = ulit;
    std::vector<SatLiteral> tmp_lits;
    tmp_lits.reserve(lits.size() + 1);
    tmp_lits.push_back(~ulit);
    for ( auto lit: lits ) {
      tmp_lits.push_back(~lit);
    }
    solver().add_clause(tmp_lits);
  }

  auto nf2 = mFaultList2.size();
  for ( SizeType i = 0; i < nf2; ++ i ) {
    auto fault = mFaultList2[i];
    mFaultMap2.emplace(fault.id(), i);
    auto cond = fault.ffr_propagate_condition();
    auto lits = conv_to_literal_list(cond);
    auto pvar = prop_var2();
    lits.push_back(pvar);
    mDLitsArray2[i] = lits;
    auto ulit = solver().new_variable(true);
    mULitArray2[i] = ulit;
    std::vector<SatLiteral> tmp_lits;
    tmp_lits.reserve(lits.size() + 1);
    tmp_lits.push_back(~ulit);
    for ( auto lit: lits ) {
      tmp_lits.push_back(~lit);
    }
    solver().add_clause(tmp_lits);
  }
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
  auto pos1 = mFaultMap1.at(fault1.id());
  auto pos2 = mFaultMap2.at(fault2.id());
  if ( det1 ) {
    if ( det2 ) {
      auto tmp_lits = concat_lits(mDLitsArray1[pos1], mDLitsArray2[pos2]);
      return solver().solve(tmp_lits);
    }
    else {
      auto tmp_lits = mDLitsArray1[pos1];
      tmp_lits.push_back(mULitArray2[pos2]);
      return solver().solve(tmp_lits);
    }
  }
  else {
    if ( det2 ) {
      auto tmp_lits = mDLitsArray2[pos2];
      tmp_lits.push_back(mULitArray1[pos1]);
      return solver().solve(tmp_lits);
    }
    else {
      std::vector<SatLiteral> tmp_lits{mULitArray1[pos1], mULitArray2[pos2]};
      return solver().solve(tmp_lits);
    }
  }
}

END_NAMESPACE_DRUID
