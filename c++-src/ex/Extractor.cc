
/// @file Extractor.cc
/// @brief Extractor の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2015, 2017, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "Extractor.h"
#include "TpgFault.h"
#include "TpgNode.h"
#include "NodeValList.h"


BEGIN_NAMESPACE_DRUID

NodeValList
extract(const TpgNode* root,
	const VidMap& gvar_map,
	const VidMap& fvar_map,
	const vector<SatBool3>& model)
{
  Extractor extractor(gvar_map, fvar_map, model);
  return extractor.get_assignment(root);
}

BEGIN_NONAMESPACE

int debug = false;

END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// クラス Extractor
//////////////////////////////////////////////////////////////////////

// @param[in] gvar_map 正常値の変数番号のマップ
// @param[in] fvar_map 故障値の変数番号のマップ
// @param[in] model SATソルバの作ったモデル
Extractor::Extractor(const VidMap& gvar_map,
		     const VidMap& fvar_map,
		     const vector<SatBool3>& model) :
  mGvarMap(gvar_map),
  mFvarMap(fvar_map),
  mSatModel(model)
{
}

// @brief デストラクタ
Extractor::~Extractor()
{
}

// @brief 値割当を求める．
// @param[in] root 起点となるノード
// @param[out] assign_list 値の割当リスト
NodeValList
Extractor::get_assignment(const TpgNode* root)
{
  // root の TFO (fault cone) に印をつける．
  // 同時に故障差の伝搬している外部出力のリストを作る．
  mFconeMark.clear();
  mark_tfo(root);

  // 故障差の伝搬している経路を探す．
  ASSERT_COND( mSpoList.size() > 0 );
  const TpgNode* spo = mSpoList[0];

  // その経路の side input の値を記録する．
  mRecorded.clear();

  NodeValList assign_list;

  record_sensitized_node(spo, assign_list);

  if ( debug ) {
    ostream& dbg_out = cout;
    dbg_out << "Extract at Node#" << root->id() << endl;
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

  return assign_list;
}

// @brief node の TFO に印をつけ，故障差の伝搬している外部出力を求める．
void
Extractor::mark_tfo(const TpgNode* node)
{
  if ( mFconeMark.check(node->id()) ) {
    return;
  }
  mFconeMark.add(node->id());

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
// @param[in] node 対象のノード
// @param[out] assign_list 値割当を記録するリスト
void
Extractor::record_sensitized_node(const TpgNode* node,
				  NodeValList& assign_list)
{
  if ( mRecorded.check(node->id()) ) {
    return;
  }
  mRecorded.add(node->id());

  ASSERT_COND( gval(node) != fval(node) );

  for ( auto inode: node->fanin_list() ) {
    if ( mFconeMark.check(inode->id()) ) {
      if ( gval(inode) != fval(inode) ) {
	record_sensitized_node(inode, assign_list);
      }
      else {
	record_masking_node(inode, assign_list);
      }
    }
    else {
      record_side_input(inode, assign_list);
    }
  }
}

// @brief 故障の影響の伝搬を阻害する値割当を記録する．
// @param[in] node 対象のノード
// @param[out] assign_list 値割当を記録するリスト
void
Extractor::record_masking_node(const TpgNode* node,
			       NodeValList& assign_list)
{
  if ( mRecorded.check(node->id()) ) {
    return;
  }
  mRecorded.add(node->id());

  ASSERT_COND ( gval(node) == fval(node) );

  // ファンインには sensitized node があって
  // side input がある場合．
  bool has_cval = false;
  bool has_snode = false;
  const TpgNode* cnode = nullptr;
  for ( auto inode: node->fanin_list() ) {
    if ( mFconeMark.check(inode->id()) ) {
      if ( gval(inode) != fval(inode) ) {
	// このノードには故障差が伝搬している．
	has_snode = true;
      }
    }
    else {
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
      record_side_input(cnode, assign_list);
      return;
    }
  }

  // ここに来たということは全てのファンインに故障差が伝搬していないか
  // 複数のファンインの故障差が打ち消し合っているのですべてのファンイン
  // に再帰する．
  for ( auto inode: node->fanin_list() ) {
    if ( mFconeMark.check(inode->id()) ) {
      if ( gval(inode) != fval(inode) ) {
	record_sensitized_node(inode, assign_list);
      }
      else {
	record_masking_node(inode, assign_list);
      }
    }
    else {
      record_side_input(inode, assign_list);
    }
  }
}

END_NAMESPACE_DRUID
