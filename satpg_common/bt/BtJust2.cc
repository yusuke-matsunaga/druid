﻿
/// @file BtJust2.cc
/// @brief BtJust2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "BtJust2.h"
#include "TpgDff.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
// クラス BtJust2
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] max_id ノード番号の最大値
// @param[in] td_mode 遷移故障モードの時 true にするフラグ
// @param[in] val_map ノードの値を保持するクラス
BtJust2::BtJust2(ymuint max_id,
		 bool td_mode,
		 const ValMap& val_map) :
  BtImpl(max_id, td_mode, val_map),
  mAlloc(sizeof(NodeList), 1024),
  mJustArray(max_id * 2, nullptr)
{
}

// @brief デストラクタ
BtJust2::~BtJust2()
{
}

// @brief バックトレースを行なう．
// @param[in] assign_list 値の割り当てリスト
// @param[in] output_list 故障に関係する出力ノードのリスト
// @param[out] pi_assign_list 外部入力上の値の割当リスト
//
// assign_list には故障の活性化条件と ffr_root までの故障伝搬条件
// を入れる．
void
BtJust2::run(const NodeValList& assign_list,
	     const vector<const TpgNode*>& output_list,
	     NodeValList& pi_assign_list)
{
  pi_assign_list.clear();

  // assign_list の値を正当化する．
  NodeList* node_list0 = nullptr;
  for (ymuint i = 0; i < assign_list.size(); ++ i) {
    NodeVal nv = assign_list[i];
    const TpgNode* node = nv.node();
    NodeList* node_list = justify(node, nv.time());
    list_merge(node_list0, node_list);
  }

  // 故障差の伝搬している外部出力を選ぶ．
  ymuint nmin = -1;
  NodeList* best_list = nullptr;
  for (vector<const TpgNode*>::const_iterator p = output_list.begin();
       p != output_list.end(); ++ p) {
    const TpgNode* node = *p;
    if ( gval(node, 1) != fval(node, 1) ) {
      // 正当化を行う．
      NodeList* node_list = justify(node, 1);
      ymuint n = list_size(node_list);
      if ( nmin > n ) {
	nmin = n;
	best_list = node_list;
      }
    }
  }
  ASSERT_COND( nmin != -1 );

  list_merge(best_list, node_list0);

  for (NodeList* tmp = best_list; tmp; tmp = tmp->mLink) {
    const TpgNode* node = tmp->mNode;
    int time = tmp->mTime;
    record_value(node, time, pi_assign_list);
  }
}

// @brief solve 中で変数割り当ての正当化を行なう．
// @param[in] node 対象のノード
// @param[in] time タイムフレーム ( 0 or 1 )
BtJust2::NodeList*
BtJust2::justify(const TpgNode* node,
		 int time)
{
  NodeList*& node_list = mJustArray[node->id() * 2 + time];
  if ( justified_mark(node, time) ) {
    return node_list;
  }
  set_justified(node, time);

  if ( node->is_primary_input() ) {
    // val を記録
    node_list = new_list_cell(node, time);
    return node_list;
  }
  if ( node->is_dff_output() ) {
    if ( time == 1 && td_mode() ) {
      // 1時刻前のタイムフレームに戻る．
      const TpgDff* dff = node->dff();
      const TpgNode* alt_node = dff->input();
      node_list = justify(alt_node, 0);
    }
    else {
      // val を記録
      node_list = new_list_cell(node, time);
    }
    return node_list;
  }

  Val3 gval = this->gval(node, time);
  Val3 fval = this->fval(node, time);

  if ( gval != fval ) {
    // 正常値と故障値が異なっていたら
    // すべてのファンインをたどる．
    return just_all(node, time);
  }

  switch ( node->gate_type() ) {
  case kGateBUFF:
  case kGateNOT:
    // 無条件で唯一のファンインをたどる．
    return just_all(node, time);

  case kGateAND:
    if ( gval == kVal1 ) {
      // すべてのファンインノードをたどる．
      return just_all(node, time);
    }
    else if ( gval == kVal0 ) {
      // 0の値を持つ最初のノードをたどる．
      return just_one(node, time, kVal0);
    }
    break;

  case kGateNAND:
    if ( gval == kVal1 ) {
      // 0の値を持つ最初のノードをたどる．
      return just_one(node, time, kVal0);
    }
    else if ( gval == kVal0 ) {
      // すべてのファンインノードをたどる．
      return just_all(node, time);
    }
    break;

  case kGateOR:
    if ( gval == kVal1 ) {
      // 1の値を持つ最初のノードをたどる．
      return just_one(node, time, kVal1);
    }
    else if ( gval == kVal0 ) {
      // すべてのファンインノードをたどる．
      return just_all(node, time);
    }
    break;

  case kGateNOR:
    if ( gval == kVal1 ) {
      // すべてのファンインノードをたどる．
      return just_all(node, time);
    }
    else if ( gval == kVal0 ) {
      // 1の値を持つ最初のノードをたどる．
      return just_one(node, time, kVal1);
    }
    break;

  case kGateXOR:
  case kGateXNOR:
    // すべてのファンインノードをたどる．
    return just_all(node, time);
    break;

  default:
    ASSERT_NOT_REACHED;
    break;
  }

  return nullptr;
}

