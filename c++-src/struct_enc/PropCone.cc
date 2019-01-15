
/// @file PropCone.cc
/// @brief PropCone の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2012-2014, 2017 Yusuke Matsunaga
/// All rights reserved.


#include "PropCone.h"
#include "StructEnc.h"
#include "TpgNode.h"
#include "TpgFault.h"
#include "NodeValList.h"
#include "GateEnc.h"

BEGIN_NAMESPACE_DRUID

extern
NodeValList
extract(const TpgNode*,
	const VidMap&,
	const VidMap&,
	const vector<SatBool3>&);

END_NAMESPACE_DRUID

BEGIN_NAMESPACE_DRUID_STRUCTENC

BEGIN_NONAMESPACE

bool debug = false;

// TpgNode::output_id2() の値に基づく比較を行う．
struct Lt
{
  bool
  operator()(const TpgNode* left,
	     const TpgNode* right)
  {
    return left->output_id2() < right->output_id2();
  }
};

END_NONAMESPACE

// @brief コンストラクタ
// @param[in] struct_sat StructEnc ソルバ
// @param[in] root_node FFRの根のノード
// @param[in] block_node ブロックノード
// @param[in] detect 故障を検出する時に true にするフラグ
//
// ブロックノードより先のノードは含めない．
// 通常 block_node は nullptr か root_node の dominator
// となっているはず．
PropCone::PropCone(StructEnc& struct_sat,
		   const TpgNode* root_node,
		   const TpgNode* block_node,
		   bool detect) :
  mStructEnc(struct_sat),
  mDetect(detect),
  mMaxNodeId(struct_sat.max_node_id()),
  mMarkArray(max_id()),
  mFvarMap(max_id()),
  mDvarMap(max_id())
{
  if ( block_node != nullptr ) {
    set_end_mark(block_node);
  }

  mNodeList.reserve(max_id());
  mark_tfo(root_node);
}

// @brief デストラクタ
PropCone::~PropCone()
{
}

// @brief 指定されたノードの TFO に印をつける．
// @param[in] node 起点となるノード
void
PropCone::mark_tfo(const TpgNode* node)
{
  set_tfo_mark(node);

  for (int rpos = 0; rpos < mNodeList.size(); ++ rpos) {
    const TpgNode* node = mNodeList[rpos];
    if ( end_mark(node) ) {
      // ここで止まる．
      continue;
    }
    int nfo = node->fanout_num();
    for (auto fonode: node->fanout_list() ) {
      set_tfo_mark(fonode);
    }
  }

  // 出力のリストを output_id2() の昇順に整列しておく．
  sort(mOutputList.begin(), mOutputList.end(), Lt());
}

// @brief 関係するノードの変数を作る．
void
PropCone::make_vars()
{
  // TFO のノードに変数を割り当てる．
  for (int i = 0; i < mNodeList.size(); ++ i) {
    const TpgNode* node = mNodeList[i];
    SatVarId fvar = solver().new_variable();
    set_fvar(node, fvar);
    if ( debug ) {
      cout << "fvar(Node#" << node->id() << ") = " << fvar << endl;
    }
    if ( mDetect ) {
      SatVarId dvar = solver().new_variable();
      set_dvar(node, dvar);
    }
#if 0
    // ファンインのノードうち TFO に含まれないノードの fvar を gvar にする．
    int ni = node->fanin_num();
    for (int i = 0; i < ni; ++ i) {
      const TpgNode* inode = node->fanin(i);
      if ( !tfo_mark(inode) ) {
	set_fvar(inode, gvar(inode));
	if ( debug ) {
	  cout << "fvar(Node#" << inode->id() << ") = gvar" << endl;
	}
      }
    }
#endif
  }

  // 暫定的
  // TFO の TFI のノードの fvar を gvar と同じにする．
  vector<const TpgNode*> tmp_list;
  vector<bool> tfi_mark(max_id(), false);
  for (int i = 0; i < mNodeList.size(); ++ i) {
    const TpgNode* node = mNodeList[i];
    for ( auto inode: node->fanin_list() ) {
      if ( !tfo_mark(inode) && !tfi_mark[inode->id()] ) {
	tfi_mark[inode->id()] = true;
	tmp_list.push_back(inode);
      }
    }
  }
  for (int rpos = 0; rpos < tmp_list.size(); ++ rpos) {
    const TpgNode* node = tmp_list[rpos];
    set_fvar(node, gvar(node));
    for ( auto inode: node->fanin_list() ) {
      if ( !tfi_mark[inode->id()] ) {
	tfi_mark[inode->id()] = true;
	tmp_list.push_back(inode);
      }
    }
  }
}

