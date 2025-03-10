
/// @file TrivialChecker1.cc
/// @brief TrivialChecker1 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "TrivialChecker1.h"

#include "TpgNetwork.h"
#include "TpgFFR.h"
#include "TpgFault.h"
#include "TpgNodeSet.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
TrivialChecker1::TrivialChecker1(
  const TpgNetwork& network,
  const vector<const TpgFault*>& fault_list,
  const JsonValue& option
) : mEngine(network, option)
{
  vector<bool> mark(network.node_num(), false);
  vector<const TpgNode*> node_list;
  for ( auto fault: fault_list ) {
    auto node = fault->ffr_root();
    if ( !mark[node->id()] ) {
      mark[node->id()] = true;
      node_list.push_back(node);
    }
  }
  auto tfo_list = TpgNodeSet::get_tfo_list(network.node_num(), node_list,
					   [&](const TpgNode*){});
  mEngine.add_cur_node_list(tfo_list);
  mEngine.add_prev_node_list(tfo_list);
}

// @brief デストラクタ
TrivialChecker1::~TrivialChecker1()
{
}

// @brief チェックする．
bool
TrivialChecker1::check(
  const AssignList& cond1,
  const AssignList& cond2
)
{
  auto assumptions = mEngine.conv_to_literal_list(cond1);
  assumptions.push_back(SatLiteral::X);
  for ( auto nv: cond2 ) {
    auto lit = mEngine.conv_to_literal(nv);
    assumptions.back() = ~lit;
    if ( mEngine.solve(assumptions) != SatBool3::False ) {
      return false;
    }
  }
  return true;
}

END_NAMESPACE_DRUID
