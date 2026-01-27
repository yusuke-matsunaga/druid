
/// @File GateRep.cc
/// @brief GateRep の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "GateRep.h"
#include "Gate_Primitive.h"
#include "Gate_Cplx.h"
#include "NodeRep.h"
#include "types/Fval2.h"
#include "types/FaultType.h"
#include "GateType.h"
#include "FaultRep.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス GateRep
//////////////////////////////////////////////////////////////////////

// @brief プリミティブ型のインスタンスを生成するクラスメソッド
GateRep*
GateRep::new_primitive(
  SizeType id,
  const GateType* gate_type,
  const NodeRep* node,
  FaultType fault_type
)
{
  return new Gate_Primitive(id, gate_type, node, fault_type);
}

// @brief 複合ゲート型のインスタンスを生成するクラスメソッド
GateRep*
GateRep::new_cplx(
  SizeType id,
  const GateType* gate_type,
  const NodeRep* node,
  const std::vector<BranchInfo>& branch_info,
  FaultType fault_type
)
{
  return new Gate_Cplx(id, gate_type, node, branch_info, fault_type);
}

BEGIN_NONAMESPACE

inline
SizeType
fault_size(
  const GateType* gate_type,
  FaultType fault_type
)
{
  SizeType ni = gate_type->input_num();
  if ( fault_type == FaultType::GateExhaustive ) {
    return 1 << ni;
  }
  else {
    return ni * 2 + 2;
  }
}

END_NONAMESPACE

// @brief コンストラクタ
GateRep::GateRep(
  SizeType id,
  const GateType* gate_type,
  FaultType fault_type
) : mId{id},
    mGateType{gate_type},
    mFaultArray(fault_size(gate_type, fault_type))
{
}

// @brief 名前を返す．
std::string
GateRep::name() const
{
  std::ostringstream buf;
  if ( is_ppi() ) {
    auto node = output_node();
    buf << "I#" << node->input_id();
  }
  else if ( is_ppo() ) {
    auto node = output_node();
    buf << "O#" << node->output_id();
  }
  else {
    buf << "G#" << id();
  }
  return buf.str();
}

// @brief 入力に対応するノードを返す．
const NodeRep*
GateRep::input_node(
  SizeType pos
) const
{
  const auto& bi = branch_info(pos);
  auto node = bi.node;
  auto ipos = bi.ipos;
  return node->fanin(ipos);
}

// @brief PPI のときに true を返す．
bool
GateRep::is_ppi() const
{
  return mGateType->is_ppi();
}

// @brief PPO のときに true を返す．
bool
GateRep::is_ppo() const
{
  return mGateType->is_ppo();
}

// @brief 組み込みタイプのときに true を返す．
bool
GateRep::is_simple() const
{
  return mGateType->is_primitive();
}

// @brief 論理式タイプのときに true を返す．
bool
GateRep::is_complex() const
{
  return mGateType->is_expr();
}

// @brief ゲートタイプを返す．
PrimType
GateRep::primitive_type() const
{
  return mGateType->primitive_type();
}

// @brief 論理式を返す．
Expr
GateRep::expr() const
{
  return mGateType->expr();
}

// @brief 追加ノード数を返す．
SizeType
GateRep::extra_node_num() const
{
  return mGateType->extra_node_num();
}

// @brief 制御値を返す．
Val3
GateRep::cval(
  SizeType pos,
  Val3 val
) const
{
  return mGateType->cval(pos, val);
}


//////////////////////////////////////////////////////////////////////
// クラス Gate_Primivie
//////////////////////////////////////////////////////////////////////

// @brief 出力に対応するノードを返す．
const NodeRep*
Gate_Primitive::output_node() const
{
  return mNode;
}

// @brief 入力数を返す．
SizeType
Gate_Primitive::input_num() const
{
  return mNode->fanin_num();
}

// @brief ブランチの情報を返す．
GateRep::BranchInfo
Gate_Primitive::branch_info(
  SizeType pos
) const
{
  if ( pos >= input_num() ) {
    throw std::out_of_range{"pos is out of range"};
  }
  return BranchInfo{mNode, pos};
}


//////////////////////////////////////////////////////////////////////
// クラス Gate_Cplx
//////////////////////////////////////////////////////////////////////

// @brief 出力に対応するノードを返す．
const NodeRep*
Gate_Cplx::output_node() const
{
  return mOutputNode;
}

// @brief 入力数を返す．
SizeType
Gate_Cplx::input_num() const
{
  return mBranchInfoList.size();
}

// @brief ブランチの情報を返す．
GateRep::BranchInfo
Gate_Cplx::branch_info(
  SizeType pos
) const
{
  if ( pos >= input_num() ) {
    throw std::out_of_range{"pos is out of range"};
  }
  return mBranchInfoList[pos];
}

END_NAMESPACE_DRUID
