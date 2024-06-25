
/// @file MFFCEnc.cc
/// @brief MFFCEnc の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "MFFCEnc.h"
#include "TpgNetwork.h"
#include "TpgMFFC.h"
#include "TpgFault.h"
#include "GateEnc.h"

//#define DEBUG_DTPG
#define DEBUG_OUT cout


BEGIN_NONAMESPACE
#ifdef DEBUG_DTPG
int debug_mffc = 1;
#else
const int debug_mffc = 0;
#endif
END_NONAMESPACE

BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

void
dfs(
  const TpgNode* node,
  unordered_set<SizeType>& mark,
  vector<const TpgNode*>& node_list
)
{
  for ( auto onode: node->fanout_list() ) {
    if ( mark.count(onode->id()) > 0 ) {
      continue;
    }
    mark.emplace(onode->id());
    node_list.push_back(onode);
    dfs(onode, mark, node_list);
  }
}

END_NONAMESPACE

// @brief コンストラクタ
MFFCEnc::MFFCEnc(
  BaseEnc& base_enc,
  const TpgMFFC* mffc
) : SubEnc{base_enc},
    mMFFC{mffc},
    mFFRInfoArray(mMFFC->ffr_num()),
    mFvarMap{base_enc.network().node_num()}
{
  // MFFC 内の FFR に対してローカルな番号を割り当てる．
  SizeType ffr_id = 0;
  for ( auto ffr: mMFFC->ffr_list() ) {
    mFFRIdMap.emplace(ffr->id(), ffr_id);
    auto& info = mFFRInfoArray[ffr_id];
    info.mFFR = ffr;
    auto root = ffr->root();
    mRootIdMap.emplace(root->id(), ffr_id);
    ++ ffr_id;
  }

  // mRootArray[] のノードと MFFC の出力の間にあるノードを
  // mNodeList に入れる．
  // 最も入力よりにある mRootArray[] のノード自身は含まない．
  unordered_set<SizeType> mark;
  // mFFC->root() を番兵とする．
  auto root = mMFFC->root();
  mark.emplace(root->id());
  mNodeList.push_back(root);
  for ( auto ffr: mMFFC->ffr_list() ) {
    auto node = ffr->root();
    if ( node != root ) {
      dfs(node, mark, mNodeList);
    }
  }
}

// @brief 故障検出用の制御変数の割り当てを返す．
vector<SatLiteral>
MFFCEnc::cvar_assumptions(
  const TpgFault* fault
)
{
  // fault に該当する FFR の cvar のみ true にする．
  auto node = fault->ffr_root();
  auto ffr_id = mRootIdMap.at(node->id());

  SizeType nffr = mFFRInfoArray.size();
  vector<SatLiteral> assumptions(nffr);
  for ( SizeType i = 0; i < nffr; ++ i ) {
    auto clit = mFFRInfoArray[i].mCvar;
    if ( i == ffr_id ) {
      assumptions[i] = clit;
    }
    else {
      assumptions[i] = ~clit;
    }
  }
  return assumptions;
}

// @brief 直前の check() が成功したときの十分条件を求める．
NodeTimeValList
MFFCEnc::extract_sufficient_condition(
  const TpgFault* fault
)
{
  auto start = mMFFC->root();
  auto end = fault->ffr_root();
  // start から end までの故障伝搬条件を求める．
  // ここでは単純に gvar(node) == fvar(node) のノード
  // の場合に値を記憶する．
  // これだと無駄な値割り当てを含む可能性がある．
  NodeTimeValList assign_list;
  unordered_set<SizeType> mark;
  ex_sub(start, end, assign_list, mark);
  return assign_list;
}

// @brief extract_sufficient_condition() の下請け関数
void
MFFCEnc::ex_sub(
  const TpgNode* node,
  const TpgNode* end_node,
  NodeTimeValList& assign_list,
  unordered_set<SizeType>& mark
)
{
  if ( node == end_node ) {
    return;
  }
  if ( mark.count(node->id()) > 0 ) {
    return;
  }
  mark.emplace(node->id());

  // node は sensitized node のはず．
  for ( auto inode: node->fanin_list() ) {
    if ( is_in_fcone(inode) ) {
      ex_sub(inode, end_node, assign_list, mark);
    }
    else {
      auto& model = solver().model();
      auto glit = gvar(node);
      auto val = model[glit];
      bool bval = val == SatBool3::True ? true : false;
      assign_list.add(inode, 1, bval);
    }
  }
}

