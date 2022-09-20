
/// @file TpgFault.cc
/// @brief TpgFault の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2007, 2012-2014, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "TpgFault.h"
#include "TpgFaultBase.h"
#include "TpgStemFault.h"
#include "TpgBranchFault.h"
#include "TpgNode.h"
#include "FaultType.h"
#include "NodeValList.h"
#include "Val3.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス TpgFault
//////////////////////////////////////////////////////////////////////

// @brief 故障が励起してFFRの根まで伝搬する条件を求める．
NodeValList
TpgFault::ffr_propagate_condition(
  FaultType fault_type
) const
{
  NodeValList assign_list;

  // 故障の活性化条件を作る．
  auto inode = tpg_inode();
  // 0 縮退故障の時に 1 にする．
  bool is_0 = (val() == Fval2::zero);
  assign_list.add(inode, 1, is_0);

  if ( fault_type == FaultType::TransitionDelay ) {
    // 1時刻前の値が逆の値である条件を作る．
    assign_list.add(inode, 0, !is_0);
  }

  // ブランチの故障の場合，ゲートの出力までの伝搬条件を作る．
  if ( is_branch_fault() ) {
    auto onode = tpg_onode();
    SizeType fpos = tpg_pos();
    Val3 nval = onode->nval();
    if ( nval != Val3::_X ) {
      bool val = (nval == Val3::_1);
      for ( auto ipos: Range(onode->fanin_num()) ) {
	if ( ipos != fpos ) {
	  auto inode1 = onode->fanin(ipos);
	  assign_list.add(inode1, 1, val);
	}
      }
    }
  }

  // FFR の根までの伝搬条件を作る．
  for ( auto node = tpg_onode(); node->fanout_num() == 1;
	node = node->fanout_list()[0]) {
    auto fonode = node->fanout_list()[0];
    SizeType ni = fonode->fanin_num();
    if ( ni == 1 ) {
      continue;
    }
    Val3 nval = fonode->nval();
    if ( nval == Val3::_X ) {
      continue;
    }
    bool val = (nval == Val3::_1);
    for ( auto inode1: fonode->fanin_list() ) {
      if ( inode1 != node ) {
	assign_list.add(inode1, 1, val);
      }
    }
  }

  return assign_list;
}


//////////////////////////////////////////////////////////////////////
// クラス TpgFaultBase
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
TpgFaultBase::TpgFaultBase(
  SizeType id,
  Fval2 val,
  const TpgNode* node,
  const string& name,
  TpgFault* rep_fault
) : mTpgNode{node},
    mNodeName{name},
    mRepFault{rep_fault}
{
  mIdVal = (id << 1) | static_cast<SizeType>(val);
}

// @brief デストラクタ
TpgFaultBase::~TpgFaultBase()
{
}

// @brief ID番号を返す．
SizeType
TpgFaultBase::id() const
{
  return static_cast<int>(mIdVal >> 1);
}

// @brief 故障値を返す．
Fval2
TpgFaultBase::val() const
{
  if ( mIdVal & 1 ) {
    return Fval2::one;
  }
  else {
    return Fval2::zero;
  }
}

// @brief 代表故障を返す．
const TpgFault*
TpgFaultBase::rep_fault() const
{
  return mRepFault;
}

// @brief 代表故障を設定する．
void
TpgFaultBase::set_rep(
  const TpgFault* rep
)
{
  mRepFault = rep;
}


//////////////////////////////////////////////////////////////////////
// クラス TpgStemFault
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
TpgStemFault::TpgStemFault(
  SizeType id,
  Fval2 val,
  const TpgNode* node,
  const string& name,
  TpgFault* rep_fault
) : TpgFaultBase{id, val, node, name, rep_fault}
{
  ASSERT_COND( tpg_inode() != nullptr );
}

// @brief デストラクタ
TpgStemFault::~TpgStemFault()
{
}

// @brief 故障の入力側の TpgNode を返す．
const TpgNode*
TpgStemFault::tpg_inode() const
{
  return tpg_node();
}

// @brief 故障の出力側の TpgNode を返す．
//
// is_branch_fault() == true の時のみ意味を持つ．
const TpgNode*
TpgStemFault::tpg_onode() const
{
  return tpg_node();
}

// @brief ステムの故障の時 true を返す．
bool
TpgStemFault::is_stem_fault() const
{
  return true;
}

// @brief ブランチの入力位置を返す．
SizeType
TpgStemFault::fault_pos() const
{
  ASSERT_NOT_REACHED;

  return 0;
}

// @brief tpg_inode 上の故障位置を返す．
//
// is_input_fault() == true の時のみ意味を持つ．
SizeType
TpgStemFault::tpg_pos() const
{
  ASSERT_NOT_REACHED;

  return 0;
}

// @brief 故障の内容を表す文字列を返す．
string
TpgStemFault::str() const
{
  ostringstream ans;
  ans << node_name() << ":O:" << val();
  return ans.str();
}


//////////////////////////////////////////////////////////////////////
// クラス TpgBranchFault
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
TpgBranchFault::TpgBranchFault(
  SizeType id,
  Fval2 val,
  const TpgNode* onode,
  const string& name,
  SizeType pos,
  const TpgNode* inode,
  SizeType tpg_pos,
  TpgFault* rep_fault
) : TpgFaultBase{id, val, onode, name, rep_fault},
    mPos{pos},
    mInode{inode},
    mTpgPos{tpg_pos}
{
  ASSERT_COND( tpg_onode() != nullptr );
  ASSERT_COND( tpg_inode() != nullptr );
}

// @brief デストラクタ
TpgBranchFault::~TpgBranchFault()
{
}

// @brief 故障の入力側の TpgNode を返す．
const TpgNode*
TpgBranchFault::tpg_inode() const
{
  return mInode;
}

// @brief 故障の出力側の TpgNode を返す．
const TpgNode*
TpgBranchFault::tpg_onode() const
{
  return tpg_node();
}

// @brief ステムの故障の時 true を返す．
bool
TpgBranchFault::is_stem_fault() const
{
  return false;
}

// @brief ブランチの入力位置を返す．
//
// is_branch_fault() == true の時のみ意味を持つ．
SizeType
TpgBranchFault::fault_pos() const
{
  return mPos;
}

// @brief tpg_inode 上の故障位置を返す．
//
// is_branch_fault() == true の時のみ意味を持つ．
SizeType
TpgBranchFault::tpg_pos() const
{
  return mTpgPos;
}

// @brief 故障の内容を表す文字列を返す．
string
TpgBranchFault::str() const
{
  ostringstream ans;
  ans << node_name() << ":I" << fault_pos() << ":" << val();
  return ans.str();
}

END_NAMESPACE_DRUID
