
/// @file TrivialChecker2.cc
/// @brief TrivialChecker2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "TrivialChecker2.h"

#include "TpgNetwork.h"
#include "TpgFFR.h"
#include "TpgFault.h"
#include "TpgNodeSet.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
TrivialChecker2::TrivialChecker2(
  const TpgNetwork& network,
  const TpgFFR* ffr1,
  const vector<const TpgFault*>& fault2_list,
  const JsonValue& option
) : mBaseEnc{network, option}
{
  mBdEnc1 = new BoolDiffEnc{mBaseEnc, ffr1->root(), option};
  vector<bool> mark(network.node_num(), false);
  vector<const TpgNode*> node_list;
  node_list.push_back(ffr1->root());
  mark[ffr1->root()->id()] = true;
  for ( auto fault: fault2_list ) {
    auto node = fault->ffr_root();
    if ( !mark[node->id()] ) {
      mark[node->id()] = true;
      node_list.push_back(node);
    }
  }
  auto tfo_list = TpgNodeSet::get_tfo_list(network.node_num(), node_list,
					   [&](const TpgNode*){});
  mBaseEnc.make_cnf(tfo_list, tfo_list);
}

// @brief デストラクタ
TrivialChecker2::~TrivialChecker2()
{
}

// @brief チェックする．
bool
TrivialChecker2::check(
  const TpgFault* fault1,
  const TpgFault* fault2,
  const NodeTimeValList& cond2
)
{
  auto cond1 = fault1->ffr_propagate_condition();
  auto assumptions = mBaseEnc.conv_to_literal_list(cond1);
  auto pvar = mBdEnc1->prop_var();
  assumptions.push_back(pvar);
  SatLiteral clit;
  if ( mVarMap.count(fault2->id()) == 0 ) {
    clit = mBaseEnc.solver().new_variable(true);
    mVarMap.emplace(fault2->id(), clit);
    vector<SatLiteral> tmp_lits;
    tmp_lits.reserve(cond2.size() + 2);
    tmp_lits.push_back(~clit);
    tmp_lits.push_back(~pvar);
    for ( auto nv: cond2 ) {
      auto lit = mBaseEnc.conv_to_literal(nv);
      tmp_lits.push_back(~lit);
    }
    mBaseEnc.solver().add_clause(tmp_lits);
  }
  else {
    clit = mVarMap.at(fault2->id());
  }
  assumptions.push_back(clit);
  return mBaseEnc.solver().solve(assumptions) == SatBool3::False;
}

END_NAMESPACE_DRUID
