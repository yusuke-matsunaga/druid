
/// @file EqDomChecker.cc
/// @brief EqDomChecker の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "EqDomChecker.h"
#include "dtpg/NaiveDualEngine.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス EqDomChecker
//////////////////////////////////////////////////////////////////////

// @brief 等価故障のチェックを行う．
bool
EqDomChecker::check_equiv(
  const TpgFaultList& fault_list,
  FaultInfo& fault_info,
  const ConfigParam& option
)
{
  mCheckCount = 0;
  mSuccessCount = 0;
  mTvList.clear();

  auto nf = fault_list.size();
  if ( nf == 1 ) {
    return false;
  }

  SizeType TIME_LIMIT = option.get_int_elem("time_limit", 0);

  // とりあえずリファレンス用に単純なアルゴリズムを用いる．
  bool changed = false;
  for ( SizeType i1 = 0; i1 < nf - 1; ++ i1 ) {
    auto fault1 = fault_list[i1];
    if ( !fault_info.is_rep(fault1) ) {
      continue;
    }
    for ( SizeType i2 = i1 + 1; i2 < nf; ++ i2 ) {
      auto fault2 = fault_list[i2];
      if ( !fault_info.is_rep(fault2) ) {
	continue;
      }

      NaiveDualEngine engine(fault1, fault2, option);
      auto res1 = engine.solve(true, false, TIME_LIMIT);
      ++ mCheckCount;
      if ( res1 == SatBool3::False ) {
	// fault2 は fault1 に支配されている．
	fault_info.set_dominator(fault2, fault1);
	++ mSuccessCount;
	changed = true;
	continue;
      }
      if ( res1 == SatBool3::True ) {
	// この時の入力を求める．
	auto model = engine.solver().model();
	auto pi_assign = engine.get_pi_assign(model);
	auto tv = TestVector(pi_assign);
	mTvList.push_back(tv);
      }

      auto res2 = engine.solve(false, true, TIME_LIMIT);
      if ( res2 == SatBool3::False ) {
	// fault1 は fault2 に支配されている．
	fault_info.set_dominator(fault1, fault2);
	++ mSuccessCount;
	changed = true;
	break;
      }
      if ( res2 == SatBool3::True ) {
	// この時の入力を求める．
	auto model = engine.solver().model();
	auto pi_assign = engine.get_pi_assign(model);
	auto tv = TestVector(pi_assign);
	mTvList.push_back(tv);
      }
      // ここに来たということは fault1 と fault2 の関係は不明
      if ( !mTvList.empty() ) {
	// 反例があるので一旦戻る．
	return changed;
      }
    }
  }
  return changed;
}

// @brief 支配故障のチェックを行う．
bool
EqDomChecker::check_dominance(
  const TpgFault& fault,
  const TpgFault& dom_fault,
  FaultInfo& fault_info,
  const ConfigParam& option
)
{
  SizeType TIME_LIMIT = option.get_int_elem("time_limit", 0);

  mCheckCount = 0;
  mSuccessCount = 0;
  mTvList.clear();

  NaiveDualEngine engine(dom_fault, fault, option);
  auto res = engine.solve(true, false, TIME_LIMIT);
  ++ mCheckCount;
  if ( res == SatBool3::False ) {
    fault_info.set_dominator(fault, dom_fault);
    ++ mSuccessCount;
    return true;
  }
  if ( res == SatBool3::True ) {
    // この時の入力を求める．
    auto model = engine.solver().model();
    auto pi_assign = engine.get_pi_assign(model);
    auto tv = TestVector(pi_assign);
    mTvList.push_back(tv);
  }
  return false;
}

END_NAMESPACE_DRUID
