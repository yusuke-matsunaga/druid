﻿
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
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// BnFuncType を GateType に変換する．
inline
GateType
conv_to_gate_type(BnNodeType type)
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

// immediate dominator リストをマージする．
const TpgNode*
merge(const TpgNode* node1,
      const TpgNode* node2)
{
  for ( ; ; ) {
    if ( node1 == node2 ) {
      return node1;
    }
    if ( node1 == nullptr || node2 == nullptr ) {
      return nullptr;
    }
    int id1 = node1->id();
    int id2 = node2->id();
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
  operator()(const pair<int, int>& left,
	     const pair<int, int>& right)
  {
    return left.first < right.first;
  }

};


void
check_network_connection(const TpgNetworkImpl& network)
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
  int id
) const
{
  ASSERT_COND( id >= 0 && id < node_num() );

  return mAuxInfoArray[id].name();
}

// @brief ノードに関係した代表故障数を返す．
// @param[in] id ID番号 ( 0 <= id < node_num() )
int
TpgNetworkImpl::node_rep_fault_num(int id) const
{
  ASSERT_COND( id >= 0 && id < node_num() );

  return mAuxInfoArray[id].fault_num();
}

// @brief ノードに関係した代表故障を返す．
// @param[in] id ID番号 ( 0 <= id < node_num() )
// @param[in] pos 位置番号 ( 0 <= pos < node_rep_fault_num(id) )
const TpgFault*
TpgNetworkImpl::node_rep_fault(int id,
			       int pos) const
{
  ASSERT_COND( id >= 0 && id < node_num() );

  return mAuxInfoArray[id].fault(pos);
}

// @brief 出力の故障を得る．
// @param[in] id ノードID ( 0 <= id < node_num() )
// @param[in] val 故障値 ( 0 / 1 )
TpgFaultBase*
TpgNetworkImpl::_node_output_fault(int id,
				   int val)
{
  ASSERT_COND( id >= 0 && id < node_num() );

  return mAuxInfoArray[id].output_fault(val);
}

// @brief 入力の故障を得る．
TpgFaultBase*
TpgNetworkImpl::_node_input_fault(
  int id,
  int val,
  SizeType pos
)
{
  ASSERT_COND( id >= 0 && id < node_num() );

  return mAuxInfoArray[id].input_fault(pos, val);
}

// @brief DFF を得る．
// @param[in] pos 位置番号 ( 0 <= pos < dff_num() )
const TpgDff&
TpgNetworkImpl::dff(int pos) const
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
// @param[in] pos 位置番号 ( 0 <= pos < mffc_num() )
const TpgMFFC&
TpgNetworkImpl::mffc(int pos) const
{
  ASSERT_COND( pos >= 0 && pos < mffc_num() );

  return mMffcArray[pos];
}

// @brief MFFC のリストを得る．
const vector<TpgMFFC>&
TpgNetworkImpl::mffc_list() const
{
  return mMffcArray;
}

// @brief FFR を返す．
// @param[in] pos 位置番号 ( 0 <= pos < ffr_num() )
const TpgFFR&
TpgNetworkImpl::ffr(int pos) const
{
  ASSERT_COND( pos >= 0 && pos < ffr_num() );

  return mFfrArray[pos];
}

// @brief FFR のリストを得る．
const vector<TpgFFR>&
TpgNetworkImpl::ffr_list() const
{
  return mFfrArray;
}


BEGIN_NONAMESPACE

