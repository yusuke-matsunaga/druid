
/// @file PropCone.cc
/// @brief PropCone の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2012-2014, 2017, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "PropCone.h"
#include "StructEnc.h"
#include "Extractor.h"
#include "TpgNode.h"
#include "TpgFault.h"
#include "NodeValList.h"
#include "GateEnc.h"


BEGIN_NAMESPACE_DRUID_STRUCTENC

BEGIN_NONAMESPACE

bool debug = false;

// TpgNode::output_id2() の値に基づく比較を行う．
struct Lt
{
  bool
  operator()(
    const TpgNode* left,
    const TpgNode* right
  )
  {
    return left->output_id2() < right->output_id2();
  }
};

END_NONAMESPACE

// @brief コンストラクタ
PropCone::PropCone(
  StructEnc& struct_enc,
  const TpgNode* root_node,
  const TpgNode* block_node,
  bool detect
) : mStructEnc{struct_enc},
    mDetect{detect},
    mMaxNodeId{struct_enc.max_node_id()},
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
void
PropCone::mark_tfo(
  const TpgNode* node
)
{
  set_tfo_mark(node);

  for ( SizeType rpos = 0; rpos < mNodeList.size(); ++ rpos ) {
    auto node = mNodeList[rpos];
    if ( end_mark(node) ) {
      // ここで止まる．
      continue;
    }
    for ( auto fonode: node->fanout_list() ) {
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
  for ( SizeType i = 0; i < mNodeList.size(); ++ i ) {
    auto node = mNodeList[i];
    auto fvar = solver().new_variable(true);
    set_fvar(node, fvar);
    if ( debug ) {
      cout << "fvar(Node#" << node->id() << ") = " << fvar << endl;
    }
    if ( mDetect ) {
      auto dvar = solver().new_variable(true);
      set_dvar(node, dvar);
    }
#if 0
    // ファンインのノードうち TFO に含まれないノードの fvar を gvar にする．
    SizeType ni = node->fanin_num();
    for ( SizeType i = 0; i < ni; ++ i ) {
      auto inode = node->fanin(i);
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
  for ( SizeType i = 0; i < mNodeList.size(); ++ i ) {
    const TpgNode* node = mNodeList[i];
    for ( auto inode: node->fanin_list() ) {
      if ( !tfo_mark(inode) && !tfi_mark[inode->id()] ) {
	tfi_mark[inode->id()] = true;
	tmp_list.push_back(inode);
      }
    }
  }
  for ( SizeType rpos = 0; rpos < tmp_list.size(); ++ rpos ) {
    auto node = tmp_list[rpos];
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
  for ( SizeType i = 0; i < mNodeList.size(); ++ i ) {
    auto node = mNodeList[i];
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
  vector<SatLiteral> odiff;
  odiff.reserve(mOutputList.size());
  for ( auto node: mOutputList ) {
    auto dlit = dvar(node);
    odiff.push_back(dlit);
  }
  solver().add_clause(odiff);

  auto root = mNodeList[0];
  if ( !root->is_ppo() ) {
    // root の dlit が1でなければならない．
    auto dlit = dvar(root);
    solver().add_clause(dlit);
  }
}

// @brief 故障検出に必要な割り当てを求める．
NodeValList
PropCone::extract_condition(
  const SatModel& model,
  const TpgNode* root
)
{
  Extractor extractor{gvar_map(), fvar_map(), model};
  return extractor.get_assignment({root});
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

END_NAMESPACE_DRUID_STRUCTENC
