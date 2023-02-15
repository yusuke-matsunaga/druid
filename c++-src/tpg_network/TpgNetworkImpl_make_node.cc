
/// @File TpgNetworkImpl.cc
/// @brief TpgNetworkImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2019, 2022 Yusuke Matsunaga
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
  const string& name
)
{
  SizeType id = mPPIArray.size();

  auto node = new TpgInput{id};
  make_node_common(node, name, 0);

  // 出力位置の故障を生成
  for ( auto val: {Fval2::zero, Fval2::one} ) {
    new_ofault(name, val, node);
  }

  mPPIArray.push_back(node);

  return node;
}

// @brief 出力ノードを生成する．
TpgNode*
TpgNetworkImpl::make_output_node(
  const string& name,
  const TpgNode* inode
)
{
  SizeType id = mPPOArray.size();

  auto node = new TpgOutput{id, inode};
  make_node_common(node, name, 1);

  // 入力位置の故障を生成
  SizeType ipos = 0;
  for ( auto val: {Fval2::zero, Fval2::one} ) {
    new_ifault(name, ipos, val, InodeInfo{node, ipos}, nullptr);
  }

  mPPOArray.push_back(node);

  return node;
}

// @brief DFFの入力ノードを生成する．
TpgNode*
TpgNetworkImpl::make_dff_input_node(
  SizeType dff_id,
  const string& name,
  const TpgNode* inode
)
{
  SizeType id = mPPOArray.size();
  auto node = new TpgDffInput{id, dff_id, inode};
  make_node_common(node, name, 1);
  auto& dff = mDFFArray[dff_id];
  dff.mInput = node;

  // 入力位置の故障を生成
  SizeType ipos = 0;
  for ( auto val: {Fval2::zero, Fval2::one} ) {
    new_ifault(name, ipos, val, InodeInfo{node, ipos}, nullptr);
  }

  mPPOArray.push_back(node);

  return node;
}

// @brief DFFの出力ノードを生成する．
TpgNode*
TpgNetworkImpl::make_dff_output_node(
  SizeType dff_id,
  const string& name
)
{
  SizeType id = mPPIArray.size();
  auto node = new TpgDffOutput{id, dff_id};
  make_node_common(node, name, 0);
  auto& dff = mDFFArray[dff_id];
  dff.mOutput = node;

  // 出力位置の故障を生成
  for ( auto val: {Fval2::zero, Fval2::one} ) {
    new_ofault(name, val, node);
  }

  mPPIArray.push_back(node);

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
  make_node_common(node, name, 1);
  auto& dff = mDFFArray[dff_id];
  dff.mClock = node;

  // 入力位置の故障を生成
  SizeType ipos = 0;
  for ( auto val: {Fval2::zero, Fval2::one} ) {
    new_ifault(name, ipos, val, InodeInfo{node, ipos}, nullptr);
  }

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
  make_node_common(node, name, 1);
  auto& dff = mDFFArray[dff_id];
  dff.mClear = node;

  // 入力位置の故障を生成
  SizeType ipos = 0;
  for ( auto val: {Fval2::zero, Fval2::one} ) {
    new_ifault(name, ipos, val, InodeInfo{node, ipos}, nullptr);
  }

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
  make_node_common(node, name, 1);
  auto& dff = mDFFArray[dff_id];
  dff.mPreset = node;

  // 入力位置の故障を生成
  SizeType ipos = 0;
  for ( auto val: {Fval2::zero, Fval2::one} ) {
    new_ifault(name, ipos, val, InodeInfo{node, ipos}, nullptr);
  }

  return node;
}

