
/// @file CondGenChecker.cc
/// @brief CondGenChecker の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "CondGenChecker.h"
#include "CnfGenMgr.h"
#include "TpgFFR.h"
#include "TpgFault.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
CondGenChecker::CondGenChecker(
  const TpgNetwork& network,
  const TpgFFR* ffr,
  const JsonValue& option
) : mEngine{network, option},
    mBdEnc{new BoolDiffEnc(mEngine, ffr->root(), option)}
{
  mEngine.make_cnf({}, {ffr->root()});
}

// @breif 結果の検証を行う．
bool
CondGenChecker::check(
  const AssignList& extra_cond,
  const DetCond& cond
)
{
  auto size0 = mEngine.solver().cnf_size();
  auto assumptions = CnfGenMgr::make_cnf(mEngine, cond);
  auto extra_lits = mEngine.conv_to_literal_list(extra_cond);
  assumptions.insert(assumptions.end(), extra_lits.begin(), extra_lits.end());
  auto pvar = mBdEnc->prop_var();
  assumptions.push_back(~pvar);
  auto res = mEngine.solver().solve(assumptions);
  if ( res != SatBool3::False ) {
    cout << mCond.expr() << endl;
  }
#if 0
  auto size1 = mEngine.solver().cnf_size();
  auto real_size = size1 - size0;
  auto size = CnfGenMgr::calc_cnf_size(mEngine.network(), cond);
  if ( size != real_size ) {
    cout << "real_size: " << real_size << endl
	 << "calc_size: " << size << endl;
    //cout << "expr: " << cond.expr() << endl;
    return false;
  }
#endif
  return res == SatBool3::False;
}

END_NAMESPACE_DRUID
