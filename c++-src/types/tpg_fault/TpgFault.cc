
/// @file TpgFault.cc
/// @brief TpgFault の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023, 2024 Yusuke Matsunaga
/// All rights reserved.

#include "TpgFault.h"
#include "TpgFaultImpl.h"
#include "FaultType.h"
#include "TpgGate.h"
#include "TpgNode.h"
#include "NodeValList.h"
#include "Fval2.h"
#include "Val3.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス TpgFault
//////////////////////////////////////////////////////////////////////

// @brief ステムの故障を生成する．
TpgFault*
TpgFault::new_fault(
  SizeType id,
  const TpgGate* gate,
  Fval2 fval,
  FaultType fault_type
)
{
  switch ( fault_type ) {
  case FaultType::StuckAt:
    if ( fval == Fval2::zero ) {
      return new TpgFault_StemSa0{id, gate};
    }
    else {
      return new TpgFault_StemSa1{id, gate};
    }
  case FaultType::TransitionDelay:
    if ( fval == Fval2::one ) {
      return new TpgFault_StemRise{id, gate};
    }
    else {
      return new TpgFault_StemFall{id, gate};
    }
  default:
    ASSERT_NOT_REACHED;
  }
  // ダミー
  return nullptr;
}

// @brief ブランチの故障を生成する．
TpgFault*
TpgFault::new_fault(
  SizeType id,
  const TpgGate* gate,
  SizeType ipos,
  Fval2 fval,
  FaultType fault_type
)
{
  switch ( fault_type ) {
  case FaultType::StuckAt:
    if ( fval == Fval2::zero ) {
      return new TpgFault_BranchSa0{id, gate, ipos};
    }
    else {
      return new TpgFault_BranchSa1{id, gate, ipos};
    }
  case FaultType::TransitionDelay:
    if ( fval == Fval2::zero ) {
      return new TpgFault_BranchRise{id, gate, ipos};
    }
    else {
      return new TpgFault_BranchFall{id, gate, ipos};
    }
  default:
    ASSERT_NOT_REACHED;
  }
  // ダミー
  return nullptr;
}

// @brief ゲート網羅故障を生成する．
TpgFault*
TpgFault::new_fault(
  SizeType id,
  const TpgGate* gate,
  const vector<bool>& ivals
)
{
  return new TpgFault_Ex{id, gate, ivals};
}

// @brief ステムの故障の時 true を返す．
bool
TpgFault::is_stem() const
{
  return true;
}

// @brief ブランチの故障の時の入力位置を返す．
SizeType
TpgFault::branch_pos() const
{
  ASSERT_NOT_REACHED;

  return 0;
}

