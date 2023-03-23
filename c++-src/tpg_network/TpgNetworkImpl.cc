
/// @File TpgNetworkImpl.cc
/// @brief TpgNetworkImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2019, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "TpgNetworkImpl.h"
#include "TpgNode.h"
#include "TpgPPI.h"
#include "TpgPPO.h"
#include "TpgDffControl.h"
#include "TpgFault.h"
#include "TpgGateImpl.h"
#include "TpgMFFC.h"
#include "TpgFFR.h"
#include "GateType.h"
#include "Fval2.h"
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
  for ( auto gate: mGateArray ) {
    delete gate;
  }
}

BEGIN_NONAMESPACE

// @brief ノードの TFI にマークをつける．
void
mark_datapath(
  const TpgNode* node
)
{
  if ( node->is_datapath() ) {
    return;
  }
  const_cast<TpgNode*>(node)->set_datapath(true);

  for ( auto inode: node->fanin_list() ) {
    mark_datapath(inode);
  }
}

// @brief TFI のノード数を数える．
SizeType
count_tfi(
  const TpgNode* node,
  SizeType node_num
)
{
  // 再帰呼び出し用のスタック
  vector<const TpgNode*> stack;
  stack.reserve(node_num);
  // マーク
  vector<bool> mark(node_num, false);

  SizeType n = 0;
  stack.push_back(node);
  mark[node->id()] = true;

  while ( !stack.empty() ) {
    auto node = stack.back();
    stack.pop_back();
    ++ n;
    for ( auto inode: node->fanin_list() ) {
      if ( !mark[inode->id()] ) {
	stack.push_back(inode);
	mark[inode->id()] = true;
      }
    }
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
  mGateArray.clear();
  mGateArray.reserve(gate_num);

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
  // DFF の入力と出力を結びつける．
  //////////////////////////////////////////////////////////////////////
  for ( auto _dff: mDFFArray ) {
    auto input = reinterpret_cast<TpgDffInput*>(_dff.mInput);
    auto output = reinterpret_cast<TpgDffOutput*>(_dff.mOutput);
    input->set_alt_node(output);
    output->set_alt_node(input);
  }

  //////////////////////////////////////////////////////////////////////
  // データ系のノードに印をつける．
  //////////////////////////////////////////////////////////////////////
  for ( auto node: ppo_list() ) {
    mark_datapath(node);
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
    SizeType n = count_tfi(onode, node_num());
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
    if ( !node->is_datapath() ) {
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
  // ノード番号をキーにしてFFR番号を格納する辞書
  // FFRの根のノードだけ設定する．
  unordered_map<SizeType, SizeType> ffr_map;
  for ( SizeType i: Range(ffr_num) ) {
    auto node = ffr_root_list[i];
    set_ffr(i, node);
    ffr_map.emplace(node->id(), i);
  }

  //////////////////////////////////////////////////////////////////////
  // MFFC の情報を作る．
  //////////////////////////////////////////////////////////////////////
  SizeType mffc_num = mffc_root_list.size();
  mMFFCArray.clear();
  mMFFCArray.resize(mffc_num);
  for ( SizeType i: Range(mffc_num) ) {
    auto node = mffc_root_list[i];
    set_mffc(i, node, ffr_map);
  }
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

  // input_list の重複チェック用のハッシュ表のふりをした配列
  vector<bool> input_hash(node_num(), false);

  // DFS を行うためのスタック
  vector<const TpgNode*> node_stack;
  node_stack.push_back(root);
  ffr.mNodeList.push_back(root);
  while ( !node_stack.empty() ) {
    auto node = node_stack.back();
    node_stack.pop_back();
    for ( auto inode: node->fanin_list() ) {
      if ( inode->ffr_root() == inode || inode->is_ppi() ) {
	// inode は他の FFR の根
	if ( !input_hash[inode->id()] ) {
	  input_hash[inode->id()] = true;
	  ffr.mInputList.push_back(inode);
	}
      }
      else {
	node_stack.push_back(inode);
	ffr.mNodeList.push_back(inode);
      }
    }
  }
}

// @brief MFFC の情報を設定する．
void
TpgNetworkImpl::set_mffc(
  SizeType id,
  const TpgNode* root,
  const unordered_map<SizeType, SizeType>& ffr_map
)
{
  auto& mffc = mMFFCArray[id];

  mffc.mRoot = root;

  // root を根とする MFFC の情報を得る．
  vector<bool> mark(node_num(), false);
  vector<const TpgNode*> node_list;

  node_list.push_back(root);
  mark[root->id()] = true;
  while ( !node_list.empty() ) {
    auto node = node_list.back();
    node_list.pop_back();

    if ( node->ffr_root() == node ) {
      ASSERT_COND( ffr_map.count(node->id()) > 0 );
      auto ffr_id = ffr_map.at(node->id());
      mffc.mFFRList.push_back(TpgFFR{this, ffr_id});
    }

    for ( auto inode: node->fanin_list() ) {
      if ( !mark[inode->id()] &&
	   inode->imm_dom() != nullptr ) {
	mark[inode->id()] = true;
	node_list.push_back(inode);
      }
    }
  }
}


//////////////////////////////////////////////////////////////////////
// クラス TpgMFFC
//////////////////////////////////////////////////////////////////////

// @brief 根のノードを返す．
const TpgNode*
TpgMFFC::root() const
{
  ASSERT_COND( mNetwork != nullptr );

  auto& mffc = mNetwork->_mffc(mId);
  return mffc.root();
}

// @brief このMFFCに含まれるFFR番号のリストを返す．
const vector<TpgFFR>&
TpgMFFC::ffr_list() const
{
  ASSERT_COND( mNetwork != nullptr );

  auto& mffc = mNetwork->_mffc(mId);
  return mffc.ffr_list();
}


//////////////////////////////////////////////////////////////////////
// クラス TpgFFR
//////////////////////////////////////////////////////////////////////

// @brief 根のノードを返す．
const TpgNode*
TpgFFR::root() const
{
  ASSERT_COND( mNetwork != nullptr );

  auto& ffr = mNetwork->_ffr(mId);
  return ffr.root();
}

// @brief 葉(FFRの入力)のリストを返す．
const vector<const TpgNode*>&
TpgFFR::input_list() const
{
  ASSERT_COND( mNetwork != nullptr );

  auto& ffr = mNetwork->_ffr(mId);
  return ffr.input_list();
}

// @brief このFFRに含まれるノードのリストを返す．
const vector<const TpgNode*>&
TpgFFR::node_list() const
{
  ASSERT_COND( mNetwork != nullptr );

  auto& ffr = mNetwork->_ffr(mId);
  return ffr.node_list();
}


//////////////////////////////////////////////////////////////////////
// クラス TpgGate
//////////////////////////////////////////////////////////////////////

// @brief 名前を返す．
string
TpgGate::name() const
{
  ASSERT_COND( mNetwork != nullptr );

  auto gate = mNetwork->_gate(mId);
  return gate->name();
}

// @brief 出力に対応するノードを返す．
const TpgNode*
TpgGate::output_node() const
{
  ASSERT_COND( mNetwork != nullptr );

  auto gate = mNetwork->_gate(mId);
  return gate->output_node();
}

// @brief 入力数を返す．
SizeType
TpgGate::input_num() const
{
  ASSERT_COND( mNetwork != nullptr );

  auto gate = mNetwork->_gate(mId);
  return gate->input_num();
}

// @brief ブランチの情報を返す．
TpgGate::BranchInfo
TpgGate::branch_info(
  SizeType pos
) const
{
  ASSERT_COND( mNetwork != nullptr );

  auto gate = mNetwork->_gate(mId);
  return gate->branch_info(pos);
}

// @brief 代表故障かどうか調べる．
bool
TpgGate::is_rep(
  SizeType pos,
  Fval2 fval
) const
{
  ASSERT_COND( mNetwork != nullptr );

  auto gate = mNetwork->_gate(mId);
  // 具体的には pos の val が制御値でない時に
  // 代表故障となる．
  auto val = fval == Fval2::zero ? Val3::_0 : Val3::_1;
  return gate->gate_type()->cval(pos, val) == Val3::_X;
}


//////////////////////////////////////////////////////////////////////
// クラス TpgGate_Simple
//////////////////////////////////////////////////////////////////////

// @brief 出力に対応するノードを返す．
const TpgNode*
TpgGate_Simple::output_node() const
{
  return mNode;
}

// @brief 入力数を返す．
SizeType
TpgGate_Simple::input_num() const
{
  return mNode->fanin_num();
}

// @brief ブランチの情報を返す．
TpgGate::BranchInfo
TpgGate_Simple::branch_info(
  SizeType pos
) const
{
  ASSERT_COND( 0 <= pos && pos < input_num() );

  return BranchInfo{mNode, pos};
}


//////////////////////////////////////////////////////////////////////
// クラス TpgGate_Cplx
//////////////////////////////////////////////////////////////////////

// @brief 出力に対応するノードを返す．
const TpgNode*
TpgGate_Cplx::output_node() const
{
  return mOutputNode;
}

// @brief 入力数を返す．
SizeType
TpgGate_Cplx::input_num() const
{
  return mBranchInfoList.size();
}

// @brief ブランチの情報を返す．
TpgGate::BranchInfo
TpgGate_Cplx::branch_info(
  SizeType pos
) const
{
  ASSERT_COND( 0 <= pos && pos < input_num() );
  return mBranchInfoList[pos];
}

END_NAMESPACE_DRUID
