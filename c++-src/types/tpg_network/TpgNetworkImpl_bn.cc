
/// @File TpgNetworkImpl_bnet.cc
/// @brief TpgNetworkImpl::set(const BnNetwork&) の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2019, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "TpgNetwork.h"
#include "TpgNetworkImpl.h"
#include "TpgNode.h"
#include "TpgPPI.h"
#include "TpgPPO.h"
#include "GateType.h"

#include "NodeMap.h"

#include "ym/BnModel.h"
#include "ym/BnSeq.h"
#include "ym/BnNode.h"
#include "ym/BnCover.h"

#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス TpgNetwork
//////////////////////////////////////////////////////////////////////

// @brief blif ファイルを読み込む．
TpgNetwork
TpgNetwork::read_blif(
  const string& filename,
  const ClibCellLibrary& cell_library,
  const string& clock_name,
  const string& reset_name
)
{
  auto model = BnModel::read_blif(filename, cell_library, clock_name, reset_name);
  return TpgNetwork{model};
}

// @brief iscas89 形式のファイルを読み込む．
TpgNetwork
TpgNetwork::read_iscas89(
  const string& filename,
  const string& clock_name
)
{
  auto model = BnModel::read_iscas89(filename, clock_name);
  return TpgNetwork{model};
}

// @brief BnModel からの変換コンストラクタ
TpgNetwork::TpgNetwork(
  const BnModel& model
) : mImpl{new TpgNetworkImpl{model}}
{
}


BEGIN_NONAMESPACE

void
dfs_mark(
  BnNode node,
  vector<bool>& mark
)
{
  if ( mark[node.id()] ) {
    return;
  }
  mark[node.id()] = true;
  if ( node.is_logic() ) {
    for ( auto inode: node.fanin_list() ) {
      dfs_mark(inode, mark);
    }
  }
}

END_NONAMESPACE


