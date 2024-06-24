
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
  // 変数を用意する．
  for ( auto node: mFFR->node_list() ) {
    auto plit = solver().new_variable(true);
    mPropVarMap.emplace(node->id(), plit);
  }
  // DFS の in-order で条件を作る．
  make_cnf_sub(mFFR->root(), {});
}

void
FFREnc::make_cnf_sub(
  const TpgNode* node,
  const vector<SatLiteral>& cond
)
{
  if ( !cond.empty() ) {
    auto plit = solver().new_variable(true);
    mPropVarMap.emplace(node->id(), plit);
  }
  auto nval = node->nval();
  if ( nval != Val3::_X ) {
    auto bval = nval == Val3::_1;
    for ( auto inode: node->fanin_list() ) {
      // inode から node の出力までの伝搬条件
      // inode 以外の side-input の値が nval() であること．
      vector<SatLiteral> tmp_lits{cond};
      tmp_lits.reserve(tmp_lits.size() + node->fanin_num() - 1);
      for ( auto inode1: node->fanin_list() ) {
	if ( inode1 == inode ) {
	  continue;
	}
	auto lit = conv_to_literal(NodeTimeVal{inode1, 1, bval});
	tmp_lits.push_back(lit);
      }
      make_cnf_sub(inode, tmp_lits);
    }
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