// @brief すべてのファンインに対して justify() を呼ぶ．
// @param[in] node 対象のノード
// @param[in] time タイムフレーム ( 0 or 1 )
BtJust2::NodeList*
BtJust2::just_all(const TpgNode* node,
		  int time)
{
  NodeList*& node_list = mJustArray[node->id() * 2 + time];
  ymuint ni = node->fanin_num();
  for (ymuint i = 0; i < ni; ++ i) {
    const TpgNode* inode = node->fanin(i);
    NodeList* node_list1 = justify(inode, time);
    list_merge(node_list, node_list1);
  }
  return node_list;
}

// @brief 指定した値を持つのファンインに対して justify() を呼ぶ．
// @param[in] node 対象のノード
// @param[in] time タイムフレーム ( 0 or 1 )
// @param[in] val 値
BtJust2::NodeList*
BtJust2::just_one(const TpgNode* node,
		  int time,
		  Val3 val)
{
  ymuint ni = node->fanin_num();
  // まず gval と fval が等しい場合を探す．
  ymuint pos = ni;
  ymuint min = 0;
  for (ymuint i = 0; i < ni; ++ i) {
    const TpgNode* inode = node->fanin(i);
    Val3 igval = gval(inode, time);
    Val3 ifval = fval(inode, time);
    if ( igval != ifval || igval != val ) {
      continue;
    }
    NodeList* node_list1 = justify(inode, time);
    ymuint n = list_size(node_list1);
    if ( min == 0 || min > n ) {
      pos = i;
      min = n;
    }
  }
  if ( pos < ni ) {
    NodeList*& node_list = mJustArray[node->id() * 2 + time];
    list_merge(node_list, mJustArray[node->fanin(pos)->id() * 2 + time]);
    return node_list;
  }

  // 次に gval と fval が異なる場合を探す．
  ymuint gpos = ni;
  ymuint fpos = ni;
  ymuint gmin = -1;
  ymuint fmin = -1;
  for (ymuint i = 0; i < ni; ++ i) {
    const TpgNode* inode = node->fanin(i);
    Val3 igval = gval(inode, time);
    Val3 ifval = fval(inode, time);
    if ( igval != val && ifval != val ) {
      continue;
    }
    NodeList* node_list1 = justify(inode, time);
    ymuint n = list_size(node_list1);
    if ( igval == val ) {
      if ( gmin > n ) {
	gpos = i;
	gmin = n;
      }
    }
    if ( ifval == val ) {
      if ( fmin == 0 || fmin > n ) {
	fpos = i;
	fmin = n;
      }
    }
  }
  ASSERT_COND( gpos < ni );
  ASSERT_COND( fpos < ni );
  ASSERT_COND( gpos != fpos );
  NodeList*& node_list = mJustArray[node->id() * 2 + time];
  list_merge(node_list, mJustArray[node->fanin(gpos)->id() * 2 + time]);
  list_merge(node_list, mJustArray[node->fanin(fpos)->id() * 2 + time]);

  return node_list;
}

// @brief 新しいリストのセルを返す．
BtJust2::NodeList*
BtJust2::new_list_cell(const TpgNode* node,
		       int time)
{
  void* p = mAlloc.get_memory(sizeof(NodeList));
  NodeList* tmp = new (p) NodeList;
  tmp->mNode = node;
  tmp->mTime = time;
  tmp->mLink = nullptr;
  return tmp;
}

// @brief リストをマージする．
void
BtJust2::list_merge(NodeList*& dst_list,
		    NodeList* src_list)
{
  NodeList** pdst = &dst_list;
  NodeList* src = src_list;
  while ( *pdst != nullptr && src != nullptr ) {
    int r = list_compare((*pdst), src);
    if ( r < 0 ) {
      pdst = &(*pdst)->mLink;
    }
    else if ( r > 0 ) {
      NodeList* tmp = new_list_cell(src->mNode, src->mTime);
      NodeList* next = *pdst;
      *pdst = tmp;
      tmp->mLink = next;
      pdst = &tmp->mLink;
      src = src->mLink;
    }
    else {
      pdst = &(*pdst)->mLink;
      src = src->mLink;
    }
  }
  for ( ; src != nullptr; src = src->mLink) {
    NodeList* tmp = new_list_cell(src->mNode, src->mTime);
    *pdst = tmp;
    pdst = &tmp->mLink;
  }
}

// @brief リストのサイズを返す．
ymuint
BtJust2::list_size(NodeList* node_list)
{
  ymuint n = 0;
  for (NodeList* tmp = node_list; tmp; tmp = tmp->mLink) {
    ++ n;
  }
  return n;
}

// @brief リストを削除する．
void
BtJust2::list_free(NodeList* node_list)
{
  for (NodeList* tmp = node_list; tmp; ) {
    NodeList* next = tmp->mLink;
    mAlloc.put_memory(sizeof(NodeList), tmp);
    tmp = next;
  }
}

// ２つのセルを比較する．
int
BtJust2::list_compare(const NodeList* left,
		      const NodeList* right)
{
  if ( left->mNode->id() < right->mNode->id() ) {
    return -1;
  }
  if ( left->mNode->id() > right->mNode->id() ) {
    return 1;
  }
  if ( left->mTime < right->mTime ) {
    return -1;
  }
  if ( left->mTime > right->mTime ) {
    return 1;
  }
  return 0;
}

END_NAMESPACE_YM_SATPG