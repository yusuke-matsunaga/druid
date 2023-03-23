
/// @File TpgNetworkImpl.cc
/// @brief TpgNetworkImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2019, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "TpgNetworkImpl.h"
#include "TpgGate.h"
#include "TpgNode.h"
#include "GateType.h"
#include "TpgFault.h"
#include "TpgGateImpl.h"

#include "TpgPPI.h"
#include "TpgPPO.h"
#include "TpgDffControl.h"
#include "TpgLogic.h"

#include "Val3.h"

#include "ym/Expr.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス TpgNetworkImpl
//////////////////////////////////////////////////////////////////////

// @brief 入力ノードを生成する．
TpgPPI*
TpgNetworkImpl::make_input_node(
  const string& name
)
{
  auto node = new TpgInput;
  reg_ppi(node, name);

  return node;
}

// @brief 出力ノードを生成する．
TpgPPO*
TpgNetworkImpl::make_output_node(
  const string& name,
  const TpgNode* inode
)
{
  auto node = new TpgOutput{inode};
  reg_ppo(node, name);

  return node;
}

// @brief DFFの入力ノードを生成する．
TpgPPO*
TpgNetworkImpl::make_dff_input_node(
  SizeType dff_id,
  const string& name,
  const TpgNode* inode
)
{
  auto node = new TpgDffInput{dff_id, inode};
  reg_ppo(node, name);

  auto& dff = mDFFArray[dff_id];
  dff.mInput = node;

  return node;
}

// @brief DFFの出力ノードを生成する．
TpgPPI*
TpgNetworkImpl::make_dff_output_node(
  SizeType dff_id,
  const string& name
)
{
  auto node = new TpgDffOutput{dff_id};
  reg_ppi(node, name);

  auto& dff = mDFFArray[dff_id];
  dff.mOutput = node;

  return node;
}

// @brief DFFのクロック端子を生成する．
TpgNode*
TpgNetworkImpl::make_dff_clock_node(
  SizeType dff_id,
  const string& name,
  const TpgNode* inode
)
{
  auto node = new TpgDffClock{dff_id, inode};
  reg_node(node);

  auto& dff = mDFFArray[dff_id];
  dff.mClock = node;

  return node;
}

// @brief DFFのクリア端子を生成する．
TpgNode*
TpgNetworkImpl::make_dff_clear_node(
  SizeType dff_id,
  const string& name,
  const TpgNode* inode
)
{
  auto node = new TpgDffClear{dff_id, inode};
  reg_node(node);

  auto& dff = mDFFArray[dff_id];
  dff.mClear = node;

  return node;
}

// @brief DFFのプリセット端子を生成する．
TpgNode*
TpgNetworkImpl::make_dff_preset_node(
  SizeType dff_id,
  const string& name,
  const TpgNode* inode
)
{
  auto node = new TpgDffPreset{dff_id, inode};
  reg_node(node);

  auto& dff = mDFFArray[dff_id];
  dff.mPreset = node;

  return node;
}

// @brief 論理ノードを生成する．
TpgNode*
TpgNetworkImpl::make_logic_node(
  const string& src_name,
  const GateType* gate_type,
  const vector<const TpgNode*>& fanin_list,
  vector<vector<const TpgNode*>>& connection_list
)
{

  TpgNode* node = nullptr;
  TpgGateImpl* gate = nullptr;
  if ( gate_type->is_simple() ) {
    // 組み込み型の場合．
    auto prim_type = gate_type->primitive_type();
    node = make_prim_node(src_name, prim_type, fanin_list,
			  connection_list);
    gate = new TpgGate_Simple{src_name, gate_type, node};
  }
  else {
    auto expr = gate_type->expr();

    // ブランチの情報
    SizeType ni = fanin_list.size();
    vector<TpgGate::BranchInfo> branch_info(ni);

    // 論理式の葉(リテラル)に対応するノードを入れる配列．
    // pos * 2 + 0: 肯定のリテラル
    // pos * 2 + 1: 否定のリテラルに対応する．
    vector<const TpgNode*> leaf_nodes(ni * 2, nullptr);
    for ( auto i: Range(ni) ) {
      SizeType p_num = expr.literal_num(i, false);
      SizeType n_num = expr.literal_num(i, true);
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
	  auto dummy_buff = make_buff_node(string{}, inode,
					   connection_list);
	  leaf_nodes[i * 2 + 0] = dummy_buff;
	  // このバッファの入力が故障位置となる．
	  branch_info[i] = TpgGate::BranchInfo{dummy_buff, 0};
	}
      }
      else {
	if ( p_num > 0 ) {
	  // 肯定と否定のリテラルがともに現れる場合
	  // ブランチの故障に対応するためにダミーのバッファを作る．
	  auto dummy_buff = make_buff_node(string{}, inode,
					   connection_list);
	  inode = dummy_buff;
	  leaf_nodes[i * 2 + 0] = dummy_buff;
	}

	// 否定のリテラルに対応するNOTゲートを作る．
	auto not_gate = make_not_node(string{}, inode,
				      connection_list);
	leaf_nodes[i * 2 + 1] = not_gate;

	if ( p_num == 0 ) {
	  inode = not_gate;
	}

	branch_info[i] = TpgGate::BranchInfo{inode, 0};
      }
    }

    // expr の内容を表す TpgNode の木を作る．
    node = make_cplx_node(src_name, expr, leaf_nodes, branch_info,
			  connection_list);
    gate = new TpgGate_Cplx{src_name, gate_type, node, branch_info};
  }

  mGateArray.push_back(gate);

  return node;
}

