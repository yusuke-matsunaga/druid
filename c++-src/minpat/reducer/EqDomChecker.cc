
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
void
EqDomChecker::check_equiv(
  EqDomMgr* mgr,
  SizeType group_id,
  const ConfigParam& option
)
{
  auto fault_list = mgr->fault_list(group_id);
  auto nf = fault_list.size();
  if ( nf <= 1 ) {
    return;
  }

  SizeType TIME_LIMIT = option.get_int_elem("time_limit", 0);

  // とりあえずリファレンス用に単純なアルゴリズムを用いる．
  bool has_tv = false;
  for ( SizeType i1 = 0; i1 < nf - 1; ++ i1 ) {
    auto fault1 = fault_list[i1];
    if ( !mgr->is_rep(fault1) ) {
      continue;
    }
    for ( SizeType i2 = i1 + 1; i2 < nf; ++ i2 ) {
      auto fault2 = fault_list[i2];
      if ( !mgr->is_rep(fault2) ) {
	continue;
      }

      NaiveDualEngine engine(fault1, fault2, option);
      auto res1 = engine.solve(true, false, TIME_LIMIT);
      ++ mCheckCount;
      if ( res1 == SatBool3::False ) {
	// fault2 は fault1 に支配されている．
	mgr->set_rep(fault2, fault1);
	++ mSuccessCount;
	mChanged = true;
	continue;
      }
      if ( res1 == SatBool3::True ) {
	// この時の入力を求める．
	auto model = engine.solver().model();
	auto pi_assign = engine.get_pi_assign(model);
	auto tv = TestVector(pi_assign);
	mTvList.push_back(tv);
	has_tv = true;
      }

      auto res2 = engine.solve(false, true, TIME_LIMIT);
      if ( res2 == SatBool3::False ) {
	// fault1 は fault2 に支配されている．
	mgr->set_rep(fault1, fault2);
	++ mSuccessCount;
	mChanged = true;
	break;
      }
      if ( res2 == SatBool3::True ) {
	// この時の入力を求める．
	auto model = engine.solver().model();
	auto pi_assign = engine.get_pi_assign(model);
	auto tv = TestVector(pi_assign);
	mTvList.push_back(tv);
	has_tv = true;
      }
      // ここに来たということは fault1 と fault2 の関係は不明
      if ( has_tv ) {
	// 反例があるので一旦戻る．
	return;
      }
    }
  }
}

// @brief 支配故障のチェックを行う．
void
EqDomChecker::check_dominance(
  EqDomMgr* mgr,
  const TpgFault& fault,
  const ConfigParam& option
)
{
  SizeType TIME_LIMIT = option.get_int_elem("time_limit", 0);

  // 支配故障の候補を一つ取り出す．
  auto dom_fault = mgr->domcand(fault);
  if ( !dom_fault.is_valid() ) {
    return;
  }

  NaiveDualEngine engine(dom_fault, fault, option);
  auto res = engine.solve(true, false, TIME_LIMIT);
  ++ mCheckCount;
  if ( res == SatBool3::False ) {
    mgr->set_rep(fault, dom_fault);
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
  }
}

END_NAMESPACE_DRUID
