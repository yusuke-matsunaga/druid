
/// @File TpgNetworkImpl.cc
/// @brief TpgNetworkImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2019, 2022, 2024 Yusuke Matsunaga
/// All rights reserved.

#include "TpgNetworkImpl.h"
#include "TpgNode.h"
#include "TpgNodeSet.h"
#include "TpgFault.h"
#include "TpgMFFC.h"
#include "TpgFFR.h"
#include "FaultType.h"
#include "Fval2.h"
#include "ym/Range.h"

#define DFS_PRE 0
#define DFS_POST 0

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

/// @brief デストラクタ
TpgNetworkImpl::~TpgNetworkImpl()
{
  clear();
}

// @brief ステムの故障を得る.
const TpgFault*
TpgNetworkImpl::find_fault(
  const TpgGate* gate,
  Fval2 fval
) const
{
  if ( fault_type() == FaultType::GateExhaustive ) {
    // タイプ違い
    return nullptr;
  }
  SizeType key = gen_key(gate, fval);
  return _find_fault(key);
}

// @brief ブランチの故障を得る．
const TpgFault*
TpgNetworkImpl::find_fault(
  const TpgGate* gate,
  SizeType ipos,
  Fval2 fval
) const
{
  if ( fault_type() == FaultType::GateExhaustive ) {
    // タイプ違い
    return nullptr;
  }
  SizeType key = gen_key(gate, ipos, fval);
  return _find_fault(key);
}

// @brief 網羅故障を得る.
const TpgFault*
TpgNetworkImpl::find_fault(
  const TpgGate* gate,
  const vector<bool>& ivals
) const
{
  if ( fault_type() != FaultType::GateExhaustive ) {
    // タイプ違い
    return nullptr;
  }
  SizeType key = gen_key(gate, ivals);
  return _find_fault(key);
}

// @brief 内容をクリアする．
void
TpgNetworkImpl::clear()
{
  for ( auto node: mNodeArray ) {
    delete node;
  }
  for ( auto mffc: mMFFCList ) {
    delete mffc;
  }
  for ( auto ffr: mFFRList ) {
    delete ffr;
  }
  for ( auto gate: mGateList ) {
    delete gate;
  }
}

// @brief サイズを設定する．
SizeType
TpgNetworkImpl::set_size(
  SizeType input_num,
  SizeType output_num,
  SizeType dff_num,
  SizeType gate_num,
  SizeType extra_node_num
)
{
  mInputNum = input_num;
  mOutputNum = output_num;
  mDffInputList.clear();
  mDffInputList.resize(dff_num);
  mDffOutputList.clear();
  mDffOutputList.resize(dff_num);

  SizeType node_num = input_num + output_num + dff_num * 2 + gate_num + extra_node_num;

  mNodeArray.clear();
  mNodeArray.reserve(node_num);
  mGateList.clear();
  mGateList.reserve(gate_num);

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
  const TpgConnectionList& connection_list
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
  for ( SizeType i = 0; i < dff_num(); ++ i ) {
    auto input = mDffInputList[i];
    auto output = mDffOutputList[i];
    input->set_alt_node(output);
    output->set_alt_node(input);
  }

  //////////////////////////////////////////////////////////////////////
  // PPO に到達可能でないノードがないか調べる．
  //////////////////////////////////////////////////////////////////////
  auto tfi_list = TpgNodeSet::get_tfi_list(node_num(), ppo_list());
  if ( tfi_list.size() < node_num() ) {
    cerr << "warning: some nodes cannot reach to the outputs" << endl;
    // テスト不能故障になるだけ
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
    SizeType n = 0;
    TpgNodeSet::dfs(node_num(), {onode},
		    [&](const TpgNode*) { ++ n; },
		    [](const TpgNode*) {});
    tmp_list[i] = make_pair(n, i);
  }

  // TFI のサイズの昇順にソートする．
  sort(tmp_list.begin(), tmp_list.end(),
       [](const pair<SizeType, SizeType>& a,
	  const pair<SizeType, SizeType>& b)->bool
       {
	 return a.first < b.first;
       });

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
  mFFRList.clear();
  mFFRList.reserve(ffr_num);
  // ノード番号をキーにしてFFR番号を格納する辞書
  // FFRの根のノードだけ設定する．
  unordered_map<SizeType, const TpgFFR*> ffr_map;
  for ( auto node: ffr_root_list ) {
    auto ffr = new_ffr(node);
    ffr_map.emplace(node->id(), ffr);
  }

  //////////////////////////////////////////////////////////////////////
  // MFFC の情報を作る．
  //////////////////////////////////////////////////////////////////////
  SizeType mffc_num = mffc_root_list.size();
  mMFFCList.clear();
  mMFFCList.reserve(mffc_num);
  for ( auto node: mffc_root_list ) {
    new_mffc(node, ffr_map);
  }

  //////////////////////////////////////////////////////////////////////
  // 故障を作る．
  //////////////////////////////////////////////////////////////////////
  // 各ノードの出力の故障を記録する配列
  // 場合によっては対応する故障がないノードもある．
  vector<SizeType> fault_map(node_num() * 2, static_cast<SizeType>(-1));
  for ( auto gate: gate_list() ) {
    gen_gate_faults(gate, fault_map);
  }

  //////////////////////////////////////////////////////////////////////
  // 代表故障を求める．
  //////////////////////////////////////////////////////////////////////
  SizeType NF = mFaultArray.size();
  vector<SizeType> rep_map(NF, static_cast<SizeType>(-1));
  // この処理は入力からのトポロジカル順で行う必要がある．
  // gate_list() はトポロジカル順になっているはず．
  for ( auto gate: gate_list() ) {
    set_rep_fault(gate, fault_map, rep_map);
  }
  // rep_map の情報を元に代表故障を求める．
  // この処理は出力側からのトポロジカル順だと都合がよい．
  for ( SizeType i = 0; i < NF; ++ i ) {
    SizeType fid = NF - i - 1;
    auto f =  mFaultArray[fid];
    auto rep_id = rep_map[fid];
    if ( rep_id != static_cast<SizeType>(-1) ) {
      auto rep_f = mFaultArray[rep_id];
      f->set_rep_fault(rep_f->rep_fault());
    }
    else {
      // それ以外は自身が代表故障
      f->set_rep_fault(f);
    }
  }

  // 求めた代表故障を記録する．
  mRepFaultList.clear();
  for ( auto f: mFaultArray ) {
    if ( f->rep_fault() == f ) {
      mRepFaultList.push_back(f);
    }
  }
}

