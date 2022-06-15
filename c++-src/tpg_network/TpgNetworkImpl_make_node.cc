
/// @File TpgNetworkImpl.cc
/// @brief TpgNetworkImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2019 Yusuke Matsunaga
/// All rights reserved.

#include "TpgNetworkImpl.h"
#include "TpgNode.h"
#include "TpgGateInfo.h"
#include "TpgFault.h"
#include "TpgStemFault.h"
#include "TpgBranchFault.h"
#include "AuxNodeInfo.h"

#include "TpgInput.h"
#include "TpgOutput.h"

#include "TpgDffInput.h"
#include "TpgDffOutput.h"
#include "TpgDffClock.h"
#include "TpgDffClear.h"
#include "TpgDffPreset.h"

#include "TpgLogicC0.h"
#include "TpgLogicC1.h"
#include "TpgLogicBUFF.h"
#include "TpgLogicNOT.h"
#include "TpgLogicAND.h"
#include "TpgLogicNAND.h"
#include "TpgLogicOR.h"
#include "TpgLogicNOR.h"
#include "TpgLogicXOR.h"
#include "TpgLogicXNOR.h"

#include "GateType.h"
#include "Val3.h"

#include "ym/Expr.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス TpgNetworkImpl
//////////////////////////////////////////////////////////////////////

// @brief 入力ノードを生成する．
TpgNode*
TpgNetworkImpl::make_input_node(
  int iid,
  const string& name,
  SizeType fanout_num
)
{
  TpgNode* node = new TpgInput{iid, fanout_num};
  make_node_common(node, name, 0);

  // 出力位置の故障を生成
  for ( int val: {0, 1} ) {
    new_ofault(name, val, node);
  }

  return node;
}

// @brief 出力ノードを生成する．
TpgNode*
TpgNetworkImpl::make_output_node(
  int oid,
  const string& name,
  const TpgNode* inode
)
{
  TpgNode* node = new TpgOutput{oid, inode};
  make_node_common(node, name, 1);

  // 入力位置の故障を生成
  int ipos = 0;
  for ( int val: {0, 1} ) {
    new_ifault(name, ipos, val, InodeInfo(node, ipos), nullptr);
  }

  return node;
}

// @brief DFFの入力ノードを生成する．
TpgNode*
TpgNetworkImpl::make_dff_input_node(
  int oid,
  const TpgDff* dff,
  const string& name,
  const TpgNode* inode
)
{
  TpgNode* node = new TpgDffInput{oid, dff, inode};
  make_node_common(node, name, 1);

  // 入力位置の故障を生成
  int ipos = 0;
  for ( int val: {0, 1} ) {
    new_ifault(name, ipos, val, InodeInfo(node, ipos), nullptr);
  }

  return node;
}

// @brief DFFの出力ノードを生成する．
TpgNode*
TpgNetworkImpl::make_dff_output_node(
  int iid,
  const TpgDff* dff,
  const string& name,
  SizeType fanout_num
)
{
  TpgNode* node = new TpgDffOutput{iid, fanout_num, dff};
  make_node_common(node, name, 0);

  // 出力位置の故障を生成
  for ( int val: {0, 1} ) {
    new_ofault(name, val, node);
  }

  return node;
}

// @brief DFFのクロック端子を生成する．
TpgNode*
TpgNetworkImpl::make_dff_clock_node(
  const TpgDff* dff,
  const string& name,
  const TpgNode* inode
)
{
  TpgNode* node = new TpgDffClock{dff, inode};
  make_node_common(node, name, 1);

  // 入力位置の故障を生成
  int ipos = 0;
  for ( int val: {0, 1} ) {
    new_ifault(name, ipos, val, InodeInfo(node, ipos), nullptr);
  }

  return node;
}

// @brief DFFのクリア端子を生成する．
TpgNode*
TpgNetworkImpl::make_dff_clear_node(
  const TpgDff* dff,
  const string& name,
  const TpgNode* inode
)
{
  TpgNode* node = new TpgDffClear{dff, inode};
  make_node_common(node, name, 1);

  // 入力位置の故障を生成
  int ipos = 0;
  for ( int val: {0, 1} ) {
    new_ifault(name, ipos, val, InodeInfo(node, ipos), nullptr);
  }

  return node;
}

