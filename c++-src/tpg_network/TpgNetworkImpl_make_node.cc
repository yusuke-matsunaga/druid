
/// @File TpgNetworkImpl.cc
/// @brief TpgNetworkImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.


#include "TpgNetworkImpl.h"
#include "TpgNode.h"
#include "TpgNodeFactory.h"
#include "TpgGateInfo.h"
#include "TpgFault.h"
#include "TpgStemFault.h"
#include "TpgBranchFault.h"
#include "AuxNodeInfo.h"

#include "GateType.h"
#include "Val3.h"

#include "ym/Expr.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス TpgNetworkImpl
//////////////////////////////////////////////////////////////////////

// @brief 入力ノードを生成する．
// @param[in] iid 入力の番号
// @param[in] name ノード名
// @param[in] fanout_num ファンアウト数
// @return 生成したノードを返す．
TpgNode*
TpgNetworkImpl::make_input_node(int iid,
				const string& name,
				int fanout_num)
{
  TpgNodeFactory factory(mAlloc);

  TpgNode* node = factory.make_input(mNodeNum, iid, fanout_num);
  mNodeArray[mNodeNum] = node;
  ++ mNodeNum;

  mAuxInfoArray[node->id()].init(name, 0, mAlloc);

  // 出力位置の故障を生成
  const char* c_name = node_name(node->id());
  for (int val = 0; val < 2; ++ val) {
    new_ofault(c_name, val, node);
  }

  return node;
}

// @brief 出力ノードを生成する．
// @param[in] oid 出力の番号
// @param[in] name ノード名
// @param[in] inode 入力のノード
// @param[in] fanout_num ファンアウト数
// @return 生成したノードを返す．
TpgNode*
TpgNetworkImpl::make_output_node(int oid,
				 const string& name,
				 TpgNode* inode)
{
  TpgNodeFactory factory(mAlloc);

  TpgNode* node = factory.make_output(mNodeNum, oid, inode);
  mNodeArray[mNodeNum] = node;
  ++ mNodeNum;

  mAuxInfoArray[node->id()].init(name, 1, mAlloc);

  // 入力位置の故障を生成
  const char* c_name = node_name(node->id());
  int ipos = 0;
  for ( int val = 0; val < 2; ++ val ) {
    new_ifault(c_name, ipos, val, InodeInfo(node, ipos), nullptr);
  }

  return node;
}

// @brief DFFの入力ノードを生成する．
// @param[in] oid 出力の番号
// @param[in] dff 接続しているDFF
// @param[in] name ノード名
// @param[in] inode 入力のノード
// @return 生成したノードを返す．
TpgNode*
TpgNetworkImpl::make_dff_input_node(int oid,
				    const TpgDff* dff,
				    const string& name,
				    TpgNode* inode)
{
  TpgNodeFactory factory(mAlloc);

  TpgNode* node = factory.make_dff_input(mNodeNum, oid, dff, inode);
  mNodeArray[mNodeNum] = node;
  ++ mNodeNum;

  mAuxInfoArray[node->id()].init(name, 1, mAlloc);

  // 入力位置の故障を生成
  const char* c_name = node_name(node->id());
  int ipos = 0;
  for ( int val = 0; val < 2; ++ val ) {
    new_ifault(c_name, ipos, val, InodeInfo(node, ipos), nullptr);
  }

  return node;
}

// @brief DFFの出力ノードを生成する．
// @param[in] iid 入力の番号
// @param[in] dff 接続しているDFF
// @param[in] name ノード名
// @param[in] fanout_num ファンアウト数
// @return 生成したノードを返す．
TpgNode*
TpgNetworkImpl::make_dff_output_node(int iid,
				     const TpgDff* dff,
				     const string& name,
				     int fanout_num)
{
  TpgNodeFactory factory(mAlloc);

  TpgNode* node = factory.make_dff_output(mNodeNum, iid, dff, fanout_num);
  mNodeArray[mNodeNum] = node;
  ++ mNodeNum;

  mAuxInfoArray[node->id()].init(name, 0, mAlloc);

  // 出力位置の故障を生成
  const char* c_name = node_name(node->id());
  for (int val = 0; val < 2; ++ val) {
    new_ofault(c_name, val, node);
  }

  return node;
}

