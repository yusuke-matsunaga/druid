
/// @file Extractor.cc
/// @brief Extractor の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2015, 2017 Yusuke Matsunaga
/// All rights reserved.


#include "Extractor.h"
#include "TpgFault.h"
#include "TpgNode.h"
#include "NodeValList.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// node の TFO にマークをつける．
void
dfs(const TpgNode* node,
    HashSet<ymuint>& mark)
{
  if ( mark.check(node->id()) ) {
    return;
  }
  mark.add(node->id());

  ymuint nfo = node->fanout_num();
  for (ymuint i = 0; i < nfo; ++ i) {
    const TpgNode* onode = node->fanout(i);
    dfs(onode, mark);
  }
}

END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// クラス Extractor
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] fault_type 故障の型
// @param[in] val_map ノードの値割当を保持するクラス
Extractor::Extractor(FaultType fault_type,
		     const ValMap& val_map) :
  mFaultType(fault_type),
  mValMap(val_map)
{
}

// @brief デストラクタ
Extractor::~Extractor()
{
}

// @brief 値割当を求める．
// @param[in] fault 故障
// @param[out] assign_list 値の割当リスト
void
Extractor::operator()(const TpgFault* fault,
		      NodeValList& assign_list)
{
  const TpgNode* fnode = fault->tpg_onode();

  // fnode の TFO (fault cone) に印をつける．
  mFconeMark.clear();
  dfs(fnode, mFconeMark);

  mRecorded.clear();
  assign_list.clear();

  // 故障の活性化条件を記録する．
  mRecorded.add(fnode->id());
  record_node(fnode, assign_list);
  if ( fault->is_branch_fault() ) {
    ymuint ni = fnode->fanin_num();
    for (ymuint i = 0; i < ni; ++ i) {
      const TpgNode* inode = fnode->fanin(i);
      mRecorded.add(inode->id());
      record_node(inode, assign_list);
    }
  }
  if ( mFaultType == kFsTransitionDelay ) {
    bool val = (mValMap.hval(node) == kVal1);
    assign_list.add(node, 0, val);
  }

  // 故障差の伝搬している経路を探す．
  const TpgNode* spo = find_sensitized_output(fnode);
  ASSERT_COND( spo != nullptr );

  // その経路の side input の値を記録する．
  record_sensitized_node(spo, assign_list);

  assign_list.sort();

  if ( false ) {
    ostream& dbg_out = cout;
    ymuint n = assign_list.size();
    for (ymuint i = 0; i < n; ++ i) {
      NodeVal nv = assign_list[i];
      const TpgNode* node = nv.node();
      dbg_out << "Node#" << node->id()
	      << ":";
      if ( nv.val() ) {
	dbg_out << "1";
      }
      else {
	dbg_out << "0";
      }
    }
    dbg_out << endl;
  }
}

// @brief 故障の影響を伝搬するノードを求める．
// @param[in] node 対象のノード
const TpgNode*
Extractor::find_sensitized_output(const TpgNode* node)
{
  if ( node->is_ppo() ) {
    return node;
  }

  ymuint nfo = node->fanout_num();
  for (ymuint i = 0; i < nfo; ++ i) {
    const TpgNode* onode = node->fanout(i);
    if ( mValMap.gval(onode) != mValMap.fval(onode) ) {
      const TpgNode* ans = find_sensitized_output(onode);
      if ( ans != nullptr ) {
	return ans;
      }
    }
  }
  return nullptr;
}

// @brief 故障の影響の伝搬を保証する値割当を記録する．
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
  record_node(node, assign_list);

  ASSERT_COND( mValMap.gval(node) != mValMap.fval(node) );

  ymuint ni = node->fanin_num();
  for (ymuint i = 0; i < ni; ++ i) {
    const TpgNode* inode = node->fanin(i);
    if ( mFconeMark.check(inode->id()) ) {
      if ( mValMap.gval(inode) != mValMap.fval(inode) ) {
	record_sensitized_node(inode, assign_list);
      }
      else {
	record_masking_node(inode, assign_list);
      }
    }
    else {
      record_side_inputs(inode, assign_list);
    }
  }
}

// @brief side inputs の値を記録する．
// @param[in] node 対象のノード
// @param[out] assign_list 値割当を記録するリスト
void
Extractor::record_side_inputs(const TpgNode* node,
			      NodeValList& assign_list)
{
  if ( mValMap.gval(node) != mValMap.fval(node) ) {
    return;
  }

  if ( mRecorded.check(node->id()) ) {
    return;
  }
  mRecorded.add(node->id());
  record_node(node, assign_list);
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

  if ( !mFconeMark.check(node->id()) ) {
    record_node(node, assign_list);
    return;
  }

  ASSERT_COND ( mValMap.gval(node) == mValMap.fval(node) );

  ymuint ni = node->fanin_num();
  // ファンインには sensitized node があって
  // side input がある場合．
  bool has_cval = false;
  bool has_snode = false;
  const TpgNode* cnode = nullptr;
  for (ymuint i = 0; i < ni; ++ i) {
    const TpgNode* inode = node->fanin(i);
    if ( mFconeMark.check(inode->id()) ) {
      if ( mValMap.gval(inode) != mValMap.fval(inode) ) {
	has_snode = true;
      }
    }
    else {
      if ( node->cval() == mValMap.gval(inode) ) {
	has_cval = true;
	cnode = inode;
      }
    }
    if ( has_snode && has_cval ) {
      record_side_inputs(cnode, assign_list);
      return;
    }
  }

  // ファンインに再帰する．
  for (ymuint i = 0; i < ni; ++ i) {
    const TpgNode* inode = node->fanin(i);
    if ( mFconeMark.check(inode->id()) && mValMap.gval(inode) != mValMap.fval(inode) ) {
      record_sensitized_node(inode, assign_list);
    }
    else {
      record_masking_node(inode, assign_list);
    }
  }
}

// @brief ノードの割当を記録する．
// @param[in] node 対象のノード
// @param[out] assign_list 値割当を記録するリスト
void
Extractor::record_node(const TpgNode* node,
		       NodeValList& assign_list)
{
  int time = mFaultType == kFsStuckAt ? 0 : 1;
  bool val = (mValMap.gval(node) == kVal1);
  assign_list.add(node, time, val);
}

END_NAMESPACE_DRUID