//////////////////////////////////////////////////////////////////////
// クラス TpgNetworkImpl
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
TpgNetworkImpl::TpgNetworkImpl(
  const BnModel& model
)
{
  //////////////////////////////////////////////////////////////////////
  // NodeInfoMgr にノードの論理関数を登録する．
  //////////////////////////////////////////////////////////////////////
  vector<const GateType*> gate_type_list1;
  vector<const GateType*> gate_type_list2;
  gate_type_list1.reserve(model.cover_num());
  gate_type_list2.reserve(model.expr_num());
  for ( SizeType i: Range(model.cover_num()) ) {
    auto& cover = model.cover(i);
    SizeType ni = cover.input_num();
    auto expr = cover.expr();
    auto gate_type = mGateTypeMgr.new_type(ni, expr);
    gate_type_list1.push_back(gate_type);
  }
  for ( SizeType i: Range(model.expr_num()) ) {
    auto expr = model.expr(i);
    SizeType ni = expr.input_size();
    auto gate_type = mGateTypeMgr.new_type(ni, expr);
    gate_type_list2.push_back(gate_type);
  }

  //////////////////////////////////////////////////////////////////////
  // 追加で生成されるノード数を数える．
  //////////////////////////////////////////////////////////////////////
  SizeType extra_node_num = 0;
  for ( auto src_node: model.logic_list() ) {
    auto logic_type = src_node.type();
    if ( logic_type == BnNodeType::COVER ) {
      auto gate_type = gate_type_list1[src_node.cover_id()];
      extra_node_num += gate_type->extra_node_num();
    }
    else if ( logic_type == BnNodeType::EXPR ) {
      auto gate_type = gate_type_list2[src_node.expr_id()];
      extra_node_num += gate_type->extra_node_num();
    }
  }

  //////////////////////////////////////////////////////////////////////
  // 要素数を数え，必要なメモリ領域を確保する．
  //////////////////////////////////////////////////////////////////////

  SizeType output_num = model.output_num();
  SizeType dff_num = model.seq_num();
  SizeType gate_num = model.logic_num();

  // druid はクロック系，リセット系の回路は無視する．
  // そのため，output/DFF src から DFS で印をつける．
  vector<bool> mark(model.node_num(), false);
  for ( auto node: model.output_list() ) {
    dfs_mark(node, mark);
  }
  for ( auto seq: model.seq_node_list() ) {
    dfs_mark(seq.data_src(), mark);
  }
  vector<BnNode> input_list;
  input_list.reserve(model.input_num());
  for ( auto node: model.input_list() ) {
    if ( mark[node.id()] ) {
      input_list.push_back(node);
    }
  }
  SizeType input_num = input_list.size();

  // ノード数の見積もり
  SizeType nn = set_size(input_num, output_num, dff_num, gate_num, extra_node_num);

  NodeMap node_map;
  TpgConnectionList connection_list(nn);

  //////////////////////////////////////////////////////////////////////
  // 入力ノードを作成する．
  //////////////////////////////////////////////////////////////////////
  for ( auto src_node: input_list ) {
    auto node = make_input_node(src_node.name());
    node_map.reg(src_node.id(), node);
  }

  //////////////////////////////////////////////////////////////////////
  // DFFの出力ノードを作成する．
  //////////////////////////////////////////////////////////////////////
  for ( auto i: Range(dff_num) ) {
    auto src_dff = model.seq_node(i);
    auto src_node = src_dff.data_output();
    auto node = make_dff_output_node(i, src_node.name());
    node_map.reg(src_node.id(), node);
  }

  //////////////////////////////////////////////////////////////////////
  // 論理ノードを作成する．
  // BnNetwork::logic_id_list() はトポロジカルソートされているので
  // 結果として TpgNode もトポロジカル順に並べられる．
  //////////////////////////////////////////////////////////////////////
  for ( auto src_node: model.logic_list() ) {
    const GateType* gate_type = nullptr;
    auto logic_type = src_node.type();
    if ( logic_type == BnNodeType::COVER ) {
      gate_type = gate_type_list1[src_node.cover_id()];
    }
    else if ( logic_type == BnNodeType::EXPR ) {
      gate_type = gate_type_list2[src_node.expr_id()];
    }
    else if ( logic_type == BnNodeType::PRIMITIVE ) {
      auto prim_type = src_node.primitive_type();
      gate_type = mGateTypeMgr.simple_type(prim_type);
    }
    else {
      ASSERT_NOT_REACHED;
    }

    // ファンインのノードを取ってくる．
    vector<const TpgNode*> fanin_array;
    fanin_array.reserve(src_node.fanin_num());
    for ( auto inode: src_node.fanin_list() ) {
      fanin_array.push_back(node_map.get(inode.id()));
    }
    auto node = make_logic_node(src_node.name(), gate_type, fanin_array,
				connection_list);

    // ノードを登録する．
    node_map.reg(src_node.id(), node);
  }

  //////////////////////////////////////////////////////////////////////
  // 出力ノードを作成する．
  //////////////////////////////////////////////////////////////////////
  for ( auto i: Range(output_num) ) {
    auto src_node = model.output(i);
    auto name = model.output_name(i);
    auto inode = node_map.get(src_node.id());
    auto node = make_output_node(name, inode);
    connection_list[inode->id()].push_back(node);
  }

  //////////////////////////////////////////////////////////////////////
  // DFFの入力ノードを作成する．
  //////////////////////////////////////////////////////////////////////
  for ( auto i: Range(dff_num) ) {
    auto src_dff = model.seq_node(i);
    auto src_node = src_dff.data_src();

    auto inode = node_map.get(src_node.id());
    string dff_name = src_dff.name();
    string input_name = dff_name + ".input";
    auto node = make_dff_input_node(i, input_name, inode);
    connection_list[inode->id()].push_back(node);
  }

  ASSERT_COND( node_num() == nn );

  post_op(connection_list);
}

END_NAMESPACE_DRUID