// @brief DFFのクロック端子を生成する．
// @param[in] dff 接続しているDFF
// @param[in] name ノード名
// @param[in] inode 入力のノード
// @return 生成したノードを返す．
TpgNode*
TpgNetworkImpl::make_dff_clock_node(const TpgDff* dff,
				    const string& name,
				    TpgNode* inode)
{
  TpgNodeFactory factory(mAlloc);

  TpgNode* node = factory.make_dff_clock(mNodeNum, dff, inode);
  mNodeArray[mNodeNum] = node;
  ++ mNodeNum;

  mAuxInfoArray[node->id()].init(name, 1, mAlloc);

  // 入力位置の故障を生成
  const char* c_name = node_name(node->id());
  int ipos = 0;
  for ( int val = 0; val < 2; ++ val ) {
    new_ifault(c_name, ipos, val, InodeInfo(node, ipos), nullptr);
  }

  return node;
}

// @brief DFFのクリア端子を生成する．
// @param[in] dff 接続しているDFF
// @param[in] name ノード名
// @param[in] inode 入力のノード
// @return 生成したノードを返す．
TpgNode*
TpgNetworkImpl::make_dff_clear_node(const TpgDff* dff,
				    const string& name,
				    TpgNode* inode)
{
  TpgNodeFactory factory(mAlloc);

  TpgNode* node = factory.make_dff_clear(mNodeNum, dff, inode);
  mNodeArray[mNodeNum] = node;
  ++ mNodeNum;

  mAuxInfoArray[node->id()].init(name, 1, mAlloc);

  // 入力位置の故障を生成
  const char* c_name = node_name(node->id());
  int ipos = 0;
  for ( int val = 0; val < 2; ++ val ) {
    new_ifault(c_name, ipos, val, InodeInfo(node, ipos), nullptr);
  }

  return node;
}

// @brief DFFのプリセット端子を生成する．
// @param[in] dff 接続しているDFF
// @param[in] name ノード名
// @param[in] inode 入力のノード
// @return 生成したノードを返す．
TpgNode*
TpgNetworkImpl::make_dff_preset_node(const TpgDff* dff,
				     const string& name,
				     TpgNode* inode)
{
  TpgNodeFactory factory(mAlloc);

  TpgNode* node = factory.make_dff_preset(mNodeNum, dff, inode);
  mNodeArray[mNodeNum] = node;
  ++ mNodeNum;

  mAuxInfoArray[node->id()].init(name, 1, mAlloc);

  // 入力位置の故障を生成
  const char* c_name = node_name(node->id());
  int ipos = 0;
  for ( int val = 0; val < 2; ++ val ) {
    new_ifault(c_name, ipos, val, InodeInfo(node, ipos), nullptr);
  }

  return node;
}

// @brief 論理ノードを生成する．
// @param[in] src_name ノード名
// @param[in] node_info 論理関数の情報
// @param[in] fanin_list ファンインのリスト
// @param[in] fanout_num ファンアウト数
// @param[out] connection_list 接続リスト
// @return 生成したノードを返す．
TpgNode*
TpgNetworkImpl::make_logic_node(const string& src_name,
				const TpgGateInfo* node_info,
				const vector<TpgNode*>& fanin_list,
				int fanout_num,
				vector<pair<int, int>>& connection_list)
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
      vector<TpgNode*> tmp_list(2);
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
      vector<TpgNode*> tmp_list(2);
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
    vector<TpgNode*> leaf_nodes(ni * 2, nullptr);
    for ( auto i: Range(ni) ) {
      int p_num = expr.litnum(VarId(i), false);
      int n_num = expr.litnum(VarId(i), true);
      TpgNode* inode = fanin_list[i];
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
  const char* c_name = node_name(node->id());
  for (int val = 0; val < 2; ++ val) {
    new_ofault(c_name, val, node);
  }

  // 入力位置の故障を生成
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
    new_ifault(c_name, i, 0, inode_array[i], rep0);
    new_ifault(c_name, i, 1, inode_array[i], rep1);
  }

  return node;
}

