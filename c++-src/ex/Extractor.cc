
/// @file Extractor.cc
/// @brief Extractor の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2015, 2017, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "Extractor.h"
#include "TpgFault.h"
#include "TpgNode.h"
#include "NodeValList.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

int debug = false;

END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// クラス Extractor
//////////////////////////////////////////////////////////////////////

Extractor::Extractor(
  const VidMap& gvar_map,
  const VidMap& fvar_map,
  const SatModel& model
) : mGvarMap{gvar_map},
    mFvarMap{fvar_map},
    mSatModel{model}
{
}

// @brief デストラクタ
Extractor::~Extractor()
{
}

// @brief 値割り当てを１つ求める．
NodeValList
Extractor::get_assignment(
  const vector<const TpgNode*>& root_list
)
{
  // root の TFO (fault cone) に印をつける．
  // 同時に故障差の伝搬している外部出力のリストを作る．
  mFconeMark.clear();
  mSpoList.clear();
  mQueue.clear();
  mMarks.clear();

  for ( auto root: root_list ) {
    mark_tfo(root);
  }

  // 故障差の伝搬している経路を探す．
  ASSERT_COND( mSpoList.size() > 0 );
  const TpgNode* spo = mSpoList[0];

  // その経路の side input の値を記録する．
  NodeValList assign_list;
  put_queue(spo, 1);
  for ( SizeType rpos = 0; rpos < mQueue.size(); ++ rpos ) {
    auto node = mQueue[rpos];
    int mark = mMarks.at(node->id());
    switch ( mark ) {
    case 1: record_sensitized_node(node); break;
    case 2: record_masking_node(node); break;
    case 3:
      {
	bool val = (gval(node) == Val3::_1);
	assign_list.add(node, 1, val);
      }
      break;
    }
  }

  if ( debug ) {
    ostream& dbg_out = cout;
    dbg_out << "Extract at ";
    const char* comma = "";
    for ( auto root: root_list ) {
      dbg_out << comma << "Node#" << root->id();
      comma = ", ";
    }
    dbg_out << endl;
    comma = "";
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

  return assign_list;
}

// @brief node の TFO に印をつけ，故障差の伝搬している外部出力を求める．
void
Extractor::mark_tfo(
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
void
Extractor::record_sensitized_node(
  const TpgNode* node
)
{
  for ( auto inode: node->fanin_list() ) {
    int t = type(inode);
    put_queue(inode, t);
  }
}

// @brief 故障の影響の伝搬を阻害する値割当を記録する．
void
Extractor::record_masking_node(
  const TpgNode* node
)
{
  bool has_cval = false;
  bool has_snode = false;
  const TpgNode* cnode = nullptr;
  for ( auto inode: node->fanin_list() ) {
    int t = type(inode);
    if ( t == 1 ) {
      // このノードには故障差が伝搬している．
      has_snode = true;
    }
    else if ( t == 3 ) {
      if ( node->cval() == gval(inode) ) {
	// このノードは制御値を持っている．
	has_cval = true;
	cnode = inode;
      }
    }
    if ( has_snode && has_cval ) {
      // node のファンインに故障差が伝搬しており，
      // 他のファンインの制御値でブロックされている場合，
      // その制御値を持つノードの値を確定させる．
      // 制御値を持つファンインが2つ以上ある場合には
      // 異なる結果になる可能性がある．
      put_queue(cnode, 3);
      return;
    }
  }

  // ここに来たということは全てのファンインに故障差が伝搬していないか
  // 複数のファンインの故障差が打ち消し合っているのですべてのファンイン
  // に再帰する．
  record_sensitized_node(node);
}

END_NAMESPACE_DRUID
