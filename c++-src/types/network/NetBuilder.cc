
/// @file NetBuilder.cc
/// @brief NetBuilder の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "NetBuilder.h"
#include "NetworkRep.h"
#include "NodeRep.h"
#include "GateType.h"
#include "GateRep.h"
#include "FaultRep.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス NetBuilder
//////////////////////////////////////////////////////////////////////

// @brief 初期化する．
void
NetBuilder::init(
  SizeType input_num,
  SizeType output_num,
  SizeType dff_num,
  SizeType gate_num,
  SizeType extra_node_num
)
{
  _check_network();
  mNetwork->set_size(input_num, output_num, dff_num, gate_num, extra_node_num);
}

// @brief 入力ノードを生成する．
NodeRep*
NetBuilder::make_input_node(
  const std::string& name
)
{
  _check_network();
  auto node = mNetwork->make_input_node(name);
  return node;
}

// @brief DFFの出力ノードを生成する．
NodeRep*
NetBuilder::make_dff_output_node(
  const std::string& name
)
{
  _check_network();
  auto node = mNetwork->make_dff_output_node(name);
  return node;
}

// @brief 出力ノードを生成する．
NodeRep*
NetBuilder::make_output_node(
  const std::string& name,
  const NodeRep* inode
)
{
  _check_network();
  auto node = mNetwork->make_output_node(name, inode);
  add_fanout(inode, node);
  return node;
}

// @brief DFFの入力ノードを生成する．
NodeRep*
NetBuilder::make_dff_input_node(
  SizeType dff_id,
  const std::string& name,
  const NodeRep* inode
)
{
  _check_network();
  auto node = mNetwork->make_dff_input_node(dff_id, name, inode);
  add_fanout(inode, node);
  return node;
}

// @brief 組込み型の GateType を生成し，登録する．
const GateType*
NetBuilder::make_gate_type(
  SizeType input_num,
  PrimType prim_type
)
{
  _check_network();
  return mNetwork->make_gate_type(input_num, prim_type);
}

// @brief 複合型の GateType を生成し，登録する．
const GateType*
NetBuilder::make_gate_type(
  SizeType input_num,
  const Expr& expr
)
{
  _check_network();
  return mNetwork->make_gate_type(input_num, expr);
}

// @brief GateType に対応する GateRep を作る．
GateRep*
NetBuilder::make_gate(
  const GateType* gate_type,
  const std::vector<const NodeRep*>& fanin_list
)
{
  _check_network();
  NodeRep* node = nullptr;
  std::vector<GateRep::BranchInfo> branch_info;
  if ( gate_type->is_primitive() ) {
    // 組み込み型の場合．
    auto prim_type = gate_type->primitive_type();
    node = make_prim_node(prim_type, fanin_list);
  }
  else {
    auto expr = gate_type->expr();

    // ブランチの情報
    auto ni = fanin_list.size();
    branch_info.resize(ni);

    // 論理式の葉(リテラル)に対応するノードを入れる配列．
    // pos * 2 + 0: 肯定のリテラル
    // pos * 2 + 1: 否定のリテラルに対応する．
    std::vector<const NodeRep*> leaf_nodes(ni * 2, nullptr);
    for ( auto i: Range(ni) ) {
      auto p_num = expr.literal_num(i, false);
      auto n_num = expr.literal_num(i, true);
      auto inode = fanin_list[i];
      if ( n_num == 0 ) {
	if ( p_num == 1 ) {
	  // 肯定のリテラルが1回だけ現れている場合
	  // 本当のファンインを直接つなぐ
	  leaf_nodes[i * 2 + 0] = inode;
	  // この時点では branch_info に設定すべきノードが
	  // 生成されていないので空にしておく．
	  // make_cplx_node 中で設定される．
	}
	else {
	  // 肯定のリテラルが2回以上現れている場合
	  // ブランチの故障に対応するためにダミーのバッファをつくる．
	  auto dummy_buff = make_buff_node(inode);
	  leaf_nodes[i * 2 + 0] = dummy_buff;
	  // このバッファの入力が故障位置となる．
	  branch_info[i] = GateRep::BranchInfo{dummy_buff, 0};
	}
      }
      else {
	if ( p_num > 0 ) {
	  // 肯定と否定のリテラルがともに現れる場合
	  // ブランチの故障に対応するためにダミーのバッファを作る．
	  auto dummy_buff = make_buff_node(inode);
	  inode = dummy_buff;
	  leaf_nodes[i * 2 + 0] = dummy_buff;
	}

	// 否定のリテラルに対応するNOTゲートを作る．
	auto not_gate = make_not_node(inode);
	leaf_nodes[i * 2 + 1] = not_gate;

	if ( p_num == 0 ) {
	  inode = not_gate;
	}

	branch_info[i] = GateRep::BranchInfo{inode, 0};
      }
    }

    // expr の内容を表す TpgNode の木を作る．
    node = make_cplx_node(expr, leaf_nodes, branch_info);
  }
  return mNetwork->make_gate(gate_type, node, branch_info);
}

