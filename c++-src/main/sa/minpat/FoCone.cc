
/// @file FoCone.cc
/// @brief FoCone の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2012-2014 Yusuke Matsunaga
/// All rights reserved.


#include "FoCone.h"
#include "StructSat.h"
#include "TpgNode.h"
#include "../../struct_enc/GateLitMap_vid.h"
#include "../../struct_enc/ValMap_model.h"
#include "Extractor.h"
#include "NodeValList.h"


BEGIN_NAMESPACE_DRUID_SA

BEGIN_NONAMESPACE

bool debug = false;

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
// @param[in] struct_sat StructSat ソルバ
// @param[in] fnode 故障位置のノード
// @param[in] bnode ブロックノード
// @param[in] detect 検出条件
//
// ブロックノードより先のノードは含めない．
// 通常 bnode は fnode の dominator
// となっているはず．
FoCone::FoCone(StructSat& struct_sat,
	       const TpgNode* fnode,
	       const TpgNode* bnode,
	       Val3 detect) :
  mStructSat(struct_sat),
  mMaxNodeId(struct_sat.max_node_id()),
  mMarkArray(max_id()),
  mFvarMap(max_id()),
  mDvarMap(max_id())
{
  if ( bnode != nullptr ) {
    set_end_mark(bnode);
  }

  mark_tfo_tfi(fnode);

  for (ymuint i = 0; i < tfo_num(); ++ i) {
    const TpgNode* node = tfo_node(i);
    if ( node != fnode ) {
      // 故障回路のゲートの入出力関係を表すCNFを作る．
      //node->make_cnf(solver(), GateLitMap_vid(node, fvar_map()));
    }

    if ( detect == kVal1 ) {
      // D-Chain 制約を作る．
      make_dchain_cnf(node);
    }
  }

  ymuint npo = output_num();

  if ( detect == kVal0 ) {
    for (ymuint i = 0; i < npo; ++ i) {
      const TpgNode* node = output_node(i);
      SatLiteral glit(gvar(node), false);
      SatLiteral flit(fvar(node), false);
      solver().add_clause(~glit,  flit);
      solver().add_clause( glit, ~flit);
    }
  }
  else if ( detect == kVal1 ) {
    vector<SatLiteral> tmp_lits;
    tmp_lits.reserve(npo);
    for (ymuint i = 0; i < npo; ++ i) {
      const TpgNode* node = output_node(i);
      SatLiteral dlit(dvar(node));
      tmp_lits.push_back(dlit);
    }
    solver().add_clause(tmp_lits);

    SatLiteral dlit(dvar(fnode));
    solver().add_clause(dlit);
  }
}

// @brief デストラクタ
FoCone::~FoCone()
{
}

// @brief 十分条件を得る．
// @param[in] sat_model SAT の割り当て結果
// @param[in] fault 故障
// @param[out] suf_list 十分条件の割り当てリスト
void
FoCone::get_suf_list(const vector<SatBool3>& sat_model,
		     const TpgFault* fault,
		     NodeValList& suf_list) const
{
  ValMap_model val_map(gvar_map(), fvar_map(), sat_model);

  Extractor extractor(val_map);
  extractor(fault, suf_list);
  suf_list.sort();
}

// @brief 指定されたノードの TFO の TFI に印をつける．
// @param[in] node_list 起点となるノードのリスト
// @param[in] use_dvar D-var を使う時 true にする．
void
FoCone::mark_tfo_tfi(const vector<const TpgNode*>& node_list,
		       bool use_dvar)
{
  mNodeList.reserve(max_id());

  // node_list の TFO を mNodeList に入れる．
  for (ymuint i = 0; i < node_list.size(); ++ i) {
    const TpgNode* node = node_list[i];
    set_tfo_mark(node);
  }
  for (ymuint rpos = 0; rpos < mNodeList.size(); ++ rpos) {
    const TpgNode* node = mNodeList[rpos];
    if ( end_mark(node) ) {
      continue;
    }
    ymuint nfo = node->fanout_num();
    for (ymuint i = 0; i < nfo; ++ i) {
      const TpgNode* fonode = node->fanout(i);
      if ( !tfo_mark(fonode) ) {
	set_tfo_mark(fonode);
      }
    }
  }

  mTfoNum = mNodeList.size();

  // mNodeList に含まれるノードの TFI を mNodeList に追加する．
  for (ymuint rpos = 0; rpos < mNodeList.size(); ++ rpos) {
    const TpgNode* node = mNodeList[rpos];
    ymuint ni = node->fanin_num();
    for (ymuint i = 0; i < ni; ++ i) {
      const TpgNode* inode = node->fanin(i);
      if ( !tfo_mark(inode) ) {
	set_tfo_mark(inode);
      }
    }
  }

  // 出力のリストを整列しておく．
  sort(mOutputList.begin(), mOutputList.end(), Lt());

  // 出力のリストに end マークをつけておく．
  for (ymuint i = 0; i < mOutputList.size(); ++ i) {
    set_end_mark(mOutputList[i]);
  }

  // TFO のノードに変数を割り当てる．
  for (ymuint i = 0; i < mTfoNum; ++ i) {
    const TpgNode* node = mNodeList[i];
    mStructSat.make_tfi_cnf(node);
    SatVarId fvar = solver().new_variable();
    set_fvar(node, fvar);
    if ( debug ) {
      cout << "fvar(Node#" << node->id() << ") = " << fvar << endl;
    }
    if ( use_dvar ) {
      SatVarId dvar = solver().new_variable();
      set_dvar(node, dvar);
    }
  }

  // TFO に含まれないノードの fvar を gvar にする．
  for (ymuint i = mTfoNum; i < mNodeList.size(); ++ i) {
    const TpgNode* node = mNodeList[i];
    set_fvar(node, gvar(node));
  }
}

// @brief 指定されたノードの TFO の TFI に印をつける．
// @param[in] node 起点となるノード
// @param[in] use_dvar D-var を使う時 true にする．
void
FoCone::mark_tfo_tfi(const TpgNode* node,
		       bool use_dvar)
{
  mark_tfo_tfi(vector<const TpgNode*>(1, node), use_dvar);
}

// @brief node に関する故障伝搬条件を作る．
void
FoCone::make_dchain_cnf(const TpgNode* node)
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
    ymuint nfo = node->fanout_num();
    vector<SatLiteral> tmp_lits;
    tmp_lits.reserve(nfo + 1);
    tmp_lits.push_back(~dlit);
    for (ymuint j = 0; j < nfo; ++ j) {
      const TpgNode* onode = node->fanout(j);
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

END_NAMESPACE_DRUID_SA
