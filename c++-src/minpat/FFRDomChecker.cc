
/// @file FFRDomChecker.cc
/// @brief FFRDomChecker の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "FFRDomChecker.h"
#include "types/TpgFault.h"
#include "types/TpgFaultList.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// FFRDomChecker
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
FFRDomChecker::FFRDomChecker(
  const TpgFFR& ffr,
  const TpgFaultList& fault_list,
  std::vector<bool>& del_mark,
  const JsonValue& option
) : mFaultList{fault_list},
    mDelMark{del_mark},
    mEngine(ffr.network(), ffr.root(), option)
{
  auto pvar = mEngine.prop_var();
  mEngine.solver().add_clause(pvar);
  for ( auto fault: fault_list ) {
    if ( mDelMark[fault.id()] ) {
      continue;
    }
    auto cond = fault.ffr_propagate_condition();
    auto clit = mEngine.new_variable(true);
    std::vector<SatLiteral> tmp_lits;
    tmp_lits.reserve(cond.size() + 1);
    tmp_lits.push_back(~clit);
    for ( auto nv: cond ) {
      auto lit = mEngine.conv_to_literal(nv);
      tmp_lits.push_back(~lit);
    }
    mEngine.solver().add_clause(tmp_lits);
    mLitDict.emplace(fault.id(), clit);
  }
}

// @brief 故障の支配関係を調べる．
void
FFRDomChecker::run()
{
  auto nf = mFaultList.size();
  for ( SizeType i1 = 0; i1 < nf - 1; ++ i1 ) {
    auto fault1 = mFaultList[i1];
    if ( mDelMark[fault1.id()]) {
      continue;
    }
    for ( SizeType i2 = i1 + 1; i2 < nf; ++ i2 ) {
      auto fault2 = mFaultList[i2];
      if ( mDelMark[fault2.id()] ) {
	continue;
      }
      if ( _check(fault1, fault2) ) {
	// fault2 を除外
	mDelMark[fault2.id()] = true;
      }
      else if ( _check(fault2, fault1) ) {
	// fault1 を除外
	mDelMark[fault1.id()] = true;
	break;
      }
    }
  }
}

// @brief fault1 が fault2 を支配している時 true を返す．
bool
FFRDomChecker::_check(
  const TpgFault& fault1,
  const TpgFault& fault2
)
{
  // fault1 の検出条件
  auto cond1 = fault1.ffr_propagate_condition();
  auto assumptions = mEngine.conv_to_literal_list(cond1);
  // fault2 の非検出条件を表すリテラル
  auto clit = mLitDict.at(fault2.id());
  // cond1 を満たし，cond2 を満たさない解が存在する時
  // fault1 のみが検出可能で fault2 が検出不能のテストパタンが存在する．
  // 逆に上記の問題が UNSAT の場合，fault1 が検出可能なら常に
  // fault2 も検出可能であることがわかる．
  // ただし，cond2 の否定は CNF 節になってしまうので制御用の変数を
  // 付加しておく
  assumptions.push_back(clit);
  auto res = mEngine.solve(assumptions);
  return res == SatBool3::False;
}

END_NAMESPACE_DRUID