// @brief 網羅故障の場合の入力値のベクトルを返す．
vector<bool>
TpgFault::input_vals() const
{
  ASSERT_NOT_REACHED;

  return vector<bool>{};
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
    auto val = fonode->nval();
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

// @brief ハッシュ用の値を返す．
SizeType
TpgFault::hash() const
{
  return id();
}


//////////////////////////////////////////////////////////////////////
// TpgFault_Stem
//////////////////////////////////////////////////////////////////////

// @brief 故障伝搬の起点となるノードを返す．
const TpgNode*
TpgFault_Stem::origin_node() const
{
  return gate()->output_node();
}

// @brief 故障の内容を表す文字列の基本部分を返す．
string
TpgFault_Stem::str_base() const
{
  return gate()->name() + ":O";
}


//////////////////////////////////////////////////////////////////////
// TpgFault_StemSa
//////////////////////////////////////////////////////////////////////

// @brief 故障の種類を返す．
FaultType
TpgFault_StemSa::fault_type() const
{
  return FaultType::StuckAt;
}


//////////////////////////////////////////////////////////////////////
// TpgFault_StemSa0
//////////////////////////////////////////////////////////////////////

// @brief 故障値を得る．
Fval2
TpgFault_StemSa0::fval() const
{
  return Fval2::zero;
}

// @brief 故障が励起して origin_node の出力まで伝搬する条件を求める．
NodeValList
TpgFault_StemSa0::excitation_condition() const
{
  NodeValList assign_list;
  // 0 縮退故障なので 1 にする．
  assign_list.add(origin_node(), 1, true);
  return assign_list;
}

// @brief 故障の内容を表す文字列を返す．
string
TpgFault_StemSa0::str() const
{
  return str_base() + ":SA0";
}


//////////////////////////////////////////////////////////////////////
// TpgFault_StemSa1
//////////////////////////////////////////////////////////////////////

// @brief 故障値を得る．
Fval2
TpgFault_StemSa1::fval() const
{
  return Fval2::one;
}

// @brief 故障が励起して origin_node の出力まで伝搬する条件を求める．
NodeValList
TpgFault_StemSa1::excitation_condition() const
{
  NodeValList assign_list;
  // 1 縮退故障なので 0 にする．
  assign_list.add(origin_node(), 1, false);
  return assign_list;
}

// @brief 故障の内容を表す文字列を返す．
string
TpgFault_StemSa1::str() const
{
  return str_base() + ":SA1";
}


//////////////////////////////////////////////////////////////////////
// TpgFault_Branch
//////////////////////////////////////////////////////////////////////

// @brief ステムの故障の時 true を返す．
bool
TpgFault_Branch::is_stem() const
{
  return false;
}

// @brief ブランチの故障の時の入力位置を返す．
SizeType
TpgFault_Branch::branch_pos() const
{
  return ipos();
}

// @brief 故障伝搬の起点となるノードを返す．
const TpgNode*
TpgFault_Branch::origin_node() const
{
  return gate()->branch_info(ipos()).node;
}

// @brief 故障の伝搬条件を追加する．
void
TpgFault_Branch::add_gate_propagation_condition(
  NodeValList& assign_list
) const
{
  const auto& bi = gate()->branch_info(ipos());
  auto node = bi.node;
  auto ipos = bi.ipos;
  if ( node->nval() != Val3::_X ) {
    // 故障の伝搬条件
    SizeType ni = node->fanin_num();
    bool side_val = node->nval() == Val3::_1;
    for ( SizeType i = 0; i < ni; ++ i ) {
      if ( i != ipos ) {
	auto inode = node->fanin(i);
	assign_list.add(inode, 1, side_val);
      }
    }
  }
}

// @brief 故障の内容を表す文字列の基本部分を返す．
string
TpgFault_Branch::str_base() const
{
  ostringstream buf;
  buf << gate()->name() << ":I" << ipos();
  return buf.str();
}


//////////////////////////////////////////////////////////////////////
// TpgFault_BranchSa
//////////////////////////////////////////////////////////////////////

// @brief 故障の種類を返す．
FaultType
TpgFault_BranchSa::fault_type() const
{
  return FaultType::StuckAt;
}


//////////////////////////////////////////////////////////////////////
// TpgFault_BranchSa0
//////////////////////////////////////////////////////////////////////

// @brief 故障値を得る．
Fval2
TpgFault_BranchSa0::fval() const
{
  return Fval2::zero;
}

// @brief 故障が励起して origin_node の出力まで伝搬する条件を求める．
NodeValList
TpgFault_BranchSa0::excitation_condition() const
{
  NodeValList assign_list;
  // 故障の励起条件
  auto inode = gate()->input_node(ipos());
  assign_list.add(inode, 1, true);
  // 故障の伝搬条件
  add_gate_propagation_condition(assign_list);
  return assign_list;
}

// @brief 故障の内容を表す文字列を返す．
string
TpgFault_BranchSa0::str() const
{
  return str_base() + ":SA0";
}


//////////////////////////////////////////////////////////////////////
// TpgFault_BranchSa1
//////////////////////////////////////////////////////////////////////

// @brief 故障値を得る．
Fval2
TpgFault_BranchSa1::fval() const
{
  return Fval2::one;
}

// @brief 故障が励起して origin_node の出力まで伝搬する条件を求める．
NodeValList
TpgFault_BranchSa1::excitation_condition() const
{
  NodeValList assign_list;
  // 故障の励起条件
  auto inode = gate()->input_node(ipos());
  assign_list.add(inode, 1, false);
  // 故障の伝搬条件
  add_gate_propagation_condition(assign_list);
  return assign_list;
}

// @brief 故障の内容を表す文字列を返す．
string
TpgFault_BranchSa1::str() const
{
  return str_base() + ":SA1";
}


//////////////////////////////////////////////////////////////////////
// TpgFault_StemTd
//////////////////////////////////////////////////////////////////////

// @brief 故障の種類を返す．
FaultType
TpgFault_StemTd::fault_type() const
{
  return FaultType::TransitionDelay;
}


//////////////////////////////////////////////////////////////////////
// TpgFault_StemRise
//////////////////////////////////////////////////////////////////////

// @brief 故障値を得る．
Fval2
TpgFault_StemRise::fval() const
{
  return Fval2::zero;
}

// @brief 故障が励起して origin_node の出力まで伝搬する条件を求める．
NodeValList
TpgFault_StemRise::excitation_condition() const
{
  // 0 -> 1 の遷移を起こす．
  NodeValList assign_list;
  assign_list.add(origin_node(), 0, false);
  assign_list.add(origin_node(), 1, true);
  return assign_list;
}

// @brief 故障の内容を表す文字列を返す．
string
TpgFault_StemRise::str() const
{
  return str_base() + ":RISE";
}


//////////////////////////////////////////////////////////////////////
// TpgFault_StemFall
//////////////////////////////////////////////////////////////////////

// @brief 故障値を得る．
Fval2
TpgFault_StemFall::fval() const
{
  return Fval2::one;
}

// @brief 故障が励起して origin_node の出力まで伝搬する条件を求める．
NodeValList
TpgFault_StemFall::excitation_condition() const
{
  // 1 -> 0 の遷移を起こす．
  NodeValList assign_list;
  assign_list.add(origin_node(), 0, true);
  assign_list.add(origin_node(), 1, false);
  return assign_list;
}

// @brief 故障の内容を表す文字列を返す．
string
TpgFault_StemFall::str() const
{
  return str_base() + ":FALL";
}


//////////////////////////////////////////////////////////////////////
// TpgFault_BranchTd
//////////////////////////////////////////////////////////////////////

// @brief 故障の種類を返す．
FaultType
TpgFault_BranchTd::fault_type() const
{
  return FaultType::TransitionDelay;
}


//////////////////////////////////////////////////////////////////////
// TpgFault_BranchRise
//////////////////////////////////////////////////////////////////////

// @brief 故障値を得る．
Fval2
TpgFault_BranchRise::fval() const
{
  return Fval2::zero;
}

/// @brief 故障が励起して origin_node の出力まで伝搬する条件を求める．
NodeValList
TpgFault_BranchRise::excitation_condition() const
{
  NodeValList assign_list;
  // 故障の励起条件
  // 0 -> 1 の遷移を起こす．
  auto inode = gate()->input_node(ipos());
  assign_list.add(inode, 0, false);
  assign_list.add(inode, 1, true);
  // 故障の伝搬条件
  add_gate_propagation_condition(assign_list);
  return assign_list;
}

// @brief 故障の内容を表す文字列を返す．
string
TpgFault_BranchRise::str() const
{
  return str_base() + ":RISE";
}


//////////////////////////////////////////////////////////////////////
// TpgFault_BranchFall
//////////////////////////////////////////////////////////////////////

// @brief 故障値を得る．
Fval2
TpgFault_BranchFall::fval() const
{
  return Fval2::one;
}

/// @brief 故障が励起して origin_node の出力まで伝搬する条件を求める．
NodeValList
TpgFault_BranchFall::excitation_condition() const
{
  NodeValList assign_list;
  // 故障の励起条件
  // 0 -> 1 の遷移を起こす．
  auto inode = gate()->input_node(ipos());
  assign_list.add(inode, 0, true);
  assign_list.add(inode, 1, false);
  // 故障の伝搬条件
  add_gate_propagation_condition(assign_list);
  return assign_list;
}

// @brief 故障の内容を表す文字列を返す．
string
TpgFault_BranchFall::str() const
{
  return str_base() + ":FALL";
}


//////////////////////////////////////////////////////////////////////
// TpgFault_Ex
//////////////////////////////////////////////////////////////////////

// @brief 故障の種類を返す．
FaultType
TpgFault_Ex::fault_type() const
{
  return FaultType::GateExhaustive;
}

// @brief 故障値を返す．
Fval2
TpgFault_Ex::fval() const
{
  ASSERT_NOT_REACHED;
  return Fval2::zero;
}

// @brief 故障伝搬の起点となるノードを返す．
const TpgNode*
TpgFault_Ex::origin_node() const
{
  return gate()->output_node();
}

// @brief 故障が励起して origin_node の出力まで伝搬する条件を求める．
NodeValList
TpgFault_Ex::excitation_condition() const
{
  NodeValList assign_list;
  SizeType ni = gate()->input_num();
  for ( SizeType i = 0; i < ni; ++ i ) {
    auto inode = gate()->input_node(i);
    assign_list.add(inode, 1, mIvals[i]);
  }
  return assign_list;
}

// @brief 故障の内容を表す文字列を返す．
string
TpgFault_Ex::str() const
{
  ostringstream buf;
  buf << gate()->name() << ":EX";
  SizeType ni = gate()->input_num();
  for ( SizeType i = 0; i < ni; ++ i ) {
    buf << mIvals[i];
  }
  return buf.str();
}

END_NAMESPACE_DRUID
