
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
  cout << endl;
  cout << "FFREnc::make_cnf()" << endl;
  // 変数を用意する．
  for ( auto node: mFFR->node_list() ) {
    auto plit = solver().new_variable(true);
    mPropVarMap.emplace(node->id(), plit);
    cout << "prop_var(Node#" << node->id() << ") = " << plit << endl;
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
  if ( node->is_ppi() ) {
    return;
  }
  auto nval = node->nval();
  if ( nval != Val3::_X ) {
    auto bval = nval == Val3::_1;
    for ( auto inode: node->fanin_list() ) {
      if ( inode->ffr_root() == inode ) {
	// inode は他の FFR の根のノードだった．
	continue;
      }
      // inode から node の出力までの伝搬条件
      // inode 以外の side-input の値が nval() であること．
      vector<SatLiteral> cond1{cond};
      cond1.reserve(cond1.size() + node->fanin_num() - 1);
      for ( auto inode1: node->fanin_list() ) {
	if ( inode1 == inode ) {
	  continue;
	}
	auto lit = conv_to_literal(NodeTimeVal{inode1, 1, bval});
	cond1.push_back(lit);
      }
      if ( !cond1.empty() ) {
	cout << "get prop_var(Node#" << inode->id() << ")" << endl;
	auto plit = mPropVarMap.at(inode->id());
	solver().add_andgate(plit, cond1);
      }
      make_cnf_sub(inode, cond1);
    }
  }
}

// @brief 故障伝搬条件を表す変数を返す．
SatLiteral
FFREnc::prop_var(
  const TpgNode* node
)
{
  cout << "get prop_var(Node#" << node->id() << ")" << endl;
  return mPropVarMap.at(node->id());
}

// @brief 関連するノードのリストを返す．
const vector<const TpgNode*>&
FFREnc::node_list() const
{
  return mFFR->node_list();
}

END_NAMESPACE_DRUID
