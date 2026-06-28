
/// @file DomChecker.cc
/// @brief DomChecker の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "DomChecker.h"
#include "dtpg/NaiveDualEngine.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス DomChecker
//////////////////////////////////////////////////////////////////////

// @brief 支配故障のチェックを行う．
void
DomChecker::check_dominance(
  DomMgr& dommgr,
  const TpgFault& fault,
  const ConfigParam& option
)
{
  SizeType TIME_LIMIT = option.get_int_elem("time_limit", 0);

  // 支配故障の候補を一つ取り出す．
  for ( auto dom_fault: dommgr.cand_list(fault) ) {
    if ( !dommgr.fault_info().is_rep(dom_fault) ) {
      continue;
    }

    NaiveDualEngine engine(dom_fault, fault, option);
    auto res = engine.solve(true, false, TIME_LIMIT);
    ++ mCheckCount;
    if ( res == SatBool3::False ) {
      dommgr.fault_info().set_dominator(fault, dom_fault);
      ++ mSuccessCount;
      mChanged = true;
      return;
    }
    if ( res == SatBool3::True ) {
      // この時の入力を求める．
      auto model = engine.solver().model();
      auto pi_assign = engine.get_pi_assign(model);
      auto tv = TestVector(pi_assign);
      mTvList.push_back(tv);
      return;
    }
  }
}

END_NAMESPACE_DRUID