// @brief ノードの TFI にマークをつける．
int
tfimark(
  const TpgNode* node,
  vector<bool>& mark
)
{
  if ( mark[node->id()] ) {
    return 0;
  }
  mark[node->id()] = true;

  int n = 1;
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
  for ( int i: Range(network.expr_num()) ) {
    auto expr = network.expr(i);
    int ni = expr.input_size();
    const TpgGateInfo* node_info = node_info_mgr.complex_type(ni, expr);
    node_info_list.push_back(node_info);
  }

  //////////////////////////////////////////////////////////////////////
  // 追加で生成されるノード数を数える．
  //////////////////////////////////////////////////////////////////////
  int extra_node_num = 0;
  int nl = network.logic_num();
  for ( int src_id: network.logic_id_list() ) {
    auto& src_node = network.node(src_id);
    BnNodeType logic_type = src_node.type();
    if ( logic_type == BnNodeType::Expr ) {
      const TpgGateInfo* node_info = node_info_list[src_node.func_id()];
      extra_node_num += node_info->extra_node_num();
    }
    else if ( logic_type == BnNodeType::Xor || logic_type == BnNodeType::Xnor ) {
      int ni = src_node.fanin_num();
      extra_node_num += (ni - 2);
    }
  }

  //////////////////////////////////////////////////////////////////////
  // 要素数を数え，必要なメモリ領域を確保する．
  //////////////////////////////////////////////////////////////////////

  // BnPort は複数ビットの場合があり，さらに入出力が一緒なのでめんどくさい
  vector<int> input_map;
  vector<int> output_map;
  for ( int i: Range(network.port_num()) ) {
    auto& port = network.port(i);
    for ( auto b: Range(port.bit_width() ) ) {
      int id = port.bit(b);
      auto& node = network.node(id);
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
  int input_num = input_map.size();
  int output_num = output_map.size();
  int dff_num = network.dff_num();

  int dff_control_num = 0;
  for ( int i: Range(network.dff_num()) ) {
    auto& dff = network.dff(i);
    // まずクロックで一つ
    ++ dff_control_num;
    if ( dff.clear() != BNET_NULLID ) {
      // クリア端子で一つ
      ++ dff_control_num;
    }
    if ( dff.preset() != BNET_NULLID ) {
      // プリセット端子で一つ
      ++ dff_control_num;
    }
  }

  int nn = input_num + output_num + dff_num * 2 + nl + extra_node_num + dff_control_num;
  set_size(input_num, output_num, dff_num, nn);

  NodeMap node_map;
  vector<pair<int, int> > connection_list;
  vector<TpgNode*> tmp_node_list(nn, nullptr);

  //////////////////////////////////////////////////////////////////////
  // 入力ノードを作成する．
  //////////////////////////////////////////////////////////////////////
  for ( auto i: Range(mInputNum) ) {
    int id = input_map[i];
    auto& src_node = network.node(id);
    ASSERT_COND( src_node.is_input() );
    int nfo = src_node.fanout_num();
    TpgNode* node = make_input_node(i, src_node.name(), nfo);
    mPPIArray[i] = node;

    node_map.reg(id, node);
    tmp_node_list[node->id()] = node;
  }

  //////////////////////////////////////////////////////////////////////
  // DFFの出力ノードを作成する．
  //////////////////////////////////////////////////////////////////////
  for ( auto i: Range(dff_num) ) {
    auto& src_dff = network.dff(i);
    auto& src_node = network.node(src_dff.output());
    ASSERT_COND( src_node.is_input() );
    int nfo = src_node.fanout_num();
    TpgDff* dff = &mDffArray[i];
    int iid = i + mInputNum;
    TpgNode* node = make_dff_output_node(iid, dff, src_node.name(), nfo);
    mPPIArray[iid] = node;
    dff->mOutput = node;

    node_map.reg(src_node.id(), node);
    tmp_node_list[node->id()] = node;
  }

  //////////////////////////////////////////////////////////////////////
  // 論理ノードを作成する．
  // BnNetwork::logic_id_list() はトポロジカルソートされているので
  // 結果として TpgNode もトポロジカル順に並べられる．
  //////////////////////////////////////////////////////////////////////
  for ( int src_id: network.logic_id_list() ) {
    auto& src_node = network.node(src_id);
    const TpgGateInfo* node_info = nullptr;
    BnNodeType logic_type = src_node.type();
    if ( logic_type == BnNodeType::Expr ) {
      node_info = node_info_list[src_node.func_id()];
    }
    else {
      ASSERT_COND( logic_type != BnNodeType::TvFunc );
      GateType gate_type = conv_to_gate_type(logic_type);
      node_info = node_info_mgr.simple_type(gate_type);
    }

    // ファンインのノードを取ってくる．
    vector<const TpgNode*> fanin_array;
    fanin_array.reserve(src_node.fanin_num());
    for ( auto iid: src_node.fanin_id_list() ) {
      fanin_array.push_back(node_map.get(iid));
    }
    int nfo = src_node.fanout_num();
    TpgNode* node = make_logic_node(src_node.name(), node_info, fanin_array, nfo,
				    connection_list);

    // ノードを登録する．
    node_map.reg(src_id, node);
    tmp_node_list[node->id()] = node;
  }

  //////////////////////////////////////////////////////////////////////
  // 出力ノードを作成する．
  //////////////////////////////////////////////////////////////////////
  vector<TpgNode*> ppo_array(mPPOArray.size());
  for ( auto i: Range(mOutputNum) ) {
    int id = output_map[i];
    auto& src_node = network.node(id);
    ASSERT_COND( src_node.is_output() );
    TpgNode* inode = node_map.get(src_node.fanin_id(0));
    string buf = "*";
    buf += src_node.name();
    TpgNode* node = make_output_node(i, buf, inode);
    connection_list.push_back(make_pair(inode->id(), node->id()));
    mPPOArray[i] = node;
    ppo_array[i] = node;
    tmp_node_list[node->id()] = node;
  }

  //////////////////////////////////////////////////////////////////////
  // DFFの入力ノードを作成する．
  //////////////////////////////////////////////////////////////////////
  for ( auto i: Range(dff_num) ) {
    auto& src_dff = network.dff(i);
    auto& src_node = network.node(src_dff.input());

    TpgNode* inode = node_map.get(src_node.fanin_id(0));
    string dff_name = src_dff.name();
    string input_name = dff_name + ".input";
    TpgDff* dff = &mDffArray[i];
    int oid = i + mOutputNum;
    TpgNode* node = make_dff_input_node(oid, dff, input_name, inode);
    connection_list.push_back(make_pair(inode->id(), node->id()));
    mPPOArray[oid] = node;
    ppo_array[oid] = node;
    tmp_node_list[node->id()] = node;
    dff->mInput = node;

    // クロック端子を作る．
    auto& src_clock = network.node(src_dff.clock());
    TpgNode* clock_fanin = node_map.get(src_clock.fanin_id(0));
    string clock_name = dff_name + ".clock";
    TpgNode* clock = make_dff_clock_node(dff, clock_name, clock_fanin);
    connection_list.push_back(make_pair(clock_fanin->id(), clock->id()));
    tmp_node_list[clock->id()] = clock;
    dff->mClock = clock;

    // クリア端子を作る．
    if ( src_dff.clear() != BNET_NULLID ) {
      auto& src_clear = network.node(src_dff.clear());
      TpgNode* clear_fanin = node_map.get(src_clear.fanin_id(0));
      string clear_name = dff_name + ".clear";
      TpgNode* clear = make_dff_clear_node(dff, clear_name, clear_fanin);
      connection_list.push_back(make_pair(clear_fanin->id(), clear->id()));
      tmp_node_list[clear->id()] = clear;
      dff->mClear = clear;
    }

    // プリセット端子を作る．
    if ( src_dff.preset() != BNET_NULLID ) {
      auto& src_preset = network.node(src_dff.preset());
      TpgNode* preset_fanin = node_map.get(src_preset.fanin_id(0));
      string preset_name = dff_name + ".preset";
      TpgNode* preset = make_dff_preset_node(dff, preset_name, preset_fanin);
      connection_list.push_back(make_pair(preset_fanin->id(), preset->id()));
      tmp_node_list[preset->id()] = preset;
      dff->mPreset = preset;
    }
  }

  ASSERT_COND( node_num() == nn );

  //////////////////////////////////////////////////////////////////////
  // ファンアウトをセットする．
  //////////////////////////////////////////////////////////////////////
  for ( auto p: connection_list ) {
    auto from = tmp_node_list[p.first];
    auto to = mNodeArray[p.second];
    from->add_fanout(to);
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
  int rep_num = 0;
  for ( auto i: Range(node_num()) ) {
    // ノードごとに代表故障を設定する．
    // この処理は出力側から行う必要がある．
    const TpgNode* node = mNodeArray[node_num() - i - 1];
    if ( dmarks[node->id()] ) {
      int nf = set_rep_faults(node);
      rep_num += nf;
    }
  }

  mRepFaultArray.clear();
  mRepFaultArray.reserve(rep_num);
  for ( auto i: Range(node_num()) ) {
    const AuxNodeInfo& aux_node_info = mAuxInfoArray[i];
    int nf = aux_node_info.fault_num();
    for ( auto j: Range(nf) ) {
      const TpgFault* fault = aux_node_info.fault(j);
      mRepFaultArray.push_back(fault);
    }
  }


  //////////////////////////////////////////////////////////////////////
  // TFI のサイズの昇順に並べた出力順を
  // mPPOArray2 に記録する．
  //////////////////////////////////////////////////////////////////////
  int npo = ppo_num();
  vector<pair<int, int> > tmp_list(npo);
  for ( auto i: Range(npo) ) {
    const TpgNode* onode = ppo(i);
    // onode の TFI のノード数を計算する．
    vector<bool> mark(nn, false);
    int n = tfimark(onode, mark);
    tmp_list[i] = make_pair(n, i);
  }

  // TFI のサイズの昇順にソートする．
  sort(tmp_list.begin(), tmp_list.end(), Lt());
  // tmp_list の順に mPPOArray2 にセットする．
  for ( auto i: Range(npo) ) {
    int opos = tmp_list[i].second;
    TpgNode* onode = ppo_array[opos];
    mPPOArray2[i] = onode;
    onode->set_output_id2(i);
  }

  // immediate dominator を求める．
  for ( auto i: Range(node_num()) ) {
    TpgNode* node = tmp_node_list[node_num() - i - 1];
    const TpgNode* imm_dom = nullptr;
    if ( !node->is_ppo() ) {
      int nfo = node->fanout_num();
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
    TpgFFR* ffr = &mFfrArray[i];
    set_ffr(node, ffr);
  }


  //////////////////////////////////////////////////////////////////////
  // MFFC の情報を作る．
  //////////////////////////////////////////////////////////////////////
  SizeType mffc_num = mffc_root_list.size();
  mMffcArray.clear();
  mMffcArray.resize(mffc_num);
  for ( SizeType i: Range(mffc_num) ) {
    auto node = mffc_root_list[i];
    TpgMFFC* mffc = &mMffcArray[i];
    set_mffc(node, mffc);
  }
}

// @brief サイズを設定する．
void
TpgNetworkImpl::set_size(
  int input_num,
  int output_num,
  int dff_num,
  int node_num
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

  int nppi = mInputNum + dff_num;
  mPPIArray.clear();
  mPPIArray.resize(nppi);

  int nppo = mOutputNum + dff_num;
  mPPOArray.clear();
  mPPOArray.resize(nppo);
  mPPOArray2.clear();
  mPPOArray2.resize(nppo);
}

// @brief 代表故障を設定する．
// @param[in] node 対象のノード
int
TpgNetworkImpl::set_rep_faults(const TpgNode* node)
{
  vector<const TpgFault*> fault_list;

  if ( node->fanout_num() == 1 ) {
    const TpgNode* onode = node->fanout_list()[0];
    // ファンアウト先が一つならばそのファンイン
    // ブランチの故障と出力の故障は等価
    int ipos = 0;
    for ( auto inode: onode->fanin_list() ) {
      if ( inode == node ) {
	break;
      }
      ++ ipos;
    }
    ASSERT_COND( ipos < onode->fanin_num() );

    TpgFaultBase* rep0 = _node_input_fault(onode->id(), 0, ipos);
    TpgFaultBase* of0 = _node_output_fault(node->id(), 0);
    if ( of0 != nullptr ) {
      of0->set_rep(rep0);
    }

    TpgFaultBase* rep1 = _node_input_fault(onode->id(), 1, ipos);
    TpgFaultBase* of1 = _node_output_fault(node->id(), 1);
    if ( of1 != nullptr ){
      of1->set_rep(rep1);
    }
  }

  if ( !node->is_ppo() ) {
    // of0, of1 の代表故障が設定されていない場合には自分自身を代表故障とする．
    TpgFaultBase* of0 = _node_output_fault(node->id(), 0);
    if ( of0 != nullptr ) {
      const TpgFault* rep0 = of0->rep_fault();
      if ( rep0 == nullptr ) {
	of0->set_rep(of0);
	fault_list.push_back(of0);
      }
      else {
	of0->set_rep(rep0->rep_fault());
      }
    }

    TpgFaultBase* of1 = _node_output_fault(node->id(), 1);
    if ( of1 != nullptr ) {
      const TpgFault* rep1 = of1->rep_fault();
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
    TpgFaultBase* if0 = _node_input_fault(node->id(), 0, i);
    if ( if0 != nullptr ) {
      const TpgFault* rep0 = if0->rep_fault();
      if ( rep0 == nullptr ) {
	if0->set_rep(if0);
	fault_list.push_back(if0);
      }
      else {
	if0->set_rep(rep0->rep_fault());
      }
    }

    TpgFaultBase* if1 = _node_input_fault(node->id(), 1, i);
    if ( if1 != nullptr ) {
      const TpgFault* rep1 = if1->rep_fault();
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
// @param[in] root FFR の根のノード
// @param[in] ffr 対象の FFR
void
TpgNetworkImpl::set_ffr(
  const TpgNode* root,
  TpgFFR* ffr
)
{
  // root を根とするFFRの故障リストを求める．
  vector<const TpgFault*> fault_list;

  // root を根とするFFRの入力のリスト
  vector<const TpgNode*> input_list;
  // input_list の重複チェック用のハッシュ表
  unordered_set<int> input_hash;

  // DFS を行うためのスタック
  vector<const TpgNode*> node_stack;
  node_stack.push_back(root);
  while ( !node_stack.empty() ) {
    const TpgNode* node = node_stack.back();
    node_stack.pop_back();

    mAuxInfoArray[node->id()].add_to_fault_list(fault_list);

    for ( auto inode: node->fanin_list() ) {
      if ( inode->ffr_root() == inode || inode->is_ppi() ) {
	// inode は他の FFR の根
	if ( input_hash.count(inode->id()) == 0 ) {
	  input_hash.emplace(inode->id());
	  input_list.push_back(inode);
	}
      }
      else {
	node_stack.push_back(inode);
      }
    }
  }

  mAuxInfoArray[root->id()].set_ffr(ffr);

  ffr->set(root, input_list, fault_list);
}

// @brief MFFC の情報を設定する．
// @param[in] root MFFCの根のノード
// @param[in] mffc 対象のMFFC
void
TpgNetworkImpl::set_mffc(const TpgNode* root,
			 TpgMFFC* mffc)
{
  // root を根とする MFFC の情報を得る．
  vector<bool> mark(node_num());
  vector<const TpgNode*> node_list;
  vector<const TpgFFR*> ffr_list;
  vector<const TpgFault*> fault_list;

  node_list.push_back(root);
  mark[root->id()] = true;
  while ( !node_list.empty() ) {
    const TpgNode* node = node_list.back();
    node_list.pop_back();

    if ( node->ffr_root() == node ) {
      ffr_list.push_back(mAuxInfoArray[node->id()].ffr());
    }

    mAuxInfoArray[node->id()].add_to_fault_list(fault_list);

    for ( auto inode: node->fanin_list() ) {
      if ( !mark[inode->id()] &&
	   inode->imm_dom() != nullptr ) {
	mark[inode->id()] = true;
	node_list.push_back(inode);
      }
    }
  }

  mAuxInfoArray[root->id()].set_mffc(mffc);

  mffc->set(root, ffr_list, fault_list);
}

END_NAMESPACE_DRUID