// @brief DFFのプリセット端子を生成する．
TpgNode*
TpgNetworkImpl::make_dff_preset_node(
  const TpgDff* dff,
  const string& name,
  const TpgNode* inode
)
{
  TpgNode* node = new TpgDffPreset{dff, inode};
  make_node_common(node, name, 1);

  // 入力位置の故障を生成
  int ipos = 0;
  for ( int val: {0, 1} ) {
    new_ifault(name, ipos, val, InodeInfo(node, ipos), nullptr);
  }

  return node;
}

// @brief 論理ノードを生成する．
TpgNode*
TpgNetworkImpl::make_logic_node(
  const string& src_name,
  const TpgGateInfo* node_info,
  const vector<const TpgNode*>& fanin_list,
  SizeType fanout_num,
  vector<pair<int, int>>& connection_list
)
{
  int ni = fanin_list.size();

  // 複合型の場合の入力ノードを納める配列
  vector<InodeInfo> inode_array(ni);

  TpgNode* node = nullptr;
  if ( node_info->is_simple() ) {
    // 組み込み型の場合．
    // 2入力以上の XOR/XNOR ゲートを2入力に分解する．
    GateType gate_type = node_info->gate_type();
    if ( gate_type == GateType::Xor && ni > 2 ) {
      vector<const TpgNode*> tmp_list(2);
      tmp_list[0] = fanin_list[0];
      tmp_list[1] = fanin_list[1];
      TpgNode* tmp_node = make_prim_node(string(), GateType::Xor, tmp_list, 1,
					 connection_list);
      inode_array[0].set(tmp_node, 0);
      inode_array[1].set(tmp_node, 1);
      for ( auto i: Range(2, ni) ) {
	tmp_list[0] = tmp_node;
	tmp_list[1] = fanin_list[i];
	if ( i < ni - 1 ) {
	  tmp_node = make_prim_node(string(), GateType::Xor, tmp_list, 1,
				    connection_list);
	}
	else {
	  tmp_node = make_prim_node(src_name, GateType::Xor, tmp_list, fanout_num,
				    connection_list);
	}
	inode_array[i].set(tmp_node, 1);
      }
      node = tmp_node;
    }
    else if ( gate_type == GateType::Xnor && ni > 2 ) {
      vector<const TpgNode*> tmp_list(2);
      tmp_list[0] = fanin_list[0];
      tmp_list[1] = fanin_list[1];
      TpgNode* tmp_node = make_prim_node(string(), GateType::Xor, tmp_list, 1,
					 connection_list);
      inode_array[0].set(tmp_node, 0);
      inode_array[1].set(tmp_node, 1);
      for ( auto i: Range(2, ni) ) {
	tmp_list[0] = tmp_node;
	tmp_list[1] = fanin_list[i];
	if ( i < ni - 1 ) {
	  tmp_node = make_prim_node(string(), GateType::Xor, tmp_list, 1,
				    connection_list);
	}
	else {
	  tmp_node = make_prim_node(src_name, GateType::Xnor, tmp_list, fanout_num,
				    connection_list);
	}
	inode_array[i].set(tmp_node, i);
      }
      node = tmp_node;
    }
    else {
      node = make_prim_node(src_name, gate_type, fanin_list, fanout_num,
			    connection_list);
      for ( auto i: Range(ni) ) {
	inode_array[i].set(node, i);
      }
    }
  }
  else {
    Expr expr = node_info->expr();

    // 論理式の葉(リテラル)に対応するノードを入れる配列．
    // pos * 2 + 0: 肯定のリテラル
    // pos * 2 + 1: 否定のリテラルに対応する．
    vector<const TpgNode*> leaf_nodes(ni * 2, nullptr);
    for ( auto i: Range(ni) ) {
      int p_num = expr.literal_num(VarId(i), false);
      int n_num = expr.literal_num(VarId(i), true);
      auto inode = fanin_list[i];
      if ( n_num == 0 ) {
	if ( p_num == 1 ) {
	  // 肯定のリテラルが1回だけ現れている場合
	  // 本当のファンインを直接つなぐ
	  leaf_nodes[i * 2 + 0] = inode;
	}
	else {
	  // 肯定のリテラルが2回以上現れている場合
	  // ブランチの故障に対応するためにダミーのバッファをつくる．
	  TpgNode* dummy_buff = make_buff_node(string(), inode, p_num,
					       connection_list);
	  leaf_nodes[i * 2 + 0] = dummy_buff;
	  // このバッファの入力が故障位置となる．
	  inode_array[i].set(dummy_buff, 0);
	}
      }
      else {
	if ( p_num > 0 ) {
	  // 肯定と否定のリテラルがともに現れる場合
	  // ブランチの故障に対応するためにダミーのバッファを作る．
	  TpgNode* dummy_buff = make_buff_node(string(), inode, p_num + 1,
					       connection_list);
	  inode = dummy_buff;
	  leaf_nodes[i * 2 + 0] = dummy_buff;
	}

	// 否定のリテラルに対応するNOTゲートを作る．
	TpgNode* not_gate = make_not_node(string(), inode, n_num,
					  connection_list);
	leaf_nodes[i * 2 + 1] = not_gate;

	if ( p_num > 0 ) {
	  inode_array[i].set(inode, 0);
	}
	else {
	  inode_array[i].set(not_gate, 0);
	}
      }
    }

    // expr の内容を表す TpgNode の木を作る．
    node = make_cplx_node(src_name, expr, leaf_nodes, inode_array, fanout_num,
			  connection_list);
  }

  // 出力位置の故障を生成
  for (int val: {0, 1} ) {
    new_ofault(src_name, val, node);
  }

  // 入力位置の故障を生成
  // 制御値を考慮して代表故障の設定を行う．
  for ( auto i: Range(ni) ) {
    Val3 oval0 = node_info->cval(i, Val3::_0);
    Val3 oval1 = node_info->cval(i, Val3::_1);

    TpgFault* rep0 = nullptr;
    if ( oval0 == Val3::_0 ) {
      rep0 = _node_output_fault(node->id(), 0);
    }
    else if ( oval0 == Val3::_1 ) {
      rep0 = _node_output_fault(node->id(), 1);
    }

    TpgFault* rep1 = nullptr;
    if ( oval1 == Val3::_0 ) {
      rep1 = _node_output_fault(node->id(), 0);
    }
    else if ( oval1 == Val3::_1 ) {
      rep1 = _node_output_fault(node->id(), 1);
    }
    new_ifault(src_name, i, 0, inode_array[i], rep0);
    new_ifault(src_name, i, 1, inode_array[i], rep1);
  }

  return node;
}

