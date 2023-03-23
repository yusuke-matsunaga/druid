﻿
/// @File TpgNetworkImpl_blif.cc
/// @brief TpgNetworkImpl::set(const BlifModel&) の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "TpgNetwork.h"
#include "TpgNetworkImpl.h"
#include "TpgNode.h"
#include "TpgPPI.h"
#include "TpgPPO.h"
#include "GateType.h"
#include "NodeMap.h"

#include "ym/BlifModel.h"
#include "ym/BlifCover.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス TpgNetwork
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
TpgNetwork::TpgNetwork(
  const BlifModel& model,
  const string& clock_name,
  const string& reset_name
) : mImpl{new TpgNetworkImpl}
{
  mImpl->set(model, clock_name, reset_name);
}

// @brief blif ファイルを読み込む．
TpgNetwork
TpgNetwork::read_blif(
  const string& filename,
  const ClibCellLibrary& cell_library,
  const string& clock_name,
  const string& reset_name
)
{
  BlifModel model;
  if ( !model.read(filename, cell_library) ) {
    throw std::invalid_argument("read failed");
  }
  return TpgNetwork{model, clock_name, reset_name};
}


//////////////////////////////////////////////////////////////////////
// クラス TpgNetworkImpl
//////////////////////////////////////////////////////////////////////

