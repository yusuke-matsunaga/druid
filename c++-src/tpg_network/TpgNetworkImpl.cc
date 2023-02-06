
/// @File TpgNetworkImpl.cc
/// @brief TpgNetworkImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2019, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "TpgNetworkImpl.h"
#include "TpgNode.h"
#include "TpgGateInfo.h"
#include "TpgFaultBase.h"
#include "TpgDff.h"
#include "TpgMFFC.h"
#include "TpgFFR.h"

#include "GateType.h"

#include "NodeMap.h"
#include "AuxNodeInfo.h"

#include "ym/BnNetwork.h"
#include "ym/BnPort.h"
#include "ym/BnDff.h"
#include "ym/BnNode.h"
#include "ym/Expr.h"

#include "ym/Iscas89Model.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// BnFuncType を GateType に変換する．
inline
GateType
conv_to_gate_type(
  BnNodeType type
)
{
  switch ( type ) {
  case BnNodeType::C0:   return GateType::Const0;
  case BnNodeType::C1:   return GateType::Const1;
  case BnNodeType::Buff: return GateType::Buff;
  case BnNodeType::Not:  return GateType::Not;
  case BnNodeType::And:  return GateType::And;
  case BnNodeType::Nand: return GateType::Nand;
  case BnNodeType::Or:   return GateType::Or;
  case BnNodeType::Nor:  return GateType::Nor;
  case BnNodeType::Xor:  return GateType::Xor;
  case BnNodeType::Xnor: return GateType::Xnor;
  default: break;
  }
  ASSERT_NOT_REACHED;
  return GateType::Const0;
}

// Iscas89Gate を GateType に変換する．
inline
GateType
conv_to_gate_type(
  Iscas89Gate type
)
{
  switch ( type ) {
  case Iscas89Gate::C0:   return GateType::Const0;
  case Iscas89Gate::C1:   return GateType::Const1;
  case Iscas89Gate::Buff: return GateType::Buff;
  case Iscas89Gate::Not:  return GateType::Not;
  case Iscas89Gate::And:  return GateType::And;
  case Iscas89Gate::Nand: return GateType::Nand;
  case Iscas89Gate::Or:   return GateType::Or;
  case Iscas89Gate::Nor:  return GateType::Nor;
  case Iscas89Gate::Xor:  return GateType::Xor;
  case Iscas89Gate::Xnor: return GateType::Xnor;
  default: break;
  }
  ASSERT_NOT_REACHED;
  return GateType::Const0;
}

// immediate dominator リストをマージする．
const TpgNode*
merge(
  const TpgNode* node1,
  const TpgNode* node2
)
{
  for ( ; ; ) {
    if ( node1 == node2 ) {
      return node1;
    }
    if ( node1 == nullptr || node2 == nullptr ) {
      return nullptr;
    }
    SizeType id1 = node1->id();
    SizeType id2 = node2->id();
    if ( id1 < id2 ) {
      node1 = node1->imm_dom();
    }
    else if ( id1 > id2 ) {
      node2 = node2->imm_dom();
    }
  }
}

// pair の最初の要素で比較を行なうファンクター
// こういうのは lambda 表記があると簡潔にすむ．
struct Lt
{
  bool
  operator()(
    const pair<SizeType, SizeType>& left,
    const pair<SizeType, SizeType>& right
  )
  {
    return left.first < right.first;
  }

};


void
check_network_connection(
  const TpgNetworkImpl& network
)
{
  // fanin/fanout の sanity check
  bool error = false;

  for ( auto node: network.node_list() ) {
    for ( auto inode: node->fanin_list() ) {
      bool found = false;
      for ( auto onode: inode->fanout_list() ) {
	if ( onode == node ) {
	  found = true;
	  break;
	}
      }
      if ( !found ) {
	error = true;
	cout << "Error: inode(" << inode->id() << ") is a fanin of "
	     << "node(" << node->id() << "), but "
	     << "node(" << node->id() << ") is not a fanout of "
	     << "inode(" << inode->id() << ")" << endl;
      }
    }
    for ( auto onode: node->fanout_list() ) {
      bool found = false;
      for ( auto inode: onode->fanin_list() ) {
	if ( inode == node ) {
	  found = true;
	  break;
	}
      }
      if ( !found ) {
	error = true;
	cout << "Error: onode(" << onode->id() << ") is a fanout of "
	     << "node(" << node->id() << "), but "
	     << "node(" << node->id() << ") is not a fanin of "
	     << "onode(" << onode->id() << ")" << endl;
      }
    }
  }
  if ( error ) {
    cout << "network connectivity check failed" << endl;
    abort();
  }
}

