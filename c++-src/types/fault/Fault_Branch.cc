
/// @file FaultRep.cc
/// @brief FaultRep とその継承クラスの実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "Fault_Branch.h"
#include "GateRep.h"
#include "NodeRep.h"
#include "types/FaultType.h"
#include "types/TpgNode.h"
#include "types/Assign.h"
#include "types/Fval2.h"
#include "types/Val3.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス FaultRep
//////////////////////////////////////////////////////////////////////

// @brief ブランチの故障を生成する．
FaultRep*
FaultRep::new_branch_fault(
  SizeType id,
  const GateRep* gate,
  SizeType ipos,
  Fval2 fval,
  FaultType fault_type
)
{
  switch ( fault_type ) {
  case FaultType::StuckAt:
    if ( fval == Fval2::zero ) {
      return new Fault_BranchSa0{id, gate, ipos};
    }
    else {
      return new Fault_BranchSa1{id, gate, ipos};
    }
  case FaultType::TransitionDelay:
    if ( fval == Fval2::zero ) {
      return new Fault_BranchRise{id, gate, ipos};
    }
    else {
      return new Fault_BranchFall{id, gate, ipos};
    }
  default:
    throw std::logic_error{"never be reached"};
  }
  // ダミー
  return nullptr;
}


//////////////////////////////////////////////////////////////////////
// Fault_Branch
//////////////////////////////////////////////////////////////////////

// @brief ステムの故障の時 true を返す．
bool
Fault_Branch::is_stem() const
{
  return false;
}

// @brief ブランチの故障の時の入力位置を返す．
SizeType
Fault_Branch::branch_pos() const
{
  return ipos();
}

// @brief 故障伝搬の起点となるノードを返す．
const NodeRep*
Fault_Branch::origin_node() const
{
  return gate()->branch_info(ipos()).node;
}

// @brief 故障の伝搬条件を追加する．
void
Fault_Branch::add_gate_propagation_condition(
  std::vector<SizeType>& assign_list
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
	assign_list.push_back(Assign::encode(inode->id(), 1, side_val));
      }
    }
  }
}

// @brief 故障の内容を表す文字列の基本部分を返す．
std::string
Fault_Branch::str_base() const
{
  std::ostringstream buf;
  buf << gate()->name() << ":I" << ipos();
  return buf.str();
}


//////////////////////////////////////////////////////////////////////
// Fault_BranchSa
//////////////////////////////////////////////////////////////////////

// @brief 故障の種類を返す．
FaultType
Fault_BranchSa::fault_type() const
{
  return FaultType::StuckAt;
}


//////////////////////////////////////////////////////////////////////
// Fault_BranchSa0
//////////////////////////////////////////////////////////////////////

// @brief 故障値を得る．
Fval2
Fault_BranchSa0::fval() const
{
  return Fval2::zero;
}

// @brief 故障が励起して origin_node の出力まで伝搬する条件を求める．
std::vector<SizeType>
Fault_BranchSa0::excitation_condition() const
{
  std::vector<SizeType> assign_list;
  // 故障の励起条件
  auto inode = gate()->input_node(ipos());
  assign_list.push_back(Assign::encode(inode->id(), 1, true));
  // 故障の伝搬条件
  add_gate_propagation_condition(assign_list);
  return assign_list;
}

// @brief 故障の内容を表す文字列を返す．
std::string
Fault_BranchSa0::str() const
{
  return str_base() + ":SA0";
}


//////////////////////////////////////////////////////////////////////
// Fault_BranchSa1
//////////////////////////////////////////////////////////////////////

// @brief 故障値を得る．
Fval2
Fault_BranchSa1::fval() const
{
  return Fval2::one;
}

// @brief 故障が励起して origin_node の出力まで伝搬する条件を求める．
std::vector<SizeType>
Fault_BranchSa1::excitation_condition() const
{
  std::vector<SizeType> assign_list;
  // 故障の励起条件
  auto inode = gate()->input_node(ipos());
  assign_list.push_back(Assign::encode(inode->id(), 1, false));
  // 故障の伝搬条件
  add_gate_propagation_condition(assign_list);
  return assign_list;
}

// @brief 故障の内容を表す文字列を返す．
std::string
Fault_BranchSa1::str() const
{
  return str_base() + ":SA1";
}


//////////////////////////////////////////////////////////////////////
// Fault_BranchTd
//////////////////////////////////////////////////////////////////////

// @brief 故障の種類を返す．
FaultType
Fault_BranchTd::fault_type() const
{
  return FaultType::TransitionDelay;
}


//////////////////////////////////////////////////////////////////////
// Fault_BranchRise
//////////////////////////////////////////////////////////////////////

// @brief 故障値を得る．
Fval2
Fault_BranchRise::fval() const
{
  return Fval2::zero;
}

/// @brief 故障が励起して origin_node の出力まで伝搬する条件を求める．
std::vector<SizeType>
Fault_BranchRise::excitation_condition() const
{
  std::vector<SizeType> assign_list;
  // 故障の励起条件
  // 0 -> 1 の遷移を起こす．
  auto inode = gate()->input_node(ipos());
  assign_list.push_back(Assign::encode(inode->id(), 0, false));
  assign_list.push_back(Assign::encode(inode->id(), 1, true));
  // 故障の伝搬条件
  add_gate_propagation_condition(assign_list);
  return assign_list;
}

// @brief 故障の内容を表す文字列を返す．
std::string
Fault_BranchRise::str() const
{
  return str_base() + ":RISE";
}


//////////////////////////////////////////////////////////////////////
// Fault_BranchFall
//////////////////////////////////////////////////////////////////////

// @brief 故障値を得る．
Fval2
Fault_BranchFall::fval() const
{
  return Fval2::one;
}

/// @brief 故障が励起して origin_node の出力まで伝搬する条件を求める．
std::vector<SizeType>
Fault_BranchFall::excitation_condition() const
{
  std::vector<SizeType> assign_list;
  // 故障の励起条件
  // 0 -> 1 の遷移を起こす．
  auto inode = gate()->input_node(ipos());
  assign_list.push_back(Assign::encode(inode->id(), 0, true));
  assign_list.push_back(Assign::encode(inode->id(), 1, false));
  // 故障の伝搬条件
  add_gate_propagation_condition(assign_list);
  return assign_list;
}

// @brief 故障の内容を表す文字列を返す．
std::string
Fault_BranchFall::str() const
{
  return str_base() + ":FALL";
}

END_NAMESPACE_DRUID
