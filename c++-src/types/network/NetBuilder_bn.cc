
/// @file NetBuilder_bn.cc
/// @brief NetBuilder::from_bn() の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "NetBuilder.h"
#include "NetworkRep.h"
#include "NodeRep.h"
#include "GateRep.h"
#include "NetBuilder.h"
#include "NodeMap.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス BnConv
//////////////////////////////////////////////////////////////////////

// @brief BnModel を TpgNetwork に変換する．
TpgNetwork
NetBuilder::from_bn(
  const BnModel& model,
  FaultType fault_type
)
{
  NetBuilder builder(fault_type);

  //////////////////////////////////////////////////////////////////////
  // ノードの論理関数から GateType を作り，登録する．
  //////////////////////////////////////////////////////////////////////
  // 関数のIDをキーにして GateType を格納するリスト
  std::vector<const GateType*> gt_list(model.func_num());
  for ( SizeType id: Range(model.func_num()) ) {
    auto func = model.func(id);
    auto ni = func.input_num();
    if ( func.is_primitive() ) {
      gt_list[id] = builder.make_gate_type(ni, func.primitive_type());
    }
    else if ( func.is_cover() ) {
      auto expr = func.input_cover().expr();
      if ( func.output_inv() ) {
	expr = ~expr;
      }
      gt_list[id] = builder.make_gate_type(ni, expr);
    }
    else if ( func.is_expr() ) {
      auto expr = func.expr();
      gt_list[id] = builder.make_gate_type(ni, expr);
    }
    else if ( func.is_tvfunc() ) {
      throw std::invalid_argument{"TvFunc type is not supported"};
    }
    else if ( func.is_bdd() ) {
      throw std::invalid_argument{"Bdd type is not supported"};
    }
  }

  //////////////////////////////////////////////////////////////////////
  // 追加で生成されるノード数を数える．
  //////////////////////////////////////////////////////////////////////
  SizeType extra_node_num = 0;
  for ( auto src_node: model.logic_list() ) {
    auto func = src_node.func();
    auto gate_type = gt_list[func.id()];
    extra_node_num += gate_type->extra_node_num();
  }

  //////////////////////////////////////////////////////////////////////
  // 要素数を数え，必要なメモリ領域を確保する．
  //////////////////////////////////////////////////////////////////////
  auto input_num = model.input_num();
  auto output_num = model.output_num();
  auto dff_num = model.dff_num();
  auto gate_num = model.logic_num();
  builder.init(input_num, output_num, dff_num, gate_num, extra_node_num);

  NodeMap node_map;

  //////////////////////////////////////////////////////////////////////
  // 入力ノードを作成する．
  //////////////////////////////////////////////////////////////////////
  for ( auto i: Range(input_num) ) {
    auto src_node = model.input(i);
    auto name = model.input_name(i);
    auto node = builder.make_input_node(name);
    node_map.reg(src_node.id(), node);
  }

  //////////////////////////////////////////////////////////////////////
  // DFFの出力ノードを作成する．
  //////////////////////////////////////////////////////////////////////
  for ( auto i: Range(dff_num) ) {
    auto src_dff = model.dff(i);
    auto src_node = src_dff.output();
    auto dff_name = model.dff_name(i);
    auto output_name = dff_name + ".output";
    auto node = builder.make_dff_output_node(output_name);
    node_map.reg(src_node.id(), node);
  }

  //////////////////////////////////////////////////////////////////////
  // 論理ノードを作成する．
  // BnModel::logic_id_list() はトポロジカルソートされているので
  // 結果として TpgNode もトポロジカル順に並べられる．
  //////////////////////////////////////////////////////////////////////
  for ( auto src_node: model.logic_list() ) {
    auto func = src_node.func();
    auto gate_type = gt_list[func.id()];
    // ファンインのノードを取ってくる．
    std::vector<const NodeRep*> fanin_list;
    fanin_list.reserve(src_node.fanin_num());
    for ( auto inode: src_node.fanin_list() ) {
      fanin_list.push_back(node_map.get(inode.id()));
    }
    auto gate = builder.make_gate(gate_type, fanin_list);
    auto node = gate->output_node();

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
    auto node = builder.make_output_node(name, inode);
  }

  //////////////////////////////////////////////////////////////////////
  // DFFの入力ノードを作成する．
  //////////////////////////////////////////////////////////////////////
  for ( auto i: Range(dff_num) ) {
    auto src_dff = model.dff(i);
    auto dff_name = model.dff_name(i);
    auto input_name = dff_name + ".input";
    auto src_node = src_dff.input();
    auto inode = node_map.get(src_node.id());
    auto node = builder.make_dff_input_node(i, input_name, inode);
  }

  return builder.wrap_up();
}

END_NAMESPACE_DRUID