// @brief 論理式から TpgNode の木を生成する．
// @param[in] name ノード名
// @param[in] expr 式
// @param[in] leaf_nodes 式のリテラルに対応するノードの配列
// @param[in] inode_array ファンインの対応関係を収める配列
// @param[in] fanout_num ファンアウト数
// @param[out] connection_list 接続リスト
// @return 生成したノードを返す．
//
// leaf_nodes は 変数番号 * 2 + (0/1) に
// 該当する変数の肯定/否定のリテラルが入っている．
TpgNode*
TpgNetworkImpl::make_cplx_node(const string& name,
			       const Expr& expr,
			       const vector<TpgNode*>& leaf_nodes,
			       vector<InodeInfo>& inode_array,
			       int fanout_num,
			       vector<pair<int, int>>& connection_list)
{
  if ( expr.is_posi_literal() ) {
    int iid = expr.varid().val();
    return leaf_nodes[iid * 2 + 0];
  }
  if ( expr.is_nega_literal() ) {
    int iid = expr.varid().val();
    return leaf_nodes[iid * 2 + 1];
  }

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
  vector<TpgNode*> fanins(nc);
  for ( auto i: Range(nc) ) {
    const Expr& expr1 = expr.child(i);
    TpgNode* inode = make_cplx_node(string(), expr1, leaf_nodes, inode_array, 1,
				    connection_list);
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

// @brief 組み込み型の論理ゲートを生成する．
// @param[in] name ノード名
// @param[in] type ゲートの型
// @param[in] fanin_list ファンインのリスト
// @param[in] fanout_num ファンアウト数
// @param[out] connection_list 接続リスト
// @return 生成したノードを返す．
TpgNode*
TpgNetworkImpl::make_prim_node(const string& name,
			       GateType type,
			       const vector<TpgNode*>& fanin_list,
			       int fanout_num,
			       vector<pair<int, int>>& connection_list)
{
  TpgNodeFactory factory(mAlloc);

  TpgNode* node = factory.make_logic(mNodeNum, type, fanin_list, fanout_num);
  mNodeArray[mNodeNum] = node;
  ++ mNodeNum;

  int id = node->id();
  int fanin_num = fanin_list.size();

  mAuxInfoArray[id].init(name, fanin_num, mAlloc);

  for ( auto i: Range(fanin_num) ) {
    connection_list.push_back(make_pair(fanin_list[i]->id(), id));
  }

  return node;
}

// @brief バッファを生成する．
// @param[in] name ノード名
// @param[in] fanin ファンインのノード
// @param[in] fanout_num ファンアウト数
// @param[out] connection_list 接続リスト
// @return 生成したノードを返す．
TpgNode*
TpgNetworkImpl::make_buff_node(const string& name,
			       TpgNode* fanin,
			       int fanout_num,
			       vector<pair<int, int>>& connection_list)
{
  return make_prim_node(name, GateType::Buff, vector<TpgNode*>(1, fanin), fanout_num,
			connection_list);
}

// @brief インバーターを生成する．
// @param[in] name ノード名
// @param[in] fanin ファンインのノード
// @param[in] fanout_num ファンアウト数
// @param[out] connection_list 接続リスト
// @return 生成したノードを返す．
TpgNode*
TpgNetworkImpl::make_not_node(const string& name,
			      TpgNode* fanin,
			      int fanout_num,
			      vector<pair<int, int>>& connection_list)
{
  return make_prim_node(name, GateType::Not, vector<TpgNode*>(1, fanin), fanout_num,
			connection_list);
}

// @brief 出力の故障を作る．
// @param[in] name 故障位置のノード名
// @param[in] val 故障値 ( 0 / 1 )
// @param[in] node 故障位置のノード
void
TpgNetworkImpl::new_ofault(const char* name,
			   int val,
			   const TpgNode* node)
{
  void* p = mAlloc.get_memory(sizeof(TpgStemFault));
  TpgFaultBase* f = new (p) TpgStemFault(mFaultNum, val, node, name, nullptr);
  mAuxInfoArray[node->id()].set_output_fault(val, f);
  ++ mFaultNum;
}

// @brief 入力の故障を作る．
// @param[in] name 故障位置のノード名
// @param[in] ipos 故障位置のファンイン番号
// @param[in] node 故障位置のノード
// @param[in] inode_pos node 上の入力位置
// @param[in] val 故障値
// @param[in] rep 代表故障
//
// プリミティブ型の場合は ipos と inode_pos は同一だが
// 複合型の場合には異なる．
void
TpgNetworkImpl::new_ifault(const char* name,
			   int ipos,
			   int val,
			   const InodeInfo& inode_info,
			   TpgFault* rep)
{
  const TpgNode* node = inode_info.mNode;
  int inode_pos = inode_info.mPos;
  const TpgNode* inode = node->fanin(inode_pos);
  void* p = mAlloc.get_memory(sizeof(TpgBranchFault));
  TpgFaultBase* f = new (p) TpgBranchFault(mFaultNum, val, node, name, ipos, inode, inode_pos, rep);
  mAuxInfoArray[node->id()].set_input_fault(inode_pos, val, f);
  ++ mFaultNum;
}

END_NAMESPACE_DRUID