END_NONAMESPACE


//////////////////////////////////////////////////////////////////////
// クラス TpgNetworkImpl
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
TpgNetworkImpl::TpgNetworkImpl()
{
}

/// @brief デストラクタ
TpgNetworkImpl::~TpgNetworkImpl()
{
  clear();
}

// @brief 内容をクリアする．
void
TpgNetworkImpl::clear()
{
  for ( auto node: mNodeArray ) {
    delete node;
  }
  for ( auto fault: mRepFaultArray ) {
    delete fault;
  }
}

// @brief ノード名を得る．
const string&
TpgNetworkImpl::node_name(
  SizeType id
) const
{
  ASSERT_COND( id >= 0 && id < node_num() );

  return mAuxInfoArray[id].name();
}

// @brief ノードに関係した代表故障数を返す．
SizeType
TpgNetworkImpl::node_rep_fault_num(
  SizeType id
) const
{
  ASSERT_COND( id >= 0 && id < node_num() );

  return mAuxInfoArray[id].fault_num();
}

// @brief ノードに関係した代表故障を返す．
const TpgFault*
TpgNetworkImpl::node_rep_fault(
  SizeType id,
  SizeType pos
) const
{
  ASSERT_COND( id >= 0 && id < node_num() );

  return mAuxInfoArray[id].fault(pos);
}

// @brief 出力の故障を得る．
TpgFaultBase*
TpgNetworkImpl::_node_output_fault(
  SizeType id,
  Fval2 val
)
{
  ASSERT_COND( id >= 0 && id < node_num() );

  return mAuxInfoArray[id].output_fault(val);
}

// @brief 入力の故障を得る．
TpgFaultBase*
TpgNetworkImpl::_node_input_fault(
  SizeType id,
  Fval2 val,
  SizeType pos
)
{
  ASSERT_COND( id >= 0 && id < node_num() );

  return mAuxInfoArray[id].input_fault(pos, val);
}

// @brief DFF を得る．
const TpgDff&
TpgNetworkImpl::dff(
  SizeType pos
) const
{
  ASSERT_COND( pos >= 0 && pos < dff_num() );

  return mDffArray[pos];
}

// @brief DFF のリストを得る．
const vector<TpgDff>&
TpgNetworkImpl::dff_list() const
{
  return mDffArray;
}

// @brief MFFC を返す．
TpgMFFC
TpgNetworkImpl::mffc(
  SizeType pos
) const
{
  ASSERT_COND( pos >= 0 && pos < mffc_num() );

  return TpgMFFC{this, pos};
}

// @brief MFFC の根のノードを返す．
const TpgNode*
TpgNetworkImpl::mffc_root(
  SizeType pos
) const
{
  ASSERT_COND( pos >= 0 && pos < mffc_num() );

  auto& mffc = mMffcArray[pos];
  return mffc.mRoot;
}

// @brief MFFCに含まれるFFRのリストを返す．
const vector<TpgFFR>&
TpgNetworkImpl::mffc_ffr_list(
  SizeType pos
) const
{
  ASSERT_COND( pos >= 0 && pos < mffc_num() );

  auto& mffc = mMffcArray[pos];
  return mffc.mFfrList;
}

// @brief MFFCに含まれる代表故障のリストを返す．
const vector<const TpgFault*>&
TpgNetworkImpl::mffc_fault_list(
  SizeType pos
) const
{
  ASSERT_COND( pos >= 0 && pos < mffc_num() );

  auto& mffc = mMffcArray[pos];
  return mffc.mFaultList;
}

#if 0
// @brief MFFC のリストを得る．
const vector<TpgMFFC>&
TpgNetworkImpl::mffc_list() const
{
  return mMffcArray;
}
#endif

// @brief FFR を返す．
TpgFFR
TpgNetworkImpl::ffr(
  SizeType pos
) const
{
  ASSERT_COND( pos >= 0 && pos < ffr_num() );

  return TpgFFR{this, pos};
}

