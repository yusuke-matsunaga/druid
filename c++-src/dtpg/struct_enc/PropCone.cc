
/// @file PropCone.cc
/// @brief PropCone の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2012-2014, 2017, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "PropCone.h"
#include "StructEnc.h"
#include "TpgNode.h"
#include "TpgFault.h"
#include "NodeTimeValList.h"
#include "GateEnc.h"
#include "TpgNodeSet.h"
#include "extract.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

bool debug = false;

END_NONAMESPACE

// @brief コンストラクタ
PropCone::PropCone(
  StructEnc& struct_enc,
  const TpgNode* root_node
) : mStructEnc{struct_enc},
    mMaxNodeId{struct_enc.max_node_id()},
    mMarkArray(max_id()),
    mFvarMap(max_id()),
    mDvarMap(max_id())
{
  mNodeList = TpgNodeSet::get_tfo_list(max_id(), root_node,
				       [&](const TpgNode* node) {
					 if ( node->is_ppo() ) {
					   set_end_mark(node);
					   mOutputList.push_back(node);
					 }
					 mTfoMark.emplace(node->id());
				       });

  // 出力のリストを output_id2() の昇順に整列しておく．
  sort(mOutputList.begin(), mOutputList.end(),
       [](const TpgNode* left, const TpgNode* right) {
	 return left->output_id2() < right->output_id2();
       });

  mPropVar = solver().new_variable(true);
}

// @brief デストラクタ
PropCone::~PropCone()
{
}

// @brief 関係するノードの変数を作る．
void
PropCone::make_vars(
  const vector<const TpgNode*>& node_list
)
{
  for ( auto node: node_list ) {
    auto var = gvar(node);
    set_fvar(node, var);
  }

  // TFO のノードに変数を割り当てる．
  for ( auto node: mNodeList ) {
    auto fvar = solver().new_variable(true);
    set_fvar(node, fvar);
    auto dvar = solver().new_variable(false);
    set_dvar(node, dvar);
    if ( debug ) {
      cout << "Node#" << node->id() << "fvar = " << fvar
	   << ", dvar = " << dvar << endl;
    }
  }
}

// @brief 関係するノードの入出力の関係を表すCNFを作る．
void
PropCone::make_cnf()
{
  GateEnc gate_enc{solver(), fvar_map()};
  for ( auto node: mNodeList ) {
    if ( node != root_node() ) {
      // 故障回路のゲートの入出力関係を表すCNFを作る．
      gate_enc.make_cnf(node);
    }
    // D-Chain 制約を作る．
    make_dchain_cnf(node);
  }

  // 外部出力へ故障の影響が伝搬する条件を作る．
  vector<SatLiteral> odiff;
  odiff.reserve(mOutputList.size());
  for ( auto node: mOutputList ) {
    auto dlit = dvar(node);
    odiff.push_back(dlit);
  }
  solver().add_orgate(mPropVar, odiff);

  auto root = root_node();
  if ( !root->is_ppo() ) {
    // root の dlit が1でなければならない．
    auto dlit = dvar(root);
    solver().add_clause(dlit);
  }
}

// @brief 故障検出に必要な割り当てを求める．
NodeTimeValList
PropCone::extract_condition(
  const SatModel& model,
  const TpgNode* root
)
{
  return extract_sufficient_condition(root, gvar_map(), fvar_map(), model);
}

// @brief node に関する故障伝搬条件を作る．
void
PropCone::make_dchain_cnf(
  const TpgNode* node
)
{
  auto glit = gvar(node);
  auto flit = fvar(node);
  auto dlit = dvar(node);

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
    SizeType nfo = node->fanout_num();
    vector<SatLiteral> tmp_lits;
    tmp_lits.reserve(nfo + 1);
    tmp_lits.push_back(~dlit);
    for ( auto onode: node->fanout_list() ) {
      auto odlit = dvar(onode);
      tmp_lits.push_back(odlit);
    }
    solver().add_clause(tmp_lits);

    // immediate dominator がある場合，immediate dominator の dlit も 1 でなければならない．
    auto idom = node->imm_dom();
    if ( idom != nullptr ) {
      auto odlit = dvar(idom);
      solver().add_clause(~dlit, odlit);
    }
  }
}

END_NAMESPACE_DRUID
