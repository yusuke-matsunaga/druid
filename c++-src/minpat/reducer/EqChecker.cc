
/// @file EqChecker.cc
/// @brief EqChecker の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "EqChecker.h"
#include "dtpg/NaiveDualEngine.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス EqChecker
//////////////////////////////////////////////////////////////////////

// @brief 等価故障のチェックを行う．
void
EqChecker::check_equiv(
  EqGroupMgr* mgr,
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
    if ( !mgr->fault_info().is_rep(fault1) ) {
      continue;
    }
    for ( SizeType i2 = i1 + 1; i2 < nf; ++ i2 ) {
      auto fault2 = fault_list[i2];
      if ( !mgr->fault_info().is_rep(fault2) ) {
	continue;
      }

      NaiveDualEngine engine(fault1, fault2, option);
      auto res1 = engine.solve(true, false, TIME_LIMIT);
      ++ mCheckCount;
      if ( res1 == SatBool3::False ) {
	// fault2 は fault1 に支配されている．
	mgr->fault_info().set_dominator(fault2, fault1);
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
	mgr->fault_info().set_dominator(fault1, fault2);
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

END_NAMESPACE_DRUID