// @brief FFR の根のノードを返す．
const TpgNode*
TpgNetworkImpl::ffr_root(
  SizeType pos
) const
{
  ASSERT_COND( pos >= 0 && pos < ffr_num() );

  auto& ffr = mFfrArray[pos];
  return ffr.mRoot;
}

// @brief 葉(FFRの入力)のリストを返す．
const vector<const TpgNode*>&
TpgNetworkImpl::ffr_input_list(
  SizeType pos
) const
{
  ASSERT_COND( pos >= 0 && pos < ffr_num() );

  auto& ffr = mFfrArray[pos];
  return ffr.mInputList;
}

// @brief FFRに含まれる代表故障のリストを返す．
const vector<const TpgFault*>&
TpgNetworkImpl::ffr_fault_list(
  SizeType pos
) const
{
  ASSERT_COND( pos >= 0 && pos < ffr_num() );

  auto& ffr = mFfrArray[pos];
  return ffr.mFaultList;
}

BEGIN_NONAMESPACE

// @brief ノードの TFI にマークをつける．
SizeType
tfimark(
  const TpgNode* node,
  vector<bool>& mark
)
{
  if ( mark[node->id()] ) {
    return 0;
  }
  mark[node->id()] = true;

  SizeType n = 1;
  for ( auto inode: node->fanin_list() ) {
    n += tfimark(inode, mark);
  }
  return n;
}

