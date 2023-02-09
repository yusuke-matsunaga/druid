
/// @File TpgNetworkImpl.cc
/// @brief TpgNetworkImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2019, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "TpgNetworkImpl.h"
#include "TpgNode.h"
#include "TpgFaultBase.h"
#include "TpgDFF.h"
#include "TpgMFFC.h"
#include "TpgFFR.h"

#include "AuxNodeInfo.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

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
  const TpgNetworkImpl* network
)
{
  // fanin/fanout の sanity check
  bool error = false;

  for ( auto node: network->node_list() ) {
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

// @brief サイズを設定する．
SizeType
TpgNetworkImpl::set_size(
  SizeType input_num,
  SizeType output_num,
  SizeType dff_num,
  SizeType gate_num,
  SizeType dff_control_num
)
{
  mInputNum = input_num;
  mOutputNum = output_num;
  mDFFArray.clear();
  mDFFArray.resize(dff_num);
  for ( auto i: Range(dff_num) ) {
    mDFFArray[i].mId = i;
  }

  SizeType node_num = input_num + output_num + dff_num * 2 + gate_num + dff_control_num;

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

  return node_num;
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
    check_network_connection(this);
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
  mFFRArray.clear();
  mFFRArray.resize(ffr_num);
  for ( SizeType i: Range(ffr_num) ) {
    auto node = ffr_root_list[i];
    set_ffr(i, node);
  }

  //////////////////////////////////////////////////////////////////////
  // MFFC の情報を作る．
  //////////////////////////////////////////////////////////////////////
  SizeType mffc_num = mffc_root_list.size();
  mMFFCArray.clear();
  mMFFCArray.resize(mffc_num);
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
  auto& ffr = mFFRArray[id];

  ffr.mRoot = root;

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
  auto& mffc = mMFFCArray[id];

  mffc.mRoot = root;

  // root を根とする MFFC の情報を得る．
  vector<bool> mark(node_num());
  vector<const TpgNode*> node_list;

  node_list.push_back(root);
  mark[root->id()] = true;
  while ( !node_list.empty() ) {
    auto node = node_list.back();
    node_list.pop_back();

    if ( node->ffr_root() == node ) {
      auto ffr_id = mAuxInfoArray[node->id()].ffr();
      mffc.mFFRList.push_back(TpgFFR{this, ffr_id});
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
}

END_NAMESPACE_DRUID
