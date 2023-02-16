
/// @File TpgNetworkImpl_iscas.cc
/// @brief TpgNetworkImpl::set(const Iscas89Model&) の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2019, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "TpgNetwork.h"
#include "TpgNetworkImpl.h"
#include "TpgNode.h"
#include "TpgGateInfo.h"
#include "NodeMap.h"
#include "ym/Iscas89ExParser.h"
#include "ym/Iscas89Model.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス TpgNetwork
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
TpgNetwork::TpgNetwork(
  const Iscas89Model& model,
  const string& clock_name
) : mImpl{new TpgNetworkImpl}
{
  mImpl->set(model, clock_name);
}

// @brief iscas89 形式のファイルを読み込む．
TpgNetwork
TpgNetwork::read_iscas89(
  const string& filename,
  const string& clock
)
{
  Iscas89ExParser parser;
  Iscas89Model model;
  if ( !parser.read(filename, model) ) {
    throw std::invalid_argument("read failed");
  }
  return TpgNetwork{model, clock};
}


//////////////////////////////////////////////////////////////////////
// クラス TpgNetworkImpl
//////////////////////////////////////////////////////////////////////

// @brief 内容を設定する．
void
TpgNetworkImpl::set(
  const Iscas89Model& model,
  const string& clock_name
)
{
  // まずクリアしておく．
  clear();

  //////////////////////////////////////////////////////////////////////
  // NodeInfoMgr にノードの論理関数を登録する．
  //////////////////////////////////////////////////////////////////////
  TpgGateInfoMgr node_info_mgr;
  vector<const TpgGateInfo*> node_info_list;
  node_info_list.reserve(model.expr_list().size());
  for ( auto expr: model.expr_list() ) {
    SizeType ni = expr.input_size();
    auto node_info = node_info_mgr.new_info(ni, expr);
    node_info_list.push_back(node_info);
  }

  //////////////////////////////////////////////////////////////////////
  // 追加で生成されるノード数を数える．
  //////////////////////////////////////////////////////////////////////
  SizeType extra_node_num = 0;
  for ( auto src_id: model.gate_list() ) {
    auto type = model.node_type(src_id);
    if ( type == Iscas89Type::Gate ) {
      auto gate_type = model.node_gate_type(src_id);
      if ( gate_type == PrimType::Xor || gate_type == PrimType::Xnor ) {
	SizeType ni = model.node_fanin_num(src_id);
	extra_node_num += (ni - 2);
      }
    }
    else if ( type == Iscas89Type::Complex ) {
      auto expr_id = model.node_expr_id(src_id);
      auto node_info = node_info_list[expr_id];
      extra_node_num += node_info->extra_node_num();
    }
    else {
      ASSERT_NOT_REACHED;
    }
  }

  //////////////////////////////////////////////////////////////////////
  // 要素数を数え，必要なメモリ領域を確保する．
  //////////////////////////////////////////////////////////////////////

  SizeType input_num = model.input_list().size();
  SizeType output_num = model.output_list().size();
  SizeType dff_num = model.dff_list().size();
  SizeType gate_num = model.gate_list().size() + extra_node_num;

  // .bench はクロックが明示的に指定されていない．
  if ( dff_num > 0 ) {
    ++ input_num;
  }
  // .bench はクロックしか持たない．
  SizeType dff_control_num = dff_num;

  // 生成されるノード数を見積もる．
  SizeType nn = set_size(input_num, output_num, dff_num,
			 gate_num, dff_control_num);

  NodeMap node_map;
  vector<vector<const TpgNode*>> connection_list(nn);

  //////////////////////////////////////////////////////////////////////
  // 外部入力ノードを作成する．
  //////////////////////////////////////////////////////////////////////
  for ( auto id: model.input_list() ) {
    auto name = model.node_name(id);
    auto node = make_input_node(name);
    node_map.reg(id, node);
  }

  // .bench には外部クロック端子の記述がないので生成する
  TpgNode* clock_node = nullptr;
  if ( dff_num > 0 ) {
    auto name = clock_name;
    if ( name == string{} ) {
      name = "__clock__";
    }
    clock_node = make_input_node(name);
  }

  //////////////////////////////////////////////////////////////////////
  // DFFの出力ノード(PPI)を作成する．
  //////////////////////////////////////////////////////////////////////
  unordered_map<SizeType, SizeType> dff_map;
  SizeType dff_id = 0;
  for ( auto id: model.dff_list() ) {
    auto name = model.node_name(id);
    auto node = make_dff_output_node(dff_id, name);
    node_map.reg(id, node);
    dff_map.emplace(id, dff_id);
    ++ dff_id;
  }

  //////////////////////////////////////////////////////////////////////
  // 論理ノードを作成する．
  // Iscas89Model::gate_list() はトポロジカルソートされているので
  // 結果として TpgNode もトポロジカル順に並べられる．
  //////////////////////////////////////////////////////////////////////
  for ( auto id: model.gate_list() ) {
    auto type = model.node_type(id);
    const TpgGateInfo* node_info = nullptr;
    if ( type == Iscas89Type::Gate ) {
      auto prim_type = model.node_gate_type(id);
      node_info = node_info_mgr.simple_type(prim_type);
    }
    else if ( type == Iscas89Type::Complex ) {
      auto expr_id = model.node_expr_id(id);
      node_info = node_info_list[expr_id];
    }

    // ファンインのノードを取ってくる．
    vector<const TpgNode*> fanin_array;
    fanin_array.reserve(model.node_fanin_num(id));
    for ( auto iid: model.node_fanin_list(id) ) {
      fanin_array.push_back(node_map.get(iid));
    }
    auto name = model.node_name(id);
    auto node = make_logic_node(name, node_info, fanin_array, connection_list);
    node_map.reg(id, node);
  }

  //////////////////////////////////////////////////////////////////////
  // 出力ノードを作成する．
  //////////////////////////////////////////////////////////////////////
  for ( auto id: model.output_list() ) {
    auto inode = node_map.get(id);
    string buf = "*";
    buf += model.node_name(id);
    auto node = make_output_node(buf, inode);
    connection_list[inode->id()].push_back(node);
  }

  //////////////////////////////////////////////////////////////////////
  // DFFの入力ノードを作成する．
  //////////////////////////////////////////////////////////////////////
  for ( auto id: model.dff_list() ) {
    auto iid = model.node_input(id);
    auto inode = node_map.get(iid);
    ASSERT_COND( dff_map.count(id) > 0 );
    auto dff_id = dff_map.at(id);
    string dff_name = model.node_name(id);
    string input_name = dff_name + ".input";
    auto node = make_dff_input_node(dff_id, input_name, inode);
    connection_list[inode->id()].push_back(node);

    // クロック端子を作る．
    string clock_name = dff_name + ".clock";
    auto clock = make_dff_clock_node(dff_id, clock_name, clock_node);
    connection_list[clock_node->id()].push_back(clock);
  }

  ASSERT_COND( node_num() == nn );

  post_op(connection_list);
}

END_NAMESPACE_DRUID
