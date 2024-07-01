
/// @file SimpleDomChecker.cc
/// @brief SimpleDomChecker の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "SimpleDomChecker.h"

#include "TpgNetwork.h"
#include "TpgFFR.h"
#include "TpgFault.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
SimpleDomChecker::SimpleDomChecker(
  const TpgNetwork& network,
  const TpgFFR* ffr1,
  const vector<const TpgFault*>& fault2_list,
  const JsonValue& option
) : mBaseEnc{network, option}
{
  mBdEnc1 = new BoolDiffEnc{mBaseEnc, ffr1->root(), option};
  vector<const TpgNode*> tmp_list;
  tmp_list.push_back(ffr1->root());
  for ( auto fault: fault2_list ) {
    tmp_list.push_back(fault->ffr_root());
  }
  mBaseEnc.make_cnf(tmp_list, tmp_list);
}

// @brief デストラクタ
SimpleDomChecker::~SimpleDomChecker()
{
}

// @brief チェックする．
bool
SimpleDomChecker::check(
  const TpgFault* fault1,
  const TpgFault* fault2
)
{
  ASSERT_COND( fault1->ffr_root() == mBdEnc1->root_node() );
  auto ffr_cond1 = fault1->ffr_propagate_condition();
  auto assumptions = mBaseEnc.conv_to_literal_list(ffr_cond1);
  assumptions.push_back(mBdEnc1->prop_var());
  assumptions.push_back(SatLiteral::X); // プレースホルダ

  SatLiteral clit;
  if ( mCVarMap.count(fault2->id()) == 0 ) {
    auto ffr_cond2 = fault2->ffr_propagate_condition();
    clit = mBaseEnc.solver().new_variable(true);
    mCVarMap.emplace(fault2->id(), clit);
    vector<SatLiteral> tmp_lits;
    tmp_lits.reserve(ffr_cond2.size() + 1);
    tmp_lits.push_back(~clit);
    for ( auto nv: ffr_cond2 ) {
      auto lit = mBaseEnc.conv_to_literal(nv);
      tmp_lits.push_back(~lit);
    }
    mBaseEnc.solver().add_clause(tmp_lits);
  }
  else {
    clit = mCVarMap.at(fault2->id());
  }
  assumptions[assumptions.size() - 1] = clit;
  return mBaseEnc.solver().solve(assumptions) == SatBool3::False;
}

END_NAMESPACE_DRUID
