
/// @File TpgGate.cc
/// @brief TpgGate の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "types/TpgGate.h"
#include "types/TpgNode.h"
#include "types/TpgFault.h"
#include "GateRep.h"
#include "NodeRep.h"
#include "GateType.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス TpgGate
//////////////////////////////////////////////////////////////////////

// @brief 名前を返す．
std::string
TpgGate::name() const
{
  return _gate()->name();
}

// @brief 出力に対応するノードを返す．
TpgNode
TpgGate::output_node() const
{
  return node(_gate()->output_node());
}

// @brief 入力数を返す．
SizeType
TpgGate::input_num() const
{
  return _gate()->input_num();
}

// @brief 入力に対応するノードを返す．
TpgNode
TpgGate::input_node(
  SizeType pos
) const
{
  return node(_gate()->input_node(pos));
}

// @brief ブランチの情報を返す．
TpgGate::BranchInfo
TpgGate::branch_info(
  SizeType pos
) const
{
  auto bi = _gate()->branch_info(pos);
  return {node(bi.node), bi.ipos};
}

// @brief PPI のときに true を返す．
bool
TpgGate::is_ppi() const
{
  return _gate()->is_ppi();
}

// @brief PPO のときに true を返す．
bool
TpgGate::is_ppo() const
{
  return _gate()->is_ppo();
}

// @brief 組み込みタイプのときに true を返す．
bool
TpgGate::is_simple() const
{
  return _gate()->is_simple();
}

// @brief 論理式タイプのときに true を返す．
bool
TpgGate::is_complex() const
{
  return _gate()->is_complex();
}

// @brief ゲートタイプを返す．
PrimType
TpgGate::primitive_type() const
{
  return _gate()->primitive_type();
}

// @brief 論理式を返す．
Expr
TpgGate::expr() const
{
  return _gate()->expr();
}

// @brief 追加ノード数を返す．
SizeType
TpgGate::extra_node_num() const
{
  return _gate()->extra_node_num();
}

// @brief 制御値を返す．
Val3
TpgGate::cval(
  SizeType pos,
  Val3 val
) const
{
  return _gate()->cval(pos, val);
}

// @brief ステムの故障を返す．
TpgFault
TpgGate::stem_fault(
  Fval2 fval
) const
{
  return TpgBase::fault(_gate()->stem_fault(fval));
}

// @brief ブランチの故障を返す．
TpgFault
TpgGate::branch_fault(
  SizeType ipos,
  Fval2 fval
) const
{
  return TpgBase::fault(_gate()->branch_fault(ipos, fval));
}

// @brief 網羅故障の故障を得る.
TpgFault
TpgGate::ex_fault(
  const std::vector<bool>& ivals
) const
{
  return TpgBase::fault(_gate()->ex_fault(ivals));
}

END_NAMESPACE_DRUID