// @brief 論理式から TpgNode の木を生成する．
TpgNode*
TpgNetworkImpl::make_cplx_node(
  const string& name,
  const Expr& expr,
  const vector<const TpgNode*>& leaf_nodes,
  vector<InodeInfo>& inode_array,
  SizeType fanout_num,
  vector<pair<int, int>>& connection_list
)
{
  // expr はリテラルではない．
  ASSERT_COND( !expr.is_literal() );
  GateType gate_type;
  if ( expr.is_and() ) {
    gate_type = GateType::And;
  }
  else if ( expr.is_or() ) {
    gate_type = GateType::Or;
  }
  else if ( expr.is_xor() ) {
    gate_type = GateType::Xor;
  }
  else {
    ASSERT_NOT_REACHED;
  }

  // 子供の論理式を表すノード(の木)を作る．
  int nc = expr.child_num();
  vector<const TpgNode*> fanins(nc);
  for ( auto i: Range(nc) ) {
    const Expr& expr1 = expr.child(i);
    const TpgNode* inode;
    if ( expr1.is_posi_literal() ) {
      int iid = expr1.varid().val();
      inode = leaf_nodes[iid * 2 + 0];
    }
    else if ( expr1.is_nega_literal() ) {
      int iid = expr1.varid().val();
      inode = leaf_nodes[iid * 2 + 1];
    }
    else {
      inode = make_cplx_node(string{}, expr1, leaf_nodes, inode_array, 1,
			     connection_list);
    }
    ASSERT_COND( inode != nullptr );
    fanins[i] = inode;
  }
  // fanins[] を確保するオーバーヘッドがあるが，
  // 子供のノードよりも先に親のノードを確保するわけには行かない．
  TpgNode* node = make_prim_node(name, gate_type, fanins, fanout_num,
				 connection_list);

  // オペランドがリテラルの場合，inode_array[]
  // の設定を行う．
  for ( auto i: Range(nc) ) {
    // 美しくないけどスマートなやり方を思いつかない．
    const Expr& expr1 = expr.child(i);
    if ( expr1.is_posi_literal() ) {
      int iid = expr1.varid().val();
      if ( inode_array[iid].mNode == nullptr ) {
	inode_array[iid].set(node, i);
      }
    }
  }

  return node;
}

