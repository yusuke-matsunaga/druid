
/// @file RefSim.cc
/// @brief RefSim の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "RefSim.h"
#include "RefNode.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
RefSim::RefSim(
  const TpgNetwork& network,
  bool has_previous_state
) : mNodeMap(network.node_num(), nullptr)
{
  auto nn = network.node_num();

  // 入力ノードを作る．
  for ( auto tpg_node: network.input_list() ) {
    auto node = new RefNode(PrimType::None, {});
    mNodeMap[tpg_node->id()] = node;
    mInputList.push_back(node);
  }

  // 出力側からノードを作る．
  for ( auto tpg_node: network.output_list() ) {
    auto node = make_node(tpg_node);
    mOutputList1.push_back(node);
  }
}

// @brief デストラクタ
RefSim::~RefSim()
{
  for ( auto node: mNodeMap ) {
    delete node;
  }
}

// @brief 故障シミュレーションを行う．
DiffBits
RefSim::simulate(
  const TestVector& tv,
  const TpgFault& fault
)
{
  // 入力に値を設定する．
  SizeType ni = mInputList.size();
  for ( SizeType i = 0; i < ni; ++ i ) {
    auto node = mInputList[i];
    auto val = tv.ppi_val(i);
    node->set_gval(val);
  }
  // 正常値を計算する．
  for ( auto node: mLogicList ) {
    node->calc_gval();
  }
  // 故障値を計算する．
  auto fnode = mNodeMap[fault->origin_node()->id()];
  for ( auto node: mInputList ) {
    auto val = node->get_gval();
    if ( check(fault) ) {
      val = ~val;
    }
    node->set_fval(val);
  }
  for ( auto node: mLogicList ) {
    auto val = node->calc_fval();
    if ( check(fault) ) {
      val = ~val;
    }
    node->set_fval(val);
  }
  SizeType no = mOutputList.size();
  DiffBits dbits(no);
  for ( SizeType i = 0; i < no; ++ i ) {
    auto node = mOutputList[i];
    if ( node->get_gval() != node->get_fval() ) {
      dbits.set_val(i, true);
    }
  }
  return dbits;
}

// @brief TpgNode に対応する RefNode を作る．
RefNode*
RefSim::make_node(
  const TpgNode* tpg_node
)
{
  auto node = mNodeMap[tpg_node->id()];
  if ( node == nullptr ) {
    // 未生成
    SizeType ni = tpg_node->fanin_num();
    vector<RefNode*> fanin_list(ni);
    for ( SizeType i = 0; i < ni; ++ i ) {
      fanin_list[i] = make_node(tpg_node->fanin(i), node_map);
    }
    node = new RefNode{tpg_node->gate_type(), fanin_list};
    mNodeMap[tpg_node->id()] = node;
    mLogicList.push_back(node);
  }
  return node;
}

END_NAMESPACE_DRUID