// @brief BlifModel から内容を設定する．
void
TpgNetworkImpl::set(
  const BlifModel& model,
  const string& clock_name,
  const string& reset_name
)
{
  // まずクリアしておく．
  clear();

  //////////////////////////////////////////////////////////////////////
  // 複雑な形のゲートを調べる．
  //////////////////////////////////////////////////////////////////////
  SizeType nc = model.cover_num();
  vector<const GateType*> gate_type_list;
  gate_type_list.reserve(nc);
  for ( SizeType i = 0; i < nc; ++ i ) {
    auto& cover = model.cover(i);
    SizeType ni = cover.input_num();
    auto expr = cover.expr();
    auto gate_type = mGateTypeMgr.new_type(ni, expr);
    gate_type_list.push_back(gate_type);
  }

  SizeType input_num = model.input_list().size();
  SizeType output_num = model.output_list().size();
  SizeType dff_num = model.dff_list().size();
  SizeType gate_num = model.logic_list().size();

  //////////////////////////////////////////////////////////////////////
  // DFF のコントロール端子の数を数える．
  //////////////////////////////////////////////////////////////////////
  SizeType dff_control_num = 0;
  bool has_clear = false;
  for ( auto src_id: model.dff_list() ) {
    // クロック端子
    ++ dff_control_num;
    char rval = model.node_rval(src_id);
    if ( rval == '0' || rval == '1' ) {
      // リセット or プリセット端子
      has_clear = true;
      ++ dff_control_num;
    }
  }
  if ( dff_num > 0 ) {
    // クロック用の外部入力
    ++ input_num;
  }
  if ( has_clear ) {
    // クリア用の外部入力
    ++ input_num;
  }

  //////////////////////////////////////////////////////////////////////
  // 追加で生成されるノード数を数える．
  //////////////////////////////////////////////////////////////////////
  for ( auto src_id: model.logic_list() ) {
    SizeType cover_id = model.node_cover_id(src_id);
    auto gate_type = gate_type_list[cover_id];
    gate_num += gate_type->extra_node_num();
  }

  // ノード数の見積もり
  SizeType nn = set_size(input_num, output_num, dff_num,
			 gate_num, dff_control_num);

  NodeMap node_map;
  vector<vector<const TpgNode*>> connection_list(nn);

  // BlifModel の ID と DFF 番号の対応表
  unordered_map<SizeType, SizeType> dff_dict;

  //////////////////////////////////////////////////////////////////////
  // 外部入力ノードを作成する．
  //////////////////////////////////////////////////////////////////////
  for ( auto id: model.input_list() ) {
    auto name = model.node_name(id);
    auto node = make_input_node(name);
    node_map.reg(id, node);
  }

  TpgNode* clock_input = nullptr;
  if ( dff_num > 0 ) {
    // クロック入力の生成
    auto name = clock_name;
    if ( name == string{} ) {
      name = "__clock__";
    }
    clock_input = make_input_node(name);
  }

  TpgNode* reset_input = nullptr;
  if ( has_clear ) {
    // クリア入力の生成
    auto name = reset_name;
    if ( name == string{} ) {
      name = "__reset__";
    }
    reset_input = make_input_node(name);
  }

  //////////////////////////////////////////////////////////////////////
  // DFFの出力ノード(PPI)を作成する．
  //////////////////////////////////////////////////////////////////////
  SizeType dff_id = 0;
  for ( auto id: model.dff_list() ) {
    auto dff_name = model.node_name(id);
    auto name = dff_name + ".output";
    auto node = make_dff_output_node(dff_id, name);
    node_map.reg(id, node);
    dff_dict.emplace(id, dff_id);
    ++ dff_id;
  }

  //////////////////////////////////////////////////////////////////////
  // 論理ノードを作成する．
  // BlifModel::logic_list() はトポロジカルソートされているので
  // 結果として TpgNode もトポロジカル順に並べられる．
  //////////////////////////////////////////////////////////////////////
  for ( auto src_id: model.logic_list() ) {
    SizeType cover_id = model.node_cover_id(src_id);
    auto gate_type = gate_type_list[cover_id];

    // ファンインのノードを取ってくる．
    auto& src_fanin_list = model.node_fanin_list(src_id);
    SizeType ni = src_fanin_list.size();
    vector<const TpgNode*> fanin_array;
    fanin_array.reserve(ni);
    for ( auto src_iid: src_fanin_list ) {
      auto inode = node_map.get(src_iid);
      fanin_array.push_back(inode);
    }
    auto name = model.node_name(src_id);
    auto node = make_logic_node(name, gate_type, fanin_array, connection_list);

    // ノードを登録する．
    node_map.reg(src_id, node);
  }

  //////////////////////////////////////////////////////////////////////
  // 出力ノードを作成する．
  //////////////////////////////////////////////////////////////////////
  for ( auto src_id: model.output_list() ) {
    auto inode = node_map.get(src_id);
    string buf = "*";
    buf += model.node_name(src_id);
    auto node = make_output_node(buf, inode);
    connection_list[inode->id()].push_back(node);
  }

  //////////////////////////////////////////////////////////////////////
  // DFFの入力ノードを作成する．
  //////////////////////////////////////////////////////////////////////
  for ( auto src_id: model.dff_list() ) {
    ASSERT_COND( dff_dict.count(src_id) > 0 );
    string dff_name = model.node_name(src_id);
    SizeType dff_id = dff_dict.at(src_id);
    auto src_iid = model.node_input(src_id);
    auto inode = node_map.get(src_iid);
    string input_name = dff_name + ".input";
    auto node = make_dff_input_node(dff_id, input_name, inode);
    connection_list[inode->id()].push_back(node);

    // クロック端子を作る．
    string clock_name = dff_name + ".clock";
    auto clock = make_dff_clock_node(dff_id, clock_name, clock_input);
    connection_list[clock_input->id()].push_back(clock);

    char rval = model.node_rval(src_id);
    if ( rval == '0' ) {
      string clear_name = dff_name + ".clear";
      auto clear = make_dff_clear_node(dff_id, clear_name, reset_input);
      connection_list[reset_input->id()].push_back(clear);
    }
    else if ( rval == '1' ) {
      // プリセット端子を作る．
      string preset_name = dff_name + ".preset";
      auto preset = make_dff_preset_node(dff_id, preset_name, reset_input);
      connection_list[reset_input->id()].push_back(preset);
    }
  }

  ASSERT_COND( node_num() == nn );

  post_op(connection_list);
}

END_NAMESPACE_DRUID