// @brief 関係するノードの入出力の関係を表すCNFを作る．
void
PropCone::make_cnf()
{
  GateEnc gate_enc(solver(), fvar_map());
  for (int i = 0; i < mNodeList.size(); ++ i) {
    const TpgNode* node = mNodeList[i];
    if ( i > 0 ) {
      // 故障回路のゲートの入出力関係を表すCNFを作る．
      gate_enc.make_cnf(node);
    }

    if ( mDetect ) {
      // D-Chain 制約を作る．
      make_dchain_cnf(node);
    }
  }

  // 外部出力へ故障の影響が伝搬する条件を作る．
  int no = mOutputList.size();
  vector<SatLiteral> odiff(no);
  for (int i = 0; i < no; ++ i) {
    const TpgNode* node = mOutputList[i];
    SatLiteral dlit(dvar(node));
    odiff[i] = dlit;
  }
  solver().add_clause(odiff);

  const TpgNode* root = mNodeList[0];
  if ( !root->is_ppo() ) {
    // root の dlit が1でなければならない．
    solver().add_clause(SatLiteral(dvar(root)));
  }
}

// @brief 故障検出に必要な割り当てを求める．
// @param[in] model SAT のモデル
// @param[in] root 起点のノード
// @param[out] 値の割り当て結果を入れるリスト
NodeValList
PropCone::extract(const vector<SatBool3>& model,
		  const TpgNode* root)
{
  return nsYm::nsDruid::extract(root, gvar_map(), fvar_map(), model);
}

// @brief node に関する故障伝搬条件を作る．
void
PropCone::make_dchain_cnf(const TpgNode* node)
{
  SatLiteral glit(gvar(node), false);
  SatLiteral flit(fvar(node), false);
  SatLiteral dlit(dvar(node), false);

  // dlit -> XOR(glit, flit) を追加する．
  // 要するに dlit が 1 の時，正常回路と故障回路で異なっていなければならない．
  solver().add_clause(~glit, ~flit, ~dlit);
  solver().add_clause( glit,  flit, ~dlit);

  if ( end_mark(node) ) {
    // 出力ノードの場合，XOR(glit, flit) -> dlit となる．
    solver().add_clause(~glit,  flit, dlit);
    solver().add_clause( glit, ~flit, dlit);
  }
  else {
    // dlit が 1 の時，ファンアウトの dlit が最低1つは 1 でなければならない．
    int nfo = node->fanout_num();
    vector<SatLiteral> tmp_lits;
    tmp_lits.reserve(nfo + 1);
    tmp_lits.push_back(~dlit);
    for ( auto onode: node->fanout_list() ) {
      SatLiteral odlit(dvar(onode), false);
      tmp_lits.push_back(odlit);
    }
    solver().add_clause(tmp_lits);

    // immediate dominator がある場合，immediate dominator の dlit も 1 でなければならない．
    const TpgNode* idom = node->imm_dom();
    if ( idom != nullptr ) {
      SatLiteral odlit(dvar(idom));
      solver().add_clause(~dlit, odlit);
    }
  }
}

END_NAMESPACE_DRUID_STRUCTENC