// @brief FFR を作る．
const TpgFFR*
TpgNetworkImpl::new_ffr(
  const TpgNode* root
)
{
  vector<const TpgNode*> input_list;
  vector<const TpgNode*> node_list;

  // input_list の重複チェック用のハッシュ表のふりをした配列
  vector<bool> input_hash(node_num(), false);

  // DFS を行うためのスタック
  vector<const TpgNode*> node_stack;
  node_stack.push_back(root);
  node_list.push_back(root);
  while ( !node_stack.empty() ) {
    auto node = node_stack.back();
    node_stack.pop_back();
    for ( auto inode: node->fanin_list() ) {
      if ( inode->ffr_root() == inode ) {
	// inode は他の FFR の根
	if ( !input_hash[inode->id()] ) {
	  input_hash[inode->id()] = true;
	  input_list.push_back(inode);
	}
      }
      else {
	if ( !inode->is_ppi() ) {
	  node_stack.push_back(inode);
	}
	node_list.push_back(inode);
      }
    }
  }
  SizeType id = mFFRList.size();
  auto ffr = new TpgFFR{id, root, input_list, node_list};
  mFFRList.push_back(ffr);
  mFFRMap.emplace(root->id(), ffr);
  return ffr;
}

// @brief MFFC を作る．
void
TpgNetworkImpl::new_mffc(
  const TpgNode* root,
  const unordered_map<SizeType, const TpgFFR*>& ffr_map
)
{
  vector<const TpgFFR*> ffr_list;

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
      auto ffr = ffr_map.at(node->id());
      ffr_list.push_back(ffr);
    }

    for ( auto inode: node->fanin_list() ) {
      if ( !mark[inode->id()] &&
	   inode->imm_dom() != nullptr ) {
	mark[inode->id()] = true;
	node_list.push_back(inode);
      }
    }
  }
  SizeType id = mMFFCList.size();
  auto mffc = new TpgMFFC{id, root, ffr_list};
  mMFFCList.push_back(mffc);
  mMFFCMap.emplace(root->id(), mffc);
}

// @brief ゲートに関連した故障を作る．
void
TpgNetworkImpl::gen_gate_faults(
  const TpgGate* gate,
  vector<SizeType>& fault_map
)
{
  if ( gate->is_ppi() ) {
    // 入力に関しては網羅故障はないので縮退故障で考える．
    gen_stem_fault(gate, fault_map);
  }
  else if ( gate->is_ppo() ) {
    // 出力
    gen_branch_fault(gate);
  }
  else {
    // ゲート
    switch ( fault_type() ) {
    case FaultType::StuckAt:
    case FaultType::TransitionDelay:
      // ステムの故障
      gen_stem_fault(gate, fault_map);
      // ブランチの故障
      gen_branch_fault(gate);
      break;

    case FaultType::GateExhaustive:
      // ゲート網羅故障
      gen_ex_fault(gate);
      break;

    default:
      cerr << "wrong fault type" << endl;
      ASSERT_NOT_REACHED;
      break;
    }
  }
}

