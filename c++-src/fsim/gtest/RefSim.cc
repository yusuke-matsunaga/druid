
/// @file RefSim.cc
/// @brief RefSim の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "RefSim.h"
#include "RefNode.h"
#include "types/TpgNetwork.h"
#include "types/TpgNode.h"
#include "types/TpgFault.h"
#include "types/TestVector.h"
#include "types/AssignList.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
RefSim::RefSim(
  const TpgNetwork& network
) : mNetwork{network},
    mFaultType{network.fault_type()},
    mNodeMap(network.node_num(), nullptr)
{
  auto nn = network.node_num();

  // 入力ノードを作る．
  for ( SizeType i = 0; i < network.input_num(); ++ i ) {
    auto tpg_node = network.input(i);
    auto node = new RefNode(tpg_node.id(), PrimType::None, {});
    mNodeMap[tpg_node.id()] = node;
    mInputList.push_back(node);
  }

  // DFFの出力ノードを作る．
  for ( SizeType i = 0; i < network.dff_num(); ++ i ) {
    auto tpg_node = network.dff_output(i);
    auto node = new RefNode(tpg_node.id(), PrimType::None, {});
    mNodeMap[tpg_node.id()] = node;
    mDffOutList.push_back(node);
  }

  // 出力側からノードを作る．
  for ( SizeType i = 0; i < network.output_num(); ++ i ) {
    auto tpg_node = network.output(i);
    auto node = make_node(tpg_node);
    mOutputList.push_back(node);
  }

  // DFFの入力ノードを作る．
  for ( SizeType i = 0; i < network.dff_num(); ++ i ) {
    auto tpg_node = network.dff_input(i);
    auto node = make_node(tpg_node);
    mDffInList.push_back(node);
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
  SizeType fid
)
{
  switch ( mFaultType ) {
  case FaultType::StuckAt: return simulate_sa(tv, fid);
  case FaultType::TransitionDelay: return simulate_td(tv, fid);
  default: break;
  }
  // ダミー
  return DiffBits{};
}

// @brief 縮退故障用の故障シミュレーションを行う．
DiffBits
RefSim::simulate_sa(
  const TestVector& tv,
  SizeType fid
)
{
  // 入力に値を設定する．
  SizeType ni = mInputList.size();
  for ( SizeType i = 0; i < ni; ++ i ) {
    auto node = mInputList[i];
    auto val = tv.ppi_val(i);
    node->set_gval(val);
  }
  SizeType nd = mDffOutList.size();
  for ( SizeType i = 0; i < nd; ++ i ) {
    auto node = mDffOutList[i];
    auto val = tv.ppi_val(i + ni);
    node->set_gval(val);
   }

  // 正常値を計算する．
  for ( auto node: mLogicList ) {
    auto val = node->calc_gval();
  }

  // 故障値を計算する．
  for ( auto node: mInputList ) {
    auto val = node->get_gval();
    if ( check_fault_cond(fid, node) ) {
      val = ~val;
    }
    node->set_fval(val);
  }
  for ( auto node: mDffOutList ) {
    auto val = node->get_gval();
    if ( check_fault_cond(fid, node) ) {
      val = ~val;
    }
    node->set_fval(val);
  }

  for ( auto node: mLogicList ) {
    auto val = node->calc_fval();
    if ( check_fault_cond(fid, node) ) {
      val = ~val;
    }
    node->set_fval(val);
  }

  // 出力結果を比較する．
  SizeType no = mOutputList.size();
  DiffBits dbits;
  for ( SizeType i = 0; i < no; ++ i ) {
    auto node = mOutputList[i];
    if ( node->get_gval() != node->get_fval() ) {
      dbits.add_output(i);
    }
  }
  for ( SizeType i = 0; i < nd; ++ i ) {
    auto node = mDffInList[i];
    if ( node->get_gval() != node->get_fval() ) {
      dbits.add_output(i + no);
    }
  }
  return dbits;
}

// @brief 遷移故障用の故障シミュレーションを行う．
DiffBits
RefSim::simulate_td(
  const TestVector& tv,
  SizeType fid
)
{
  // 入力に値を設定する．
  SizeType ni = mInputList.size();
  for ( SizeType i = 0; i < ni; ++ i ) {
    auto node = mInputList[i];
    auto val = tv.ppi_val(i);
    node->set_gval(val);
    node->shift_gval();
   }
  SizeType nd = mDffOutList.size();
  for ( SizeType i = 0; i < nd; ++ i ) {
    auto node = mDffOutList[i];
    auto val = tv.ppi_val(i + ni);
    node->set_gval(val);
    node->shift_gval();
   }

  // 正常値を計算しシフトする．
  for ( auto node: mLogicList ) {
    node->calc_gval();
    node->shift_gval();
  }

  // 入力に2時刻目の値を設定する．
  for ( SizeType i = 0; i < ni; ++ i ) {
    auto node = mInputList[i];
    auto val = tv.aux_input_val(i);
    node->set_gval(val);
  }
  // DFF の出力に1時刻目の入力の値を入れる．
  for ( SizeType i = 0; i < nd; ++ i ) {
    auto node1 = mDffInList[i];
    auto node2 = mDffOutList[i];
    auto val = node1->get_hval();
    node2->set_gval(val);
  }

  // 2時刻目の正常値を計算する．
  for ( auto node: mLogicList ) {
    node->calc_gval();
  }

  // 故障値を計算する．
  for ( auto node: mInputList ) {
    auto val = node->get_gval();
    if ( check_fault_cond(fid, node) ) {
      val = ~val;
    }
    node->set_fval(val);
  }
  for ( auto node: mDffOutList ) {
    auto val = node->get_gval();
    if ( check_fault_cond(fid, node) ) {
      val = ~val;
    }
    node->set_fval(val);
  }

  for ( auto node: mLogicList ) {
    auto val = node->calc_fval();
    if ( check_fault_cond(fid, node) ) {
      val = ~val;
    }
    node->set_fval(val);
  }

  // 出力結果を比較する．
  SizeType no = mOutputList.size();
  DiffBits dbits;
  for ( SizeType i = 0; i < no; ++ i ) {
    auto node = mOutputList[i];
    if ( node->get_gval() != node->get_fval() ) {
      dbits.add_output(i);
    }
  }
  for ( SizeType i = 0; i < nd; ++ i ) {
    auto node = mDffInList[i];
    if ( node->get_gval() != node->get_fval() ) {
      dbits.add_output(i + no);
    }
  }
  return dbits;
}

// @brief TpgNode に対応する RefNode を作る．
RefNode*
RefSim::make_node(
  const TpgNode& tpg_node
)
{
  auto node = mNodeMap[tpg_node.id()];
  if ( node == nullptr ) {
    // 未生成
    SizeType ni = tpg_node.fanin_num();
    std::vector<RefNode*> fanin_list(ni);
    for ( SizeType i = 0; i < ni; ++ i ) {
      fanin_list[i] = make_node(tpg_node.fanin(i));
    }
    node = new RefNode{tpg_node.id(), tpg_node.gate_type(), fanin_list};
    mNodeMap[tpg_node.id()] = node;
    mLogicList.push_back(node);
  }
  return node;
}

// @brief 故障の活性化条件をチェックする．
bool
RefSim::check_fault_cond(
  SizeType fid,
  RefNode* node
) const
{
  auto fault = mNetwork.fault(fid);
  auto onode = mNodeMap[fault.origin_node().id()];
  if ( onode != node ) {
    return false;
  }
  for ( auto nv: fault.excitation_condition() ) {
    auto node1 = mNodeMap[nv.node().id()];
    Val3 val;
    if ( nv.time() == 1 ) {
      val = node1->get_gval();
    }
    else {
      val = node1->get_hval();
    }
    if ( nv.val() ) {
      if ( val != Val3::_1 ) {
	return false;
      }
    }
    else {
      if ( val != Val3::_0 ) {
	return false;
      }
    }
  }
  return true;
}

END_NAMESPACE_DRUID
