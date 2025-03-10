
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

// @breif 結果の検証を行う．
bool
CondGenChecker::check(
  const TpgNetwork& network,
  const TpgFFR* ffr,
  const DetCond& cond,
  const JsonValue& option
)
{
  auto bd_enc = new BoolDiffEnc(ffr->root(), option);
  StructEngine engine(network, option);
  engine.add_subenc(std::unique_ptr<SubEnc>{bd_enc});
  engine.add_prev_node(ffr->root());

  auto assumptions = CnfGenMgr::make_cnf(engine, cond);
  auto pvar = bd_enc->prop_var();
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
