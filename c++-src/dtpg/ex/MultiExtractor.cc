
/// @file MultiExtractor.cc
/// @brief MultiExtractor の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "MultiExtractor.h"
#include "AssignExpr.h"


#define DBG_OUT cerr

BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE
int debug = false;
END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// クラス MultiExtractor
//////////////////////////////////////////////////////////////////////

// @brief インスタンスを生成するクラスメソッド
MultiExtractor*
MultiExtractor::new_impl(
  const JsonValue& option
)
{
  return new MultiExtractor;
}

// @brief 各出力へ故障伝搬する値割り当てを求める．
AssignExpr
MultiExtractor::operator()(
  const TpgNode* root,
  const VidMap& gvar_map,
  const VidMap& fvar_map,
  const SatModel& model
)
{
  ExData data{root, gvar_map, fvar_map, model};

  mData = &data;
  clear_queue();

  // 故障が伝搬している出力を取り出す．
  vector<AssignExpr> ans_list;
  for ( auto spo: mData->sensitized_output_list() ) {
    // 故障箇所から spo までの故障伝搬を行う条件を記録する．
    AssignExpr assign_list;
    vector<AssignExpr> choice_list;
    put_queue(spo, 1);
    while ( !mQueue.empty() ) {
      auto node = get_queue();
      if ( node == root ) {
	continue;
      }
      int mark = mMarks.at(node->id());
      if ( debug ) {
	DBG_OUT << "visit at Node#" << node->id()
		<< ": " << mark << endl;
      }
      switch ( mark ) {
      case 1:
	// 故障の影響が伝搬しているノード
	record_sensitized_node(node);
	break;
      case 2:
	// 故障の影響が伝搬していないノード
	record_masking_node(node, choice_list);
	break;
      case 3:
	{ // 境界ノード
	  // 無条件に現在の値を記録する．
	  bool val = (gval(node) == Val3::_1);
	  assign_list &= AssignExpr::make_literal({node, 1, val});
	}
	break;
      }
    }
    // AssignExpr を作る．
    auto ans1 = AssignExpr::make_and(choice_list);
    ans1 &= assign_list;
    ans_list.push_back(ans1);
  }
  return AssignExpr::make_or(ans_list);
}

// @brief 故障の影響の伝搬を阻害する値割当を記録する．
void
MultiExtractor::record_sensitized_node(
  const TpgNode* node
)
{
  if ( debug ) {
    DBG_OUT << "record_sinsitized_node" << endl
	    << gval(node) << " / " << fval(node) << endl;
  }
  ASSERT_COND( gval(node) != fval(node) );

  for ( auto inode: node->fanin_list() ) {
    // 値に応じてタイプ分けを行う．
    // 実は XOR の side-input は X でよいが
    // なにも考えずに現在の値を要求している．
    int t = type(inode);
    put_queue(inode, t);
    // 正確には
    // 1. 故障差の伝搬している入力を一つ選ぶ．
    // 2. その他の入力で伝搬に必要な値を固定する．
    if ( debug ) {
      DBG_OUT << "  Node#" << inode->id()
	      << " type = " << t
	      << " " << gval(inode) << " / " << fval(inode) << endl;
    }
  }
}

// @brief 故障の影響の伝搬を阻害する値割当を記録する．
void
MultiExtractor::record_masking_node(
  const TpgNode* node,
  vector<AssignExpr>& choice_list
)
{
  if ( debug ) {
    DBG_OUT << "record_masking_node" << endl
	    << gval(node) << " / " << fval(node) << endl;
  }
  ASSERT_COND ( gval(node) == fval(node) );

  bool has_snode = false;
  vector<const TpgNode*> cnode_list;
  for ( auto inode: node->fanin_list() ) {
    int t = type(inode);
    if ( t == 1 ) {
      // このノードには故障差が伝搬している．
      has_snode = true;
    }
    else if ( t == 3 ) {
      if ( node->cval() == gval(inode) ) {
	// このノードは制御値を持っている．
	cnode_list.push_back(inode);
      }
    }
  }
  if ( has_snode && cnode_list.size() > 0 ) {
    // node のファンインに故障差が伝搬しており，
    // 他のファンインの制御値でブロックされている場合，
    // その制御値を持つノードの値を確定させる．
    // 制御値を持つファンインが2つ以上ある場合には
    // 異なる結果になる可能性がある．
    if ( cnode_list.size() == 1 ) {
      auto cnode = cnode_list.front();
      put_queue(cnode, 3);
    }
    else {
      vector<AssignExpr> tmp_list;
      for ( auto node: cnode_list ) {
	bool val = (gval(node) == Val3::_1);
	tmp_list.push_back(AssignExpr::make_literal({node, 1, val}));
      }
      auto tmp = AssignExpr::make_or(tmp_list);
      choice_list.push_back(tmp);
    }
  }
  else {
    // ここに来たということは全てのファンインに故障差が伝搬していないか
    // 複数のファンインの故障差が打ち消し合っているのですべてのファンイン
    // に再帰する．
    record_sensitized_node(node);
  }
}

END_NAMESPACE_DRUID