// @brief 故障伝搬ノードの時 true を返す．
bool
MFFCEnc::is_in_fcone(
  const TpgNode* node
)
{
  auto glit = gvar(node);
  auto flit = fvar(node);
  return glit != flit;
}

// @brief 必要な変数を割り当て CNF 式を作る．
void
MFFCEnc::make_cnf()
{
  mPropVar = solver().new_variable();
  for ( auto& info: mFFRInfoArray ) {
    info.mCvar = solver().new_variable(true);

    if ( debug_mffc ) {
      DEBUG_OUT << "cvar(FFR#" << info.mFFR->id() << ") = " << info.mCvar << endl;
    }
  }

  // まず，各ノードのファンインの変数を gvar で初期化しておく．
  for ( auto node: mNodeList ) {
    for ( auto inode: node->fanin_list() ) {
      auto glit = base_enc().gvar(inode);
      mFvarMap.set_vid(inode, glit);
    }
  }
  // node に新しい変数を割り当てる．
  for ( auto node: mNodeList ) {
    auto flit = solver().new_variable(true);
    mFvarMap.set_vid(node, flit);

    if ( debug_mffc ) {
      DEBUG_OUT << "fvar(Node#" << node->id() << ") = " << flit << endl;
    }
  }

  // 最も入力よりにある FFR の根のノードの場合，
  // 正常回路の値と制御変数の XOR を故障値とする．
  for ( auto& info: mFFRInfoArray ) {
    auto node = info.mFFR->root();
    if ( fvar(node) == gvar(node) ) {
      // この FFR は最も入力よりにある．
      auto flit = solver().new_variable(true);
      mFvarMap.set_vid(node, flit);
      auto glit = gvar(node);
      inject_fault(info, glit);
    }
  }

  // 故障値の回路を表すCNFを作る．
  GateEnc fval_enc{solver(), mFvarMap};
  for ( auto node: mNodeList ) {
    if ( mRootIdMap.count(node->id()) > 0 ) {
      // FFR の根のノードの場合，cvar との XOR ゲートを挿入する．
      SizeType ffr_id = mRootIdMap.at(node->id());
      auto& info = mFFRInfoArray[ffr_id];
      // 実際のゲートの出力と ovar の間に XOR ゲートを挿入する．
      // XORの一方の入力は info.mCvar
      auto olit = solver().new_variable();
      // olit は fvar(node) ではない！
      fval_enc.make_cnf(node, olit);
      inject_fault(info, olit);
    }
    else {
      fval_enc.make_cnf(node);
    }

    if ( debug_mffc ) {
      DEBUG_OUT << "Node#" << node->id() << ": fvar = "
		<< node->gate_type()
		<< "(";
      for ( auto inode: node->fanin_list() ) {
	DEBUG_OUT << " " << fvar(inode);
      }
      DEBUG_OUT << ")" << endl;
    }
  }

  // MFFC の出力の gvar と fvar の値が異なる時 mPropVar が true となる．
  {
    auto root = mMFFC->root();
    auto glit = gvar(root);
    auto flit = fvar(root);
    solver().add_xorgate(mPropVar, glit, flit);
  }
}

// @brief 関連するノードのリストを返す．
const vector<const TpgNode*>&
MFFCEnc::node_list() const
{
  return mNodeList;
}

// @brief 故障挿入回路のCNFを作る．
void
MFFCEnc::inject_fault(
  const FFRInfo& info,
  SatLiteral ovar
)
{
  auto lit1 = ovar;
  auto lit2 = info.mCvar;
  auto node = info.mFFR->root();
  auto olit = fvar(node);

  solver().add_xorgate(olit, lit1, lit2);

  if ( debug_mffc ) {
    DEBUG_OUT << "inject fault: " << olit << " -> " << lit1
	      << " with cvar = " << lit2 << endl;
  }
}

END_NAMESPACE_DRUID