// @brief 論理式から TpgNode の木を生成する．
TpgNode*
TpgNetworkImpl::make_cplx_node(
  const string& name,
  const Expr& expr,
  const vector<const TpgNode*>& leaf_nodes,
  vector<TpgGate::BranchInfo>& branch_info,
  vector<vector<const TpgNode*>>& connection_list
)
{
  // expr はリテラルではない．
  ASSERT_COND( !expr.is_literal() );
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
    ASSERT_NOT_REACHED;
  }

  // 子供の論理式を表すノード(の木)を作る．
  vector<const TpgNode*> fanins;
  fanins.reserve(expr.operand_num());
  for ( auto expr1: expr.operand_list() ) {
    const TpgNode* inode;
    if ( expr1.is_posi_literal() ) {
      SizeType iid = expr1.varid();
      inode = leaf_nodes[iid * 2 + 0];
    }
    else if ( expr1.is_nega_literal() ) {
      SizeType iid = expr1.varid();
      inode = leaf_nodes[iid * 2 + 1];
    }
    else {
      inode = make_cplx_node(string{}, expr1, leaf_nodes, branch_info,
			     connection_list);
    }
    ASSERT_COND( inode != nullptr );
    fanins.push_back(inode);
  }
  // fanins[] を確保するオーバーヘッドがあるが，
  // 子供のノードよりも先に親のノードを確保するわけには行かない．
  auto* node = make_prim_node(name, gate_type, fanins,
			      connection_list);

  // オペランドがリテラルの場合，inode_array[]
  // の設定を行う．
  SizeType ipos = 0;
  for ( auto expr1: expr.operand_list() ) {
    // 美しくないけどスマートなやり方を思いつかない．
    if ( expr1.is_posi_literal() ) {
      SizeType iid = expr1.varid();
      if ( branch_info[iid].node == nullptr ) {
	branch_info[iid] = TpgGate::BranchInfo{node, ipos};
      }
    }
    ++ ipos;
  }

  return node;
}

// @brief バッファを生成する．
TpgNode*
TpgNetworkImpl::make_buff_node(
  const string& name,
  const TpgNode* fanin,
  vector<vector<const TpgNode*>>& connection_list
)
{
  return make_prim_node(name, PrimType::Buff, {fanin},
			connection_list);
}

// @brief インバーターを生成する．
TpgNode*
TpgNetworkImpl::make_not_node(
  const string& name,
  const TpgNode* fanin,
  vector<vector<const TpgNode*>>& connection_list
)
{
  return make_prim_node(name, PrimType::Not, {fanin},
			connection_list);
}

// @brief 組み込み型の論理ゲートを生成する．
TpgNode*
TpgNetworkImpl::make_prim_node(
  const string& name,
  PrimType type,
  const vector<const TpgNode*>& fanin_list,
  vector<vector<const TpgNode*>>& connection_list
)
{
  auto node = make_logic(type, fanin_list);
  reg_node(node);

  for ( auto inode: fanin_list ) {
    connection_list[inode->id()].push_back(node);
  }

  return node;
}

// @brief 論理ノードを作る．
TpgNode*
TpgNetworkImpl::make_logic(
  PrimType gate_type,
  const vector<const TpgNode*>& inode_list
)
{
  SizeType ni = inode_list.size();
  TpgNode* node = nullptr;
  switch ( gate_type ) {
  case PrimType::C0:
    ASSERT_COND( ni == 0 );

    node = new TpgLogicC0;
    break;

  case PrimType::C1:
    ASSERT_COND( ni == 0 );

    node = new TpgLogicC1;
    break;

  case PrimType::Buff:
    ASSERT_COND( ni == 1 );

    node = new TpgLogicBUFF{inode_list[0]};
    break;

  case PrimType::Not:
    ASSERT_COND( ni == 1 );

    node = new TpgLogicNOT{inode_list[0]};
    break;

  case PrimType::And:
    node = new TpgLogicAND{inode_list};
    break;

  case PrimType::Nand:
    node = new TpgLogicNAND{inode_list};
    break;

  case PrimType::Or:
    node = new TpgLogicOR{inode_list};
    break;

  case PrimType::Nor:
    node = new TpgLogicNOR{inode_list};
    break;

  case PrimType::Xor:
    node = new TpgLogicXOR{inode_list};
    break;

  case PrimType::Xnor:
    node = new TpgLogicXNOR{inode_list};
    break;

  default:
    ASSERT_NOT_REACHED;
  }

  return node;
}

// @brief PPI系のノードの登録
void
TpgNetworkImpl::reg_ppi(
  TpgPPI* node,
  const string& name
)
{
  reg_node(node);
  SizeType id = mPPIArray.size();
  node->set_input_id(id);
  mPPIArray.push_back(node);
  mPPINameArray.push_back(name);
}

// @brief PPO系のノードの登録
void
TpgNetworkImpl::reg_ppo(
  TpgPPO* node,
  const string& name
)
{
  reg_node(node);
  SizeType id = mPPOArray.size();
  node->set_output_id(id);
  mPPOArray.push_back(node);
  mPPONameArray.push_back(name);
}

// @brief 全てのノードの登録
void
TpgNetworkImpl::reg_node(
  TpgNode* node
)
{
  SizeType id = mNodeArray.size();
  mNodeArray.push_back(node);
  node->set_id(id);
}

END_NAMESPACE_DRUID
