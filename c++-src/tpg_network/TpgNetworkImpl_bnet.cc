
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

#include "ym/BnNetwork.h"
#include "ym/BnPort.h"
#include "ym/BnDff.h"
#include "ym/BnNode.h"

#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス TpgNetwork
//////////////////////////////////////////////////////////////////////

// @brief BnNetwork からの変換コンストラクタ
TpgNetwork::TpgNetwork(
  const BnNetwork& network
) : mImpl{new TpgNetworkImpl}
{
  mImpl->set(network);
}


//////////////////////////////////////////////////////////////////////
// クラス TpgNetworkImpl
//////////////////////////////////////////////////////////////////////

// @brief 内容を設定する．
void
TpgNetworkImpl::set(
  const BnNetwork& network
)
{
  // まずクリアしておく．
  clear();

  //////////////////////////////////////////////////////////////////////
  // NodeInfoMgr にノードの論理関数を登録する．
  //////////////////////////////////////////////////////////////////////
  vector<const GateType*> gate_type_list;
  gate_type_list.reserve(network.expr_num());
  for ( SizeType i: Range(network.expr_num()) ) {
    auto expr = network.expr(i);
    SizeType ni = expr.input_size();
    auto gate_type = mGateTypeMgr.new_type(ni, expr);
    gate_type_list.push_back(gate_type);
  }

  //////////////////////////////////////////////////////////////////////
  // 追加で生成されるノード数を数える．
  //////////////////////////////////////////////////////////////////////
  SizeType extra_node_num = 0;
  for ( auto src_node: network.logic_list() ) {
    auto logic_type = src_node.type();
    if ( logic_type == BnNodeType::Expr ) {
      auto gate_type = gate_type_list[src_node.expr_id()];
      extra_node_num += gate_type->extra_node_num();
    }
  }

  //////////////////////////////////////////////////////////////////////
  // 要素数を数え，必要なメモリ領域を確保する．
  //////////////////////////////////////////////////////////////////////

  // BnPort は複数ビットの場合があり，さらに入出力が一緒なのでめんどくさい
  vector<SizeType> input_map;
  vector<SizeType> output_map;
  for ( auto port: network.port_list() ) {
    for ( auto b: Range(port.bit_width() ) ) {
      auto node = port.bit(b);
      SizeType id = node.id();
      if ( node.is_input() ) {
	input_map.push_back(id);
      }
      else if ( node.is_output() ) {
	output_map.push_back(id);
      }
      else {
	ASSERT_NOT_REACHED;
      }
    }
  }
  SizeType input_num = input_map.size();
  SizeType output_num = output_map.size();
  SizeType dff_num = network.dff_num();
  SizeType gate_num = network.logic_num();
  SizeType dff_clock_num = 0;
  for ( auto dff: network.dff_list() ) {
    if ( dff.clear().is_valid() ) {
      // クリア端子
      ++ output_num;
    }

    if ( dff.preset().is_valid() ) {
      // プリセット端子
      ++ output_num;
    }
  }

  // ノード数の見積もり
  SizeType nn = set_size(input_num, output_num, dff_num, gate_num, extra_node_num);

  NodeMap node_map;
  TpgConnectionList connection_list(nn);

  //////////////////////////////////////////////////////////////////////
  // 入力ノードを作成する．
  //////////////////////////////////////////////////////////////////////
  for ( auto i: Range(mInputNum) ) {
    SizeType id = input_map[i];
    auto src_node = network.node(id);
    ASSERT_COND( src_node.is_input() );
    auto node = make_input_node(src_node.name());
    node_map.reg(id, node);
  }

  //////////////////////////////////////////////////////////////////////
  // DFFの出力ノードを作成する．
  //////////////////////////////////////////////////////////////////////
  SizeType i = 0;
  for ( auto src_dff: network.dff_list() ) {
    auto src_node = src_dff.data_out();
    ASSERT_COND( src_node.is_input() );
    auto node = make_dff_output_node(i, src_node.name());
    node_map.reg(src_node.id(), node);
    ++ i;
  }

  //////////////////////////////////////////////////////////////////////
  // 論理ノードを作成する．
  // BnNetwork::logic_id_list() はトポロジカルソートされているので
  // 結果として TpgNode もトポロジカル順に並べられる．
  //////////////////////////////////////////////////////////////////////
  for ( auto src_node: network.logic_list() ) {
    const GateType* gate_type = nullptr;
    auto logic_type = src_node.type();
    if ( logic_type == BnNodeType::Expr ) {
      gate_type = gate_type_list[src_node.expr_id()];
    }
    else if ( logic_type == BnNodeType::Prim ) {
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
  for ( auto id: output_map ) {
    auto src_node = network.node(id);
    ASSERT_COND( src_node.is_output() );
    auto inode = node_map.get(src_node.output_src().id());
    string buf = "*";
    buf += src_node.name();
    auto node = make_output_node(buf, inode);
    connection_list[inode->id()].push_back(node);
  }

  //////////////////////////////////////////////////////////////////////
  // DFFの入力ノードを作成する．
  //////////////////////////////////////////////////////////////////////
  for ( auto i: Range(dff_num) ) {
    auto src_dff = network.dff(i);
    auto src_node = src_dff.data_in();

    auto inode = node_map.get(src_node.output_src().id());
    string dff_name = src_dff.name();
    string input_name = dff_name + ".input";
    auto node = make_dff_input_node(i, input_name, inode);
    connection_list[inode->id()].push_back(node);

    // クリア端子を作る．
    if ( src_dff.clear().is_valid() ) {
      auto src_clear = src_dff.clear();
      auto clear_fanin = node_map.get(src_clear.output_src().id());
      string clear_name = dff_name + ".clear";
      auto clear = make_dff_clear_node(i, clear_name, clear_fanin);
      connection_list[clear_fanin->id()].push_back(clear);
    }

    // プリセット端子を作る．
    if ( src_dff.preset().is_valid() ) {
      auto src_preset = src_dff.preset();
      auto preset_fanin = node_map.get(src_preset.output_src().id());
      string preset_name = dff_name + ".preset";
      auto preset = make_dff_preset_node(i, preset_name, preset_fanin);
      connection_list[preset_fanin->id()].push_back(preset);
    }
  }

  ASSERT_COND( node_num() == nn );

  post_op(connection_list);
}

END_NAMESPACE_DRUID
