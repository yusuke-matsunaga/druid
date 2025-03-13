
/// @file CondGenChecker.cc
/// @brief CondGenChecker の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "CondGenChecker.h"
#include "CondGenMgr.h"
#include "TpgFFR.h"
#include "TpgFault.h"
#include "BdEngine.h"


BEGIN_NAMESPACE_DRUID

// @breif 結果の検証を行う．
bool
CondGenChecker::check(
  const TpgNetwork& network,
  const TpgFFR* ffr,
  const DetCond& cond,
  const JsonValue& option
)
{
  if ( cond.type() == DetCond::Undetected ) {
    return true;
  }

  BdEngine engine(network, ffr->root(), option);
  engine.add_prev_node(ffr->root());

  auto cond_lits_list = CondGenMgr::make_cnf(engine, {cond}, JsonValue{});
  if ( cond_lits_list.size() != 1 ) {
    cout << "cond_lits_list.size() != 1(" << cond_lits_list.size() << ")" << endl;
    return false;
  }
  auto& cond_lits = cond_lits_list.front();
  if ( !cond_lits.detected ) {
    cout << "cond_lits.detected == false" << endl;
    return false;
  }
  auto assumptions = cond_lits.lits;
  auto pvar = engine.prop_var();
  assumptions.push_back(~pvar);
  auto res = engine.solve(assumptions);
  if ( res != SatBool3::False ) {
    cond.print(cout);
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
