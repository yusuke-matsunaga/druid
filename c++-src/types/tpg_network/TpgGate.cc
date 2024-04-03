
/// @File TpgGate.cc
/// @brief TpgGate の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "TpgGate.h"
#include "TpgGateImpl.h"
#include "TpgNode.h"
#include "Fval2.h"
#include "GateType.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス TpgGate
//////////////////////////////////////////////////////////////////////

// @brief 名前を返す．
string
TpgGate::name() const
{
  ostringstream buf;
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
const TpgNode*
TpgGate::input_node(
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
TpgGate::is_ppi() const
{
  return mGateType->is_ppi();
}

// @brief PPO のときに true を返す．
bool
TpgGate::is_ppo() const
{
  return mGateType->is_ppo();
}

// @brief 組み込みタイプのときに true を返す．
bool
TpgGate::is_simple() const
{
  return mGateType->is_simple();
}

// @brief 論理式タイプのときに true を返す．
bool
TpgGate::is_complex() const
{
  return mGateType->is_complex();
}

// @brief ゲートタイプを返す．
PrimType
TpgGate::primitive_type() const
{
  return mGateType->primitive_type();
}

// @brief 論理式を返す．
Expr
TpgGate::expr() const
{
  return mGateType->expr();
}

// @brief 追加ノード数を返す．
SizeType
TpgGate::extra_node_num() const
{
  return mGateType->extra_node_num();
}

// @brief 制御値を返す．
Val3
TpgGate::cval(
  SizeType pos,
  Val3 val
) const
{
  return mGateType->cval(pos, val);
}


//////////////////////////////////////////////////////////////////////
// クラス TpgGate_Simple
//////////////////////////////////////////////////////////////////////

// @brief 出力に対応するノードを返す．
const TpgNode*
TpgGate_Simple::output_node() const
{
  return mNode;
}

// @brief 入力数を返す．
SizeType
TpgGate_Simple::input_num() const
{
  return mNode->fanin_num();
}

// @brief ブランチの情報を返す．
TpgGate::BranchInfo
TpgGate_Simple::branch_info(
  SizeType pos
) const
{
  ASSERT_COND( 0 <= pos && pos < input_num() );

  return BranchInfo{mNode, pos};
}


//////////////////////////////////////////////////////////////////////
// クラス TpgGate_Cplx
//////////////////////////////////////////////////////////////////////

// @brief 出力に対応するノードを返す．
const TpgNode*
TpgGate_Cplx::output_node() const
{
  return mOutputNode;
}

// @brief 入力数を返す．
SizeType
TpgGate_Cplx::input_num() const
{
  return mBranchInfoList.size();
}

// @brief ブランチの情報を返す．
TpgGate::BranchInfo
TpgGate_Cplx::branch_info(
  SizeType pos
) const
{
  ASSERT_COND( 0 <= pos && pos < input_num() );
  return mBranchInfoList[pos];
}

END_NAMESPACE_DRUID
