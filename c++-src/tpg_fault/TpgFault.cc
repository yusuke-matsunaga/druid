
/// @file TpgFault.cc
/// @brief TpgFault の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "TpgFault.h"
#include "TpgFaultImpl.h"
#include "TpgFaultMgrImpl.h"
#include "TpgNode.h"
#include "NodeValList.h"
#include "Fval2.h"
#include "Val3.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス TpgFault
//////////////////////////////////////////////////////////////////////

// @brief 故障伝搬の起点となるノードを返す．
const TpgNode*
TpgFault::origin_node() const
{
  ASSERT_COND( mMgr != nullptr );

  auto f = mMgr->_fault(mId);
  return f->origin_node();
}

// @brief 故障が励起して origin_node の出力まで伝搬する条件を求める．
NodeValList
TpgFault::excitation_condition() const
{
  ASSERT_COND( mMgr != nullptr );

  auto f = mMgr->_fault(mId);
  return f->excitation_condition();
}

// @brief origin_node を含む FFR の根のノードを返す．
const TpgNode*
TpgFault::ffr_root() const
{
  return origin_node()->ffr_root();
}

// @brief 故障が励起してFFRの根まで伝搬する条件を求める．
NodeValList
TpgFault::ffr_propagate_condition() const
{
  // ノードの出力に故障が現れる励起条件
  auto assign_list = excitation_condition();

  // FFR の根までの伝搬条件を作る．
  for ( auto node = origin_node(); node->fanout_num() == 1; ) {
    auto fonode = node->fanout(0);
    auto val = fonode->side_val();
    if ( val != Val3::_X ) {
      bool bval = val == Val3::_1 ? true : false;
      for ( auto inode: fonode->fanin_list() ) {
	if ( inode != node ) {
	  assign_list.add(inode, 1, bval);
	}
      }
    }
    node = fonode;
  }
  return assign_list;
}

// @brief 故障の内容を表す文字列を返す．
string
TpgFault::str() const
{
  ASSERT_COND( mMgr != nullptr );

  auto f = mMgr->_fault(mId);
  return f->str();
}


//////////////////////////////////////////////////////////////////////
// TpgFault_SaStem
//////////////////////////////////////////////////////////////////////

// @brief 故障が励起して origin_node の出力まで伝搬する条件を求める．
NodeValList
TpgFault_SaStem::excitation_condition() const
{
  NodeValList assign_list;
  // 0 縮退故障の時に 1 にする．
  assign_list.add(origin_node(), 1, is_sa0(fval()));
  return assign_list;
}


//////////////////////////////////////////////////////////////////////
// TpgFault_SaBranch
//////////////////////////////////////////////////////////////////////

// @brief 故障が励起して origin_node の出力まで伝搬する条件を求める．
NodeValList
TpgFault_SaBranch::excitation_condition() const
{
  NodeValList assign_list;
  auto node = origin_node();
  auto sv = node->side_val();
  SizeType ni = node->fanin_num();
  for ( SizeType i = 0; i < ni; ++ i ) {
    auto inode = node->fanin(i);
    if ( i == ipos() ) {
      assign_list.add(inode, 1, is_sa0(fval()));
    }
    else if ( sv != Val3::_X ) {
      bool bv = sv == Val3::_1 ? true : false;
      assign_list.add(inode, 1, bv);
    }
  }
  return assign_list;
}


//////////////////////////////////////////////////////////////////////
// TpgFault_TdStem
//////////////////////////////////////////////////////////////////////

// @brief 故障が励起して origin_node の出力まで伝搬する条件を求める．
NodeValList
TpgFault_TdStem::excitation_condition() const
{
  NodeValList assign_list;
  // 1時刻め: 0 -> 1 遷移故障の時に 1 にする．
  assign_list.add(origin_node(), 1, is_rise_transition(fval()));
  // 0時刻め: 1 -> 0 遷移故障の時に 1 にする．
  assign_list.add(origin_node(), 0, is_fall_transition(fval()));
  return assign_list;
}


//////////////////////////////////////////////////////////////////////
// TpgFault_TdBranch
//////////////////////////////////////////////////////////////////////

/// @brief 故障が励起して origin_node の出力まで伝搬する条件を求める．
NodeValList
TpgFault_TdBranch::excitation_condition() const
{
  NodeValList assign_list;
  auto node = origin_node();
  auto sv = node->side_val();
  SizeType ni = node->fanin_num();
  for ( SizeType i = 0; i < ni; ++ i ) {
    auto inode = node->fanin(i);
    if ( i == ipos() ) {
      // 1時刻め: 0 -> 1 遷移故障の時に 1 にする．
      assign_list.add(inode, 1, is_rise_transition(fval()));
      // 0時刻め: 1 -> 0 遷移故障の時に 1 にする．
      assign_list.add(inode, 0, is_fall_transition(fval()));
    }
    else if ( sv != Val3::_X ) {
      bool bv = sv == Val3::_1 ? true : false;
      assign_list.add(inode, 1, bv);
    }
  }
  return assign_list;
}


//////////////////////////////////////////////////////////////////////
// TpgFault_Ex
//////////////////////////////////////////////////////////////////////

// @brief 故障が励起して origin_node の出力まで伝搬する条件を求める．
NodeValList
TpgFault_Ex::excitation_condition() const
{
  NodeValList assign_list;
  auto node = origin_node();
  SizeType ni = node->fanin_num();
  for ( SizeType i = 0; i < ni; ++ i ) {
    auto inode = node->fanin(i);
    assign_list.add(inode, 1, mIvals[i]);
  }
  return assign_list;
}

END_NAMESPACE_DRUID