// @brief 終了処理を行って結果の TpgNetwork を返す．
TpgNetwork
NetBuilder::wrap_up()
{
  _check_network();

  //////////////////////////////////////////////////////////////////////
  // ファンアウトをセットする．
  //////////////////////////////////////////////////////////////////////
  for ( auto& node_ptr: mNetwork->node_list() ) {
    auto from = node_ptr.get();
    if ( mConnectionList.count(from->id()) > 0 ) {
      const auto& fo_list = mConnectionList.at(from->id());
      from->set_fanouts(fo_list);
    }
  }
  mConnectionList.clear();

  // 残りの後処理を行う．
  mNetwork->post_op();

  // 結果のネットワークを返す．
  // と同時に mNetwork を空にする．
  std::shared_ptr<NetworkRep> dummy;
  std::swap(dummy, mNetwork);
  return TpgNetwork(dummy);
}

// @brief 組み込み型の論理ゲートを生成する．
NodeRep*
NetBuilder::make_prim_node(
  PrimType type,
  const std::vector<const NodeRep*>& fanin_list
)
{
  auto node = mNetwork->make_prim_node(type, fanin_list);
  for ( auto inode: fanin_list ) {
    add_fanout(inode, node);
  }
  return node;
}

// @brief 論理式から TpgNode の木を生成する．
NodeRep*
NetBuilder::make_cplx_node(
  const Expr& expr,
  const std::vector<const NodeRep*>& leaf_nodes,
  std::vector<GateRep::BranchInfo>& branch_info
)
{
  // expr はリテラルではない．
  if ( expr.is_literal() ) {
    throw std::invalid_argument{"expr should not be aliteral"};
  }

  // 子供の論理式を表すノード(の木)を作る．
  // fanins[] を確保するオーバーヘッドがあるが，
  // 子供のノードよりも先に親のノードを確保するわけには行かない．
  std::vector<const NodeRep*> fanins;
  fanins.reserve(expr.operand_num());
  for ( auto expr1: expr.operand_list() ) {
    const NodeRep* inode;
    if ( expr1.is_positive_literal() ) {
      auto iid = expr1.varid();
      inode = leaf_nodes[iid * 2 + 0];
    }
    else if ( expr1.is_negative_literal() ) {
      auto iid = expr1.varid();
      inode = leaf_nodes[iid * 2 + 1];
    }
    else {
      inode = make_cplx_node(expr1, leaf_nodes, branch_info);
    }
    if ( inode == nullptr ) {
      throw std::logic_error{"inode == nullptr"};
    }
    fanins.push_back(inode);
  }

  PrimType gate_type;
  if ( expr.is_and() ) {
    gate_type = PrimType::And;
  }
  else if ( expr.is_or() ) {
    gate_type = PrimType::Or;
  }
  else if ( expr.is_xor() ) {
    gate_type = PrimType::Xor;
  }
  else {
    throw std::logic_error{"Never happened"};
  }
  auto node = make_prim_node(gate_type, fanins);

  // オペランドがリテラルの場合，branch_info
  // の設定を行う．
  SizeType ipos = 0;
  for ( auto expr1: expr.operand_list() ) {
    // 美しくないけどスマートなやり方を思いつかない．
    if ( expr1.is_positive_literal() ) {
      auto iid = expr1.varid();
      if ( branch_info[iid].node == nullptr ) {
	branch_info[iid] = GateRep::BranchInfo{node, ipos};
      }
    }
    ++ ipos;
  }

  return node;
}

// @brief ファンアウトの接続を追加する．
void
NetBuilder::add_fanout(
  const NodeRep* node,
  const NodeRep* fo_node
)
{
  if ( mConnectionList.count(node->id()) == 0 ) {
    mConnectionList.emplace(node->id(), std::vector<const NodeRep*>{});
  }
  mConnectionList.at(node->id()).push_back(fo_node);
}

// @brief mNetwork が空なら生成する．
void
NetBuilder::_check_network()
{
  if ( mNetwork.get() == nullptr ) {
    mNetwork = std::unique_ptr<NetworkRep>{new NetworkRep(mFaultType)};
  }
}

END_NAMESPACE_DRUID