END_NONAMESPACE


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
  TpgGateInfoMgr node_info_mgr;
  vector<const TpgGateInfo*> node_info_list;
  node_info_list.reserve(network.expr_num());
  for ( SizeType i: Range(network.expr_num()) ) {
    auto expr = network.expr(i);
    SizeType ni = expr.input_size();
    auto node_info = node_info_mgr.complex_type(ni, expr);
    node_info_list.push_back(node_info);
  }

  //////////////////////////////////////////////////////////////////////
  // 追加で生成されるノード数を数える．
  //////////////////////////////////////////////////////////////////////
  SizeType extra_node_num = 0;
  SizeType nl = network.logic_num();
  for ( auto src_node: network.logic_list() ) {
    auto logic_type = src_node.type();
    if ( logic_type == BnNodeType::Expr ) {
      auto node_info = node_info_list[src_node.func_id()];
      extra_node_num += node_info->extra_node_num();
    }
    else if ( logic_type == BnNodeType::Xor || logic_type == BnNodeType::Xnor ) {
      SizeType ni = src_node.fanin_num();
      extra_node_num += (ni - 2);
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

  SizeType dff_control_num = 0;
  for ( auto dff: network.dff_list() ) {
    // まずクロックで一つ
    ++ dff_control_num;
    if ( dff.clear().is_valid() ) {
      // クリア端子で一つ
      ++ dff_control_num;
    }
    if ( dff.preset().is_valid() ) {
      // プリセット端子で一つ
      ++ dff_control_num;
    }
  }

  SizeType nn = input_num + output_num + dff_num * 2 + nl + extra_node_num + dff_control_num;
  set_size(input_num, output_num, dff_num, nn);

  NodeMap node_map;
  vector<vector<const TpgNode*>> connection_list(nn);

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
    const TpgGateInfo* node_info = nullptr;
    auto logic_type = src_node.type();
    if ( logic_type == BnNodeType::Expr ) {
      node_info = node_info_list[src_node.func_id()];
    }
    else {
      ASSERT_COND( logic_type != BnNodeType::TvFunc );
      auto gate_type = conv_to_gate_type(logic_type);
      node_info = node_info_mgr.simple_type(gate_type);
    }

    // ファンインのノードを取ってくる．
    vector<const TpgNode*> fanin_array;
    fanin_array.reserve(src_node.fanin_num());
    for ( auto inode: src_node.fanin_list() ) {
      fanin_array.push_back(node_map.get(inode.id()));
    }
    auto node = make_logic_node(src_node.name(), node_info, fanin_array,
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

    // クロック端子を作る．
    auto src_clock = src_dff.clock();
    auto clock_fanin = node_map.get(src_clock.output_src().id());
    string clock_name = dff_name + ".clock";
    auto clock = make_dff_clock_node(i, clock_name, clock_fanin);
    connection_list[clock_fanin->id()].push_back(clock);

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

// @brief BlifModel から内容を設定する．
void
TpgNetworkImpl::set(
  const BlifModel& model
)
{
  // まずクリアしておく．
  clear();

  TpgGateInfoMgr node_info_mgr;

#if 0
  //////////////////////////////////////////////////////////////////////
  // 複雑な形のゲートを調べる．
  //////////////////////////////////////////////////////////////////////
  SizeType nc = model.cover_num();
  vector<const TpgGateInfo*> node_info_array(nc);
  for ( SizeType i = 0; i < nc; ++ i ) {
    auto& cover = mode.cover(i);
  }
#endif
}

// @brief 内容を設定する．
void
TpgNetworkImpl::set(
  const Iscas89Model& model
)
{
  // まずクリアしておく．
  clear();

  TpgGateInfoMgr node_info_mgr;

  //////////////////////////////////////////////////////////////////////
  // 複雑な形のゲートを調べる．
  // 具体的には Mux 型
  //////////////////////////////////////////////////////////////////////
  const TpgGateInfo* mux2_info = nullptr;
  const TpgGateInfo* mux4_info = nullptr;
  for ( auto src_id: model.gate_list() ) {
    auto gate_type = model.node_gate_type(src_id);
    if ( gate_type == Iscas89Gate::Mux ) {
      SizeType ni = model.node_fanin_num(src_id);
      if ( ni == 3 ) {
	auto v0 = Expr::make_literal(0);
	auto v1 = Expr::make_literal(1);
	auto v2 = Expr::make_literal(2);
	auto expr = ~v0 & v1 | v0 & v2;
	mux2_info = node_info_mgr.complex_type(3, expr);
      }
      else if ( ni == 6 ) {
	auto v0 = Expr::make_literal(0);
	auto v1 = Expr::make_literal(1);
	auto v2 = Expr::make_literal(2);
	auto v3 = Expr::make_literal(3);
	auto v4 = Expr::make_literal(4);
	auto v5 = Expr::make_literal(5);
	auto expr = ~v0 & ~v1 & v2 | ~v0 & v1 & v3 | v0 & ~v1 & v4 | v0 & v1 & v5;
	mux4_info = node_info_mgr.complex_type(6, expr);
      }
      else {
	throw std::invalid_argument("Mux type with more than 5 inputs");
      }
    }
  }

  //////////////////////////////////////////////////////////////////////
  // 追加で生成されるノード数を数える．
  //////////////////////////////////////////////////////////////////////
  SizeType extra_node_num = 0;
  for ( auto src_id: model.gate_list() ) {
    auto gate_type = model.node_gate_type(src_id);
    if ( gate_type == Iscas89Gate::Xor || gate_type == Iscas89Gate::Xnor ) {
      SizeType ni = model.node_fanin_num(src_id);
      extra_node_num += (ni - 2);
    }
    else if ( gate_type == Iscas89Gate::Mux ) {
      SizeType ni = model.node_fanin_num(src_id);
      if ( ni == 3 ) {
	extra_node_num += mux2_info->extra_node_num();
      }
      else if ( ni == 6 ) {
	extra_node_num += mux4_info->extra_node_num();
      }
    }
  }

  //////////////////////////////////////////////////////////////////////
  // 要素数を数え，必要なメモリ領域を確保する．
  //////////////////////////////////////////////////////////////////////

  SizeType input_num = model.input_list().size();
  SizeType output_num = model.output_list().size();
  SizeType dff_num = model.dff_list().size();
  SizeType gate_num = model.gate_list().size();

  SizeType dff_control_num = 0;
  if ( dff_num > 0 ) {
    // .bench はクロックしか持たない．
    dff_control_num += 1;
  }

  SizeType nn = input_num + output_num + dff_num * 2 + gate_num + extra_node_num + dff_control_num;
  set_size(input_num, output_num, dff_num, nn);

  NodeMap node_map;
  vector<vector<const TpgNode*>> connection_list;

  //////////////////////////////////////////////////////////////////////
  // 入力ノードを作成する．
  //////////////////////////////////////////////////////////////////////
  for ( auto id: model.input_list() ) {
    auto name = model.node_name(id);
    auto node = make_input_node(name);
    node_map.reg(id, node);
  }

  //////////////////////////////////////////////////////////////////////
  // DFFの出力ノードを作成する．
  //////////////////////////////////////////////////////////////////////
  SizeType dff_id = 0;
  for ( auto id: model.dff_list() ) {
    auto name = model.node_name(id);
    auto node = make_dff_output_node(dff_id, name);
    node_map.reg(id, node);
    ++ dff_id;
  }

  //////////////////////////////////////////////////////////////////////
  // .bench には外部クロック端子の記述がないので生成する
  //////////////////////////////////////////////////////////////////////
  TpgNode* clock_node = nullptr;
  if ( dff_num > 0 ) {
    clock_node = make_input_node("__clock__");
  }

  //////////////////////////////////////////////////////////////////////
  // 論理ノードを作成する．
  // Iscas89Model::gate_list() はトポロジカルソートされているので
  // 結果として TpgNode もトポロジカル順に並べられる．
  //////////////////////////////////////////////////////////////////////
  for ( auto id: model.gate_list() ) {
    auto src_gate_type = model.node_gate_type(id);
    const TpgGateInfo* node_info = nullptr;
    if ( src_gate_type == Iscas89Gate::Mux ) {
      SizeType ni = model.node_fanin_num(id);
      if ( ni == 3 ) {
	node_info = mux2_info;
      }
      else if ( ni == 6 ) {
	node_info = mux4_info;
      }
      else {
	ASSERT_NOT_REACHED;
      }
    }
    else {
      auto gate_type = conv_to_gate_type(src_gate_type);
      node_info = node_info_mgr.simple_type(gate_type);
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
    connection_list[id].push_back(node);
  }

  //////////////////////////////////////////////////////////////////////
  // DFFの入力ノードを作成する．
  //////////////////////////////////////////////////////////////////////
  dff_id = 0;
  for ( auto id: model.dff_list() ) {
    auto iid = model.node_input(id);
    auto inode = node_map.get(iid);
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

// @brief サイズを設定する．
void
TpgNetworkImpl::set_size(
  SizeType input_num,
  SizeType output_num,
  SizeType dff_num,
  SizeType node_num
)
{
  mInputNum = input_num;
  mOutputNum = output_num;
  mDffArray.clear();
  mDffArray.resize(dff_num);
  for ( auto i: Range(dff_num) ) {
    mDffArray[i].mId = i;
  }

  mNodeArray.clear();
  mNodeArray.reserve(node_num);
  mAuxInfoArray.clear();
  mAuxInfoArray.reserve(node_num);

  SizeType nppi = mInputNum + dff_num;
  mPPIArray.clear();
  mPPIArray.reserve(nppi);

  SizeType nppo = mOutputNum + dff_num;
  mPPOArray.clear();
  mPPOArray.reserve(nppo);
  mPPOArray2.clear();
  mPPOArray2.reserve(nppo);
}

// @brief set() の後処理
void
TpgNetworkImpl::post_op(
  const vector<vector<const TpgNode*>>& connection_list
)
{
  //////////////////////////////////////////////////////////////////////
  // ファンアウトをセットする．
  //////////////////////////////////////////////////////////////////////
  for ( SizeType i = 0; i < node_num(); ++ i ) {
    auto from = const_cast<TpgNode*>(mNodeArray[i]);
    const auto& fo_list = connection_list[i];
    from->set_fanouts(fo_list);
  }

  { // 検証
    // 接続が正しいかチェックする．
    check_network_connection(*this);
  }

  //////////////////////////////////////////////////////////////////////
  // データ系のノードに印をつける．
  //////////////////////////////////////////////////////////////////////
  vector<bool> dmarks(node_num(), false);
  for ( auto node: ppo_list() ) {
    tfimark(node, dmarks);
  }

  //////////////////////////////////////////////////////////////////////
  // 代表故障を求める．
  //////////////////////////////////////////////////////////////////////
  SizeType rep_num = 0;
  for ( auto i: Range(node_num()) ) {
    // ノードごとに代表故障を設定する．
    // この処理は出力側から行う必要がある．
    auto node = mNodeArray[node_num() - i - 1];
    if ( dmarks[node->id()] ) {
      SizeType nf = set_rep_faults(node);
      rep_num += nf;
    }
  }

  mRepFaultArray.clear();
  mRepFaultArray.reserve(rep_num);
  for ( auto i: Range(node_num()) ) {
    auto& aux_node_info = mAuxInfoArray[i];
    SizeType nf = aux_node_info.fault_num();
    for ( auto j: Range(nf) ) {
      auto fault = aux_node_info.fault(j);
      mRepFaultArray.push_back(fault);
    }
  }

  //////////////////////////////////////////////////////////////////////
  // TFI のサイズの昇順に並べた出力順を
  // mPPOArray2 に記録する．
  //////////////////////////////////////////////////////////////////////
  SizeType npo = ppo_num();
  vector<pair<SizeType, SizeType>> tmp_list(npo);
  for ( SizeType i: Range(npo) ) {
    auto onode = ppo(i);
    // onode の TFI のノード数を計算する．
    vector<bool> mark(node_num(), false);
    SizeType n = tfimark(onode, mark);
    tmp_list[i] = make_pair(n, i);
  }

  // TFI のサイズの昇順にソートする．
  sort(tmp_list.begin(), tmp_list.end(), Lt());
  // tmp_list の順に mPPOArray2 にセットする．
  for ( auto& p: tmp_list ) {
    SizeType opos = p.second;
    auto onode = mPPOArray[opos];
    SizeType opos2 = mPPOArray2.size();
    mPPOArray2.push_back(onode);
    const_cast<TpgNode*>(onode)->set_output_id2(opos2);
  }

  // immediate dominator を求める．
  for ( auto i: Range(node_num()) ) {
    auto node = const_cast<TpgNode*>(mNodeArray[node_num() - i - 1]);
    const TpgNode* imm_dom = nullptr;
    if ( !node->is_ppo() ) {
      SizeType nfo = node->fanout_num();
      if ( nfo > 0 ) {
	bool first = true;
	for ( auto onode: node->fanout_list() ) {
	  if ( first ) {
	    imm_dom = onode;
	    first = false;
	  }
	  else {
	    imm_dom = merge(imm_dom, onode);
	  }
	}
      }
    }
    node->set_imm_dom(imm_dom);
  }

  //////////////////////////////////////////////////////////////////////
  // FFR と MFFC の根のノードを求める．
  //////////////////////////////////////////////////////////////////////
  vector<const TpgNode*> ffr_root_list;
  vector<const TpgNode*> mffc_root_list;
  for ( auto node: node_list() ) {
    if ( !dmarks[node->id()] ) {
      // データ系のノードでなければスキップ
      continue;
    }
    if ( node->ffr_root() == node ) {
      ffr_root_list.push_back(node);

      // MFFC の根は必ず FFR の根でもある．
      if ( node->imm_dom() == nullptr ) {
	mffc_root_list.push_back(node);
      }
    }
  }

  //////////////////////////////////////////////////////////////////////
  // FFR の情報を作る．
  //////////////////////////////////////////////////////////////////////
  SizeType ffr_num = ffr_root_list.size() ;
  mFfrArray.clear();
  mFfrArray.resize(ffr_num);
  for ( SizeType i: Range(ffr_num) ) {
    auto node = ffr_root_list[i];
    set_ffr(i, node);
  }

  //////////////////////////////////////////////////////////////////////
  // MFFC の情報を作る．
  //////////////////////////////////////////////////////////////////////
  SizeType mffc_num = mffc_root_list.size();
  mMffcArray.clear();
  mMffcArray.resize(mffc_num);
  for ( SizeType i: Range(mffc_num) ) {
    auto node = mffc_root_list[i];
    set_mffc(i, node);
  }
}

// @brief 代表故障を設定する．
SizeType
TpgNetworkImpl::set_rep_faults(
  const TpgNode* node
)
{
  vector<const TpgFault*> fault_list;

  if ( node->fanout_num() == 1 ) {
    auto onode = node->fanout_list()[0];
    // ファンアウト先が一つならばそのファンイン
    // ブランチの故障と出力の故障は等価
    SizeType ipos = 0;
    for ( auto inode: onode->fanin_list() ) {
      if ( inode == node ) {
	break;
      }
      ++ ipos;
    }
    ASSERT_COND( ipos < onode->fanin_num() );

    auto rep0 = _node_input_fault(onode->id(), Fval2::zero, ipos);
    auto of0 = _node_output_fault(node->id(), Fval2::zero);
    if ( of0 != nullptr ) {
      of0->set_rep(rep0);
    }

    auto rep1 = _node_input_fault(onode->id(), Fval2::one, ipos);
    auto of1 = _node_output_fault(node->id(), Fval2::one);
    if ( of1 != nullptr ){
      of1->set_rep(rep1);
    }
  }

  if ( !node->is_ppo() ) {
    // of0, of1 の代表故障が設定されていない場合には自分自身を代表故障とする．
    auto of0 = _node_output_fault(node->id(), Fval2::zero);
    if ( of0 != nullptr ) {
      auto rep0 = of0->rep_fault();
      if ( rep0 == nullptr ) {
	of0->set_rep(of0);
	fault_list.push_back(of0);
      }
      else {
	of0->set_rep(rep0->rep_fault());
      }
    }

    auto of1 = _node_output_fault(node->id(), Fval2::one);
    if ( of1 != nullptr ) {
      auto rep1 = of1->rep_fault();
      if ( rep1 == nullptr ) {
	of1->set_rep(of1);
	fault_list.push_back(of1);
      }
      else {
	of1->set_rep(rep1->rep_fault());
      }
    }
  }

  for ( auto i: Range(node->fanin_num()) ) {
    // if0, if1 の代表故障が設定されていない場合には自分自身を代表故障とする．
    auto if0 = _node_input_fault(node->id(), Fval2::zero, i);
    if ( if0 != nullptr ) {
      auto rep0 = if0->rep_fault();
      if ( rep0 == nullptr ) {
	if0->set_rep(if0);
	fault_list.push_back(if0);
      }
      else {
	if0->set_rep(rep0->rep_fault());
      }
    }

    auto if1 = _node_input_fault(node->id(), Fval2::one, i);
    if ( if1 != nullptr ) {
      auto rep1 = if1->rep_fault();
      if ( rep1 == nullptr ) {
	if1->set_rep(if1);
	fault_list.push_back(if1);
      }
      else {
	if1->set_rep(rep1->rep_fault());
      }
    }
  }

  // node の代表故障をセットする．
  mAuxInfoArray[node->id()].set_fault_list(fault_list);

  return fault_list.size();
}

// @brief FFR の情報を設定する．
void
TpgNetworkImpl::set_ffr(
  SizeType id,
  const TpgNode* root
)
{
  auto& ffr = mFfrArray[id];

  ffr.mRoot = root;

  // root を根とするFFRの故障リストを求める．
  //vector<const TpgFault*> fault_list;

  // root を根とするFFRの入力のリスト
  //vector<const TpgNode*> input_list;
  // input_list の重複チェック用のハッシュ表
  unordered_set<int> input_hash;

  // DFS を行うためのスタック
  vector<const TpgNode*> node_stack;
  node_stack.push_back(root);
  while ( !node_stack.empty() ) {
    auto node = node_stack.back();
    node_stack.pop_back();

    mAuxInfoArray[node->id()].add_to_fault_list(ffr.mFaultList);

    for ( auto inode: node->fanin_list() ) {
      if ( inode->ffr_root() == inode || inode->is_ppi() ) {
	// inode は他の FFR の根
	if ( input_hash.count(inode->id()) == 0 ) {
	  input_hash.emplace(inode->id());
	  ffr.mInputList.push_back(inode);
	}
      }
      else {
	node_stack.push_back(inode);
      }
    }
  }

  mAuxInfoArray[root->id()].set_ffr(id);
}

// @brief MFFC の情報を設定する．
void
TpgNetworkImpl::set_mffc(
  SizeType id,
  const TpgNode* root
)
{
  auto& mffc = mMffcArray[id];

  mffc.mRoot = root;

  // root を根とする MFFC の情報を得る．
  vector<bool> mark(node_num());
  vector<const TpgNode*> node_list;
  //vector<const TpgFFR*> ffr_list;
  //vector<const TpgFault*> fault_list;

  node_list.push_back(root);
  mark[root->id()] = true;
  while ( !node_list.empty() ) {
    auto node = node_list.back();
    node_list.pop_back();

    if ( node->ffr_root() == node ) {
      auto ffr_id = mAuxInfoArray[node->id()].ffr();
      mffc.mFfrList.push_back(TpgFFR{this, ffr_id});
    }

    mAuxInfoArray[node->id()].add_to_fault_list(mffc.mFaultList);

    for ( auto inode: node->fanin_list() ) {
      if ( !mark[inode->id()] &&
	   inode->imm_dom() != nullptr ) {
	mark[inode->id()] = true;
	node_list.push_back(inode);
      }
    }
  }

  //mAuxInfoArray[root->id()].set_mffc(mffc);

  //mffc->set(root, ffr_list, fault_list);
}

END_NAMESPACE_DRUID
