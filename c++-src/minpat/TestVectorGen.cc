
/// @file TestVectorGen.cc
/// @brief TestVectorGen の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "TestVectorGen.h"
#include "TestVector.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
TestVectorGen::TestVectorGen(
  const TpgNetwork& network,
  const JsonValue& option
) : mNetwork{network},
    mBaseEnc{network, option}
{
  auto& node_list = network.node_list();
  mBaseEnc.make_cnf(node_list, node_list);
}

// @brief 値割り当てからテストベクタを作る．
TestVector
TestVectorGen::generate(
  const NodeTimeValList& assignments
)
{
  auto assumptions = mBaseEnc.conv_to_literal_list(assignments);
  auto res = mBaseEnc.solver().solve(assumptions);
  if ( res != SatBool3::True ) {
    throw std::invalid_argument{"wrong assignments"};
  }

  NodeTimeValList pi_assign;
  if ( mNetwork.has_prev_state() ) {
    for ( auto node: mNetwork.ppi_list() ) {
      auto v = mBaseEnc.val(node, 0);
      pi_assign.add(node, 0, v);
    }
    for ( auto node: mNetwork.input_list() ) {
      auto v = mBaseEnc.val(node, 1);
      pi_assign.add(node, 1, v);
    }
  }
  else {
    for ( auto node: mNetwork.ppi_list() ) {
      auto v = mBaseEnc.val(node, 1);
      pi_assign.add(node, 1, v);
    }
  }
  return TestVector{mNetwork, pi_assign};
}

END_NAMESPACE_DRUID