// @brief 論理ノードを生成する．
TpgNode*
TpgNetworkImpl::make_logic_node(
  const string& src_name,
  const TpgGateInfo* node_info,
  const vector<const TpgNode*>& fanin_list,
  vector<vector<const TpgNode*>>& connection_list
)
{
  SizeType ni = fanin_list.size();

  // 複合型の場合の入力ノードを納める配列
  vector<InodeInfo> inode_array(ni);

  TpgNode* node = nullptr;
  if ( node_info->is_simple() ) {
    // 組み込み型の場合．
    // 2入力以上の XOR/XNOR ゲートを2入力に分解する．
    auto gate_type = node_info->gate_type();
    if ( gate_type == PrimType::Xor && ni > 2 ) {
      vector<const TpgNode*> tmp_list(2);
      tmp_list[0] = fanin_list[0];
      tmp_list[1] = fanin_list[1];
      auto tmp_node = make_prim_node(string{}, PrimType::Xor, tmp_list,
				     connection_list);
      inode_array[0].set(tmp_node, 0);
      inode_array[1].set(tmp_node, 1);
      for ( auto i: Range(2, ni) ) {
	tmp_list[0] = tmp_node;
	tmp_list[1] = fanin_list[i];
	if ( i < ni - 1 ) {
	  tmp_node = make_prim_node(string(), PrimType::Xor, tmp_list,
				    connection_list);
	}
	else {
	  tmp_node = make_prim_node(src_name, PrimType::Xor, tmp_list,
				    connection_list);
	}
	inode_array[i].set(tmp_node, 1);
      }
      node = tmp_node;
    }
    else if ( gate_type == PrimType::Xnor && ni > 2 ) {
      vector<const TpgNode*> tmp_list(2);
      tmp_list[0] = fanin_list[0];
      tmp_list[1] = fanin_list[1];
      auto tmp_node = make_prim_node(string{}, PrimType::Xor, tmp_list,
				     connection_list);
      inode_array[0].set(tmp_node, 0);
      inode_array[1].set(tmp_node, 1);
      for ( auto i: Range(2, ni) ) {
	tmp_list[0] = tmp_node;
	tmp_list[1] = fanin_list[i];
	if ( i < ni - 1 ) {
	  tmp_node = make_prim_node(string{}, PrimType::Xor, tmp_list,
				    connection_list);
	}
	else {
	  tmp_node = make_prim_node(src_name, PrimType::Xnor, tmp_list,
				    connection_list);
	}
	inode_array[i].set(tmp_node, i);
      }
      node = tmp_node;
    }
    else {
      node = make_prim_node(src_name, gate_type, fanin_list,
			    connection_list);
      for ( auto i: Range(ni) ) {
	inode_array[i].set(node, i);
      }
    }
  }
  else {
    auto expr = node_info->expr();

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
	}
	else {
	  // 肯定のリテラルが2回以上現れている場合
	  // ブランチの故障に対応するためにダミーのバッファをつくる．
	  auto dummy_buff = make_buff_node(string{}, inode,
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
	  auto dummy_buff = make_buff_node(string{}, inode,
					   connection_list);
	  inode = dummy_buff;
	  leaf_nodes[i * 2 + 0] = dummy_buff;
	}

	// 否定のリテラルに対応するNOTゲートを作る．
	auto not_gate = make_not_node(string{}, inode,
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
    node = make_cplx_node(src_name, expr, leaf_nodes, inode_array,
			  connection_list);
  }

  // 出力位置の故障を生成
  for ( auto val: {Fval2::zero, Fval2::one} ) {
    new_ofault(src_name, val, node);
  }

  // 入力位置の故障を生成
  // 制御値を考慮して代表故障の設定を行う．
  for ( auto i: Range(ni) ) {
    Val3 oval0 = node_info->cval(i, Val3::_0);
    Val3 oval1 = node_info->cval(i, Val3::_1);

    TpgFault* rep0 = nullptr;
    if ( oval0 == Val3::_0 ) {
      rep0 = _node_output_fault(node->id(), Fval2::zero);
    }
    else if ( oval0 == Val3::_1 ) {
      rep0 = _node_output_fault(node->id(), Fval2::one);
    }
    new_ifault(src_name, i, Fval2::zero, inode_array[i], rep0);

    TpgFault* rep1 = nullptr;
    if ( oval1 == Val3::_0 ) {
      rep1 = _node_output_fault(node->id(), Fval2::zero);
    }
    else if ( oval1 == Val3::_1 ) {
      rep1 = _node_output_fault(node->id(), Fval2::one);
    }
    new_ifault(src_name, i, Fval2::one, inode_array[i], rep1);
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
      inode = make_cplx_node(string{}, expr1, leaf_nodes, inode_array,
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
      if ( inode_array[iid].mNode == nullptr ) {
	inode_array[iid].set(node, ipos);
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
  make_node_common(node, name, fanin_list.size());

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
    ASSERT_COND( ni == 2 );

    node = new TpgLogicXOR2{inode_list};
    break;

  case PrimType::Xnor:
    ASSERT_COND( ni == 2 );

    node = new TpgLogicXNOR2{inode_list};
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
  Fval2 val,
  const TpgNode* node
)
{
  auto f = new TpgStemFault(mFaultNum, val, node, name, nullptr);
  mAuxInfoArray[node->id()].set_output_fault(val, f);
  ++ mFaultNum;
}

// @brief 入力の故障を作る．
void
TpgNetworkImpl::new_ifault(
  const string& name,
  SizeType ipos,
  Fval2 val,
  const InodeInfo& inode_info,
  TpgFault* rep
)
{
  auto node = inode_info.mNode;
  SizeType inode_pos = inode_info.mPos;
  auto inode = node->fanin(inode_pos);
  auto f = new TpgBranchFault(mFaultNum, val, node, name, ipos, inode, inode_pos, rep);
  mAuxInfoArray[node->id()].set_input_fault(inode_pos, val, f);
  ++ mFaultNum;
}

END_NAMESPACE_DRUID