// @brief バッファを生成する．
TpgNode*
TpgNetworkImpl::make_buff_node(
  const string& name,
  const TpgNode* fanin,
  SizeType fanout_num,
  vector<pair<int, int>>& connection_list
)
{
  return make_prim_node(name, GateType::Buff, {fanin}, fanout_num,
			connection_list);
}

// @brief インバーターを生成する．
TpgNode*
TpgNetworkImpl::make_not_node(
  const string& name,
  const TpgNode* fanin,
  SizeType fanout_num,
  vector<pair<int, int>>& connection_list
)
{
  return make_prim_node(name, GateType::Not, {fanin}, fanout_num,
			connection_list);
}

// @brief 組み込み型の論理ゲートを生成する．
TpgNode*
TpgNetworkImpl::make_prim_node(
  const string& name,
  GateType type,
  const vector<const TpgNode*>& fanin_list,
  SizeType fanout_num,
  vector<pair<int, int>>& connection_list
)
{
  TpgNode* node = make_logic(type, fanin_list, fanout_num);
  make_node_common(node, name, fanin_list.size());

  for ( auto inode: fanin_list ) {
    connection_list.push_back(make_pair(inode->id(), node->id()));
  }

  return node;
}

// @brief 論理ノードを作る．
TpgNode*
TpgNetworkImpl::make_logic(
  GateType gate_type,
  const vector<const TpgNode*>& inode_list,
  SizeType fanout_num
)
{
  int ni = inode_list.size();
  TpgNode* node = nullptr;
  switch ( gate_type ) {
  case GateType::Const0:
    ASSERT_COND( ni == 0 );

    node = new TpgLogicC0{fanout_num};
    break;

  case GateType::Const1:
    ASSERT_COND( ni == 0 );

    node = new TpgLogicC1{fanout_num};
    break;

  case GateType::Buff:
    ASSERT_COND( ni == 1 );

    node = new TpgLogicBUFF{inode_list[0], fanout_num};
    break;

  case GateType::Not:
    ASSERT_COND( ni == 1 );

    node = new TpgLogicNOT{inode_list[0], fanout_num};
    break;

  case GateType::And:
    node = new TpgLogicAND{inode_list, fanout_num};
    break;

  case GateType::Nand:
    node = new TpgLogicNAND{inode_list, fanout_num};
    break;

  case GateType::Or:
    node = new TpgLogicOR{inode_list, fanout_num};
    break;

  case GateType::Nor:
    node = new TpgLogicNOR{inode_list, fanout_num};
    break;

  case GateType::Xor:
    ASSERT_COND( ni == 2 );

    node = new TpgLogicXOR2{inode_list, fanout_num};
    break;

  case GateType::Xnor:
    ASSERT_COND( ni == 2 );

    node = new TpgLogicXNOR2{inode_list, fanout_num};
    break;

  default:
    ASSERT_NOT_REACHED;
  }

  return node;
}

// @brief make_XXX_node の共通処理
void
TpgNetworkImpl::make_node_common(
  TpgNode* node,
  const string& name,
  SizeType ni
)
{
  SizeType id = mNodeArray.size();
  mNodeArray.push_back(node);
  node->set_id(id);

  ASSERT_COND( mAuxInfoArray.size() == id );
  mAuxInfoArray.push_back({name, ni});
}

// @brief 出力の故障を作る．
void
TpgNetworkImpl::new_ofault(
  const string& name,
  int val,
  const TpgNode* node
)
{
  TpgFaultBase* f = new TpgStemFault(mFaultNum, val, node, name, nullptr);
  mAuxInfoArray[node->id()].set_output_fault(val, f);
  ++ mFaultNum;
}

// @brief 入力の故障を作る．
void
TpgNetworkImpl::new_ifault(
  const string& name,
  SizeType ipos,
  int val,
  const InodeInfo& inode_info,
  TpgFault* rep
)
{
  const TpgNode* node = inode_info.mNode;
  int inode_pos = inode_info.mPos;
  const TpgNode* inode = node->fanin(inode_pos);
  TpgFaultBase* f = new TpgBranchFault(mFaultNum, val, node, name, ipos, inode, inode_pos, rep);
  mAuxInfoArray[node->id()].set_input_fault(inode_pos, val, f);
  ++ mFaultNum;
}

END_NAMESPACE_DRUID