// @brief ステムの故障を作る．
void
TpgNetworkImpl::gen_stem_fault(
  const TpgGate* gate,
  vector<SizeType>& fault_map
)
{
  auto ftype = fault_type();
  if ( ftype == FaultType::GateExhaustive ) {
    ftype = FaultType::StuckAt;
  }
  for ( auto fval: {Fval2::zero, Fval2::one} ) {
    SizeType fid = mFaultArray.size();
    auto f = TpgFault::new_fault(fid, gate, fval, ftype);
    reg_fault(f);
    auto node = gate->output_node();
    auto b = fval == Fval2::zero ? 0 : 1;
    fault_map[node->id() * 2 + b] = f->id();
  }
}

// @brief ブランチの故障を作る．
void
TpgNetworkImpl::gen_branch_fault(
  const TpgGate* gate
)
{
  SizeType ni = gate->input_num();
  for ( SizeType ipos = 0; ipos < ni; ++ ipos ) {
    for ( auto fval: {Fval2::zero, Fval2::one} ) {
      SizeType fid = mFaultArray.size();
      auto f = TpgFault::new_fault(fid, gate, ipos, fval, fault_type());
      reg_fault(f);
    }
  }
}

// @brief ゲート網羅故障を作る．
void
TpgNetworkImpl::gen_ex_fault(
  const TpgGate* gate
)
{
  SizeType ni = gate->input_num();
  SizeType ni_exp = 1 << ni;
  vector<bool> ivals(ni);
  for ( SizeType b = 0; b < ni_exp; ++ b ) {
    for ( SizeType i = 0; i < ni; ++ i ) {
      if ( b & (1 << i) ) {
	ivals[i] = true;
      }
    }
    SizeType fid = mFaultArray.size();
    auto f = TpgFault::new_fault(fid, gate, ivals);
    reg_fault(f);
  }
}

// @brief 故障を登録する．
void
TpgNetworkImpl::reg_fault(
  TpgFault* fault
)
{
  mFaultArray.push_back(fault);
  SizeType key;
  switch ( fault->fault_type() ) {
  case FaultType::StuckAt:
  case FaultType::TransitionDelay:
    if ( fault->is_stem() ) {
      key = gen_key(fault->gate(), fault->fval());
    }
    else {
      key = gen_key(fault->gate(), fault->branch_pos(), fault->fval());
    }
    break;

  case FaultType::GateExhaustive:
    key = gen_key(fault->gate(), fault->input_vals());
    break;

  default:
    ASSERT_NOT_REACHED;
    break;
  }
  mFaultDict.emplace(key, fault);
}

// @brief ステムの故障用のキーを作る．
SizeType
TpgNetworkImpl::gen_key(
  const TpgGate* gate,
  Fval2 fval
) const
{
  SizeType NG = mGateList.size();
  SizeType key = fval == Fval2::zero ? 0 : 1;
  return key * NG + gate->id();
}

// @brief ブランチの故障用のキーを作る．
SizeType
TpgNetworkImpl::gen_key(
  const TpgGate* gate,
  SizeType ipos,
  Fval2 fval
) const
{
  SizeType NG = mGateList.size();
  SizeType key = fval == Fval2::zero ? 0 : 1;
  key += (ipos + 1) * 2;
  return key * NG + gate->id();
}

// @brief ゲート網羅故障用のキーを作る．
SizeType
TpgNetworkImpl::gen_key(
  const TpgGate* gate,
  const vector<bool>& ivals
) const
{
  SizeType NG = mGateList.size();
  SizeType NI = gate->input_num();
  SizeType key = (NI + 1) * 2;
  for ( SizeType i = 0; i < NI; ++ i ) {
    if ( ivals[i] ) {
      key += (1 << i);
    }
  }
  return key * NG + gate->id();
}

// @brief 代表故障を求める．
void
TpgNetworkImpl::set_rep_fault(
  const TpgGate* gate,
  const vector<SizeType>& fault_map,
  vector<SizeType>& rep_map
)
{
  // 出力の故障
  auto f0 = find_fault(gate, Fval2::zero);
  auto f1 = find_fault(gate, Fval2::one);

  SizeType NI = gate->input_num();
  for ( SizeType i = 0; i < NI; ++ i ) {
    for ( SizeType v = 0; v < 2; ++ v ) {
      auto val = v == 0 ? Val3::_0 : Val3::_1;
      auto fval = v == 0 ? Fval2::zero : Fval2::one;
      auto i_fault = find_fault(gate, i, fval);

      // 1. 入力の故障が出力の故障と等価か調べる．
      auto oval = gate->cval(i, val);
      const TpgFault* o_fault = nullptr;
      switch ( oval ) {
      case Val3::_0:
	o_fault = f0;
	break;
      case Val3::_1:
	o_fault = f1;
	break;
      case Val3::_X:
	break;
      }
      if ( o_fault != nullptr ) {
	rep_map[i_fault->id()] = o_fault->id();
      }

      // 2. 入力のファンアウト数が1の時，その入力のステムの故障と等価となる．
      auto inode = gate->input_node(i);
      if ( inode->fanout_num() == 1 ) {
	auto s_id = fault_map[inode->id() * 2 + v];
	rep_map[s_id] = i_fault->id();
      }
    }
  }
}

END_NAMESPACE_DRUID
