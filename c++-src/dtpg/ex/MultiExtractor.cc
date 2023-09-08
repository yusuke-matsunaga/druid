
/// @file MultiExtractor.cc
/// @brief MultiExtractor の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2015, 2017, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "MultiExtractor.h"
#include "TpgFault.h"
#include "TpgNode.h"
#include "NodeValList.h"


BEGIN_NAMESPACE_DRUID

Expr
extract_all(
  const TpgNode* root,
  const VidMap& gvar_map,
  const VidMap& fvar_map,
  const SatModel& model
)
{
  MultiExtractor extractor{gvar_map, fvar_map, model};
  return extractor.get_assignments(root);
}

BEGIN_NONAMESPACE

int debug = false;

END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// クラス MultiExtractor
//////////////////////////////////////////////////////////////////////

MultiExtractor::MultiExtractor(
  const VidMap& gvar_map,
  const VidMap& fvar_map,
  const SatModel& model
) : mGvarMap{gvar_map},
    mFvarMap{fvar_map},
    mSatModel{model}
{
}

// @brief デストラクタ
MultiExtractor::~MultiExtractor()
{
}

// @brief 各出力へ故障伝搬する値割り当てを求める．
Expr
MultiExtractor::get_assignments(
  const TpgNode* root
)
{
  // root の TFO (fault cone) に印をつける．
  // 同時に故障差の伝搬している外部出力のリストを作る．
  mFconeMark.clear();
  mark_tfo(root);
  ASSERT_COND( mSpoList.size() > 0 );

  mExprMap.clear();

  // 故障差の伝搬している経路を探す．
  Expr expr = Expr::make_zero();
  for ( auto spo: mSpoList ) {
    // spo に到達する故障伝搬経路の
    // side input の値を記録する．
    expr |= record_sensitized_node(spo);
  }

#if 0
  if ( debug ) {
    ostream& dbg_out = cout;
    dbg_out << "Extract at Node#" << root->id() << endl;
    for ( auto& assign_list: assignment_list ) {
      const char* comma = "";
      for ( auto nv: assign_list ) {
	const TpgNode* node = nv.node();
	dbg_out << comma << "Node#" << node->id()
		<< ":";
	if ( nv.val() ) {
	  dbg_out << "1";
	}
	else {
	  dbg_out << "0";
	}
	comma = ", ";
      }
      dbg_out << endl;
    }
  }
#endif

  return expr;
}

// @brief node の TFO に印をつけ，故障差の伝搬している外部出力を求める．
void
MultiExtractor::mark_tfo(
  const TpgNode* node
)
{
  if ( mFconeMark.count(node->id()) > 0 ) {
    return;
  }
  mFconeMark.emplace(node->id());

  if ( node->is_ppo() ) {
    if ( gval(node) != fval(node) ) {
      mSpoList.push_back(node);
    }
  }

  for ( auto onode: node->fanout_list() ) {
    mark_tfo(onode);
  }
}

// @brief 故障の影響の伝搬を阻害する値割当を記録する．
Expr
MultiExtractor::record_sensitized_node(
  const TpgNode* node
)
{
  ASSERT_COND( gval(node) != fval(node) );

  if ( mExprMap.count(node->id()) == 0 ) {
    // * 故障差の伝搬しているファンインには record_sensitized_node() を呼ぶ．
    // * そうでない fault-cone 内のファンインには record_masking_node() を呼ぶ．
    // * それ以外のファンインには record_side_input() を呼ぶ．
    auto expr = Expr::make_one();
    for ( auto inode: node->fanin_list() ) {
      Expr expr1;
      if ( mFconeMark.count(inode->id()) > 0 ) {
	// fault-cone 内部のノード
	if ( gval(inode) != fval(inode) ) {
	  expr1 = record_sensitized_node(inode);
	}
	else {
	  expr1 = record_masking_node(inode);
	}
      }
      else {
	// fault-cone 外部のノード
	expr1 = record_side_input(inode);
      }
      expr &= expr1;
    }
    mExprMap.emplace(node->id(), expr);
  }

  return mExprMap.at(node->id());
}

// @brief 故障の影響の伝搬を阻害する値割当を記録する．
Expr
MultiExtractor::record_masking_node(
  const TpgNode* node
)
{
  ASSERT_COND ( gval(node) == fval(node) );

  // 以下の3通りの場合がある．
  // * [case1] fault-cone 内で制御値をもったファンインがある．
  // * [case2] fault-cone 以外で制御値を持ったファンインがある．
  // * [case3] 故障差の伝搬しているファンインが複数あって打ち消し合っている．
  if ( mExprMap.count(node->id()) == 0 ) {
    bool has_cnode = false;
    vector<const TpgNode*> c1node_list;
    vector<const TpgNode*> c2node_list;
    for ( auto inode: node->fanin_list() ) {
      if ( mFconeMark.count(inode->id()) > 0 ) {
	if ( gval(inode) == fval(inode) && gval(inode) == node->cval() ) {
	  has_cnode = true;
	  c1node_list.push_back(inode);
	}
      }
      else if ( gval(inode) == node->cval() ) {
	has_cnode = true;
	c2node_list.push_back(inode);
      }
    }
    Expr expr;
    if ( has_cnode ) {
      // 制御値を持つノードの値を確定させる．
      expr = Expr::make_zero();
      for ( auto cnode: c1node_list ) {
	expr |= record_masking_node(cnode);
      }
      for ( auto cnode: c2node_list ) {
	expr |= record_side_input(cnode);
      }
    }
    else {
      // ここに来たということは全てのファンインに故障差が伝搬していないか
      // 複数のファンインの故障差が打ち消し合っているのですべてのファンイン
      // に再帰する．
      expr = Expr::make_one();
      for ( auto inode: node->fanin_list() ) {
	if ( mFconeMark.count(inode->id()) > 0 ) {
	  if ( gval(inode) != fval(inode) ) {
	    expr &= record_sensitized_node(inode);
	  }
	  else {
	    expr &= record_masking_node(inode);
	  }
	}
	else {
	  expr &= record_side_input(inode);
	}
      }
    }
    mExprMap.emplace(node->id(), expr);
  }

  return mExprMap.at(node->id());
}

END_NAMESPACE_DRUID
