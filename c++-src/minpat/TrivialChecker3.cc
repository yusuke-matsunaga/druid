
/// @file TrivialChecker3.cc
/// @brief TrivialChecker3 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "TrivialChecker3.h"

#include "TpgNetwork.h"
#include "TpgFFR.h"
#include "TpgFault.h"
#include "TpgNodeSet.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
TrivialChecker3::TrivialChecker3(
  const TpgNetwork& network,
  const vector<const TpgFault*>& fault1_list,
  const TpgFFR* ffr2,
  const JsonValue& option
) : mEngine{network, option}
{
  mBdEnc2 = new BoolDiffEnc{mEngine, ffr2->root(), option};
  vector<bool> mark(network.node_num(), false);
  vector<const TpgNode*> node_list;
  node_list.push_back(ffr2->root());
  mark[ffr2->root()->id()] = true;
  for ( auto fault: fault1_list ) {
    auto node = fault->ffr_root();
    if ( !mark[node->id()] ) {
      mark[node->id()] = true;
      node_list.push_back(node);
    }
  }
  auto tfo_list = TpgNodeSet::get_tfo_list(network.node_num(), node_list,
					   [&](const TpgNode*){});
  mEngine.make_cnf(tfo_list, tfo_list);
}

// @brief デストラクタ
TrivialChecker3::~TrivialChecker3()
{
}

// @brief チェックする．
bool
TrivialChecker3::check(
  const AssignList& assignments
)
{
  auto assumptions = mEngine.conv_to_literal_list(assignments);
  auto pvar = mBdEnc2->prop_var();
  assumptions.push_back(~pvar);
  return mEngine.solver().solve(assumptions) == SatBool3::False;
}

END_NAMESPACE_DRUID
