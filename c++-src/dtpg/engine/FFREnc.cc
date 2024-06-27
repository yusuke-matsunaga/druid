
/// @file FFREnc.cc
/// @brief FFREnc の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "FFREnc.h"
#include "TpgFFR.h"
#include "Val3.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
FFREnc::FFREnc(
  BaseEnc& base_enc,
  const TpgFFR* ffr
) : SubEnc{base_enc},
    mFFR{ffr}
{
}

// @brief 必要な変数を割り当てCNF式を作る．
void
FFREnc::make_cnf()
{
  // 根のノードは無条件で伝搬する．
  auto root = mFFR->root();
  auto pvar = solver().new_variable(true);
  mPropVarMap.emplace(root->id(), pvar);
  // DFS の in-order で条件を作る．
  make_cnf_sub(root);
}

void
FFREnc::make_cnf_sub(
  const TpgNode* node
)
{
  // node の伝搬条件を表す変数
  auto ovar = mPropVarMap.at(node->id());
  auto nval = node->nval();
  if ( node->fanin_num() == 1 || node->nval() == Val3::_X ) {
    // ovar をそのままファンインの条件とする．
    for ( auto inode: node->fanin_list() ) {
      mPropVarMap.emplace(inode->id(), ovar);
    }
  }
  else {
    // side-input の値を nval にしたものを伝搬条件に加える．
    auto bval = nval == Val3::_1;
    // 2乗のループを避けるため前半と後半の配列を作る．
    SizeType ni = node->fanin_num();
    vector<vector<SatLiteral>> tmp_list1(ni);
    vector<vector<SatLiteral>> tmp_list2(ni);
    for ( SizeType i = 1; i < ni; ++ i ) {
      tmp_list1[i] = tmp_list1[i - 1];
      auto inode = node->fanin(i - 1);
      auto lit = conv_to_literal(NodeTimeVal{inode, 1, bval});
      tmp_list1[i].push_back(lit);
    }
    for ( SizeType i = 1; i < ni; ++ i ) {
      tmp_list2[ni - i - 1] = tmp_list2[ni - i];
      auto inode = node->fanin(ni - i);
      auto lit = conv_to_literal(NodeTimeVal{inode, 1, bval});
      tmp_list2[ni - i - 1].push_back(lit);
    }
    for ( SizeType i = 0; i < ni; ++ i ) {
      auto inode = node->fanin(i);
      vector<SatLiteral> cond;
      cond.reserve(ni);
      cond.push_back(ovar);
      for ( auto lit: tmp_list1[i] ) {
	cond.push_back(lit);
      }
      for ( auto lit: tmp_list2[i] ) {
	cond.push_back(lit);
      }
      auto plit = solver().new_variable(true);
      solver().add_andgate(plit, cond);
      mPropVarMap.emplace(inode->id(), plit);
    }
  }
  for ( auto inode: node->fanin_list() ) {
    if ( inode->ffr_root() == inode || inode->is_ppi() ) {
      continue;
    }
    make_cnf_sub(inode);
  }
}

// @brief 故障伝搬条件を表す変数を返す．
SatLiteral
FFREnc::prop_var(
  const TpgNode* node
)
{
  return mPropVarMap.at(node->id());
}

// @brief 関連するノードのリストを返す．
const vector<const TpgNode*>&
FFREnc::node_list() const
{
  return mFFR->node_list();
}

END_NAMESPACE_DRUID
