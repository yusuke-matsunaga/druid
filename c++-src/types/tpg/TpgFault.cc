
/// @file TpgFault.cc
/// @brief TpgFault の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "types/TpgFault.h"
#include "types/TpgGate.h"
#include "FaultRep.h"
#include "GateRep.h"
#include "NodeRep.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス TpgFault
//////////////////////////////////////////////////////////////////////

// @brief 対象のゲートを返す．
TpgGate
TpgFault::gate() const
{
  return TpgBase::gate(_fault()->gate());
}

// @brief 故障の種類を返す．
FaultType
TpgFault::fault_type() const
{
  return _fault()->fault_type();
}

// @brief 故障値を返す．
Fval2
TpgFault::fval() const
{
  return _fault()->fval();
}

// @brief ステムの故障の時 true を返す．
bool
TpgFault::is_stem() const
{
  return _fault()->is_stem();
}

// @brief ブランチの故障の時 true を返す．
bool
TpgFault::is_branch() const
{
  return _fault()->is_branch();
}

// @brief ブランチの故障の時の入力位置を返す．
SizeType
TpgFault::branch_pos() const
{
  return _fault()->branch_pos();
}

// @brief 網羅故障の場合の入力値のベクトルを返す．
std::vector<bool>
TpgFault::input_vals() const
{
  return _fault()->input_vals();
}

// @brief 代表故障を返す．
TpgFault
TpgFault::rep_fault() const
{
  return fault(_fault()->rep_fault());
}

// @brief 故障の内容を表す文字列を返す．
std::string
TpgFault::str() const
{
  return _fault()->str();
}

// @brief 故障伝搬の起点となるノードを返す．
TpgNode
TpgFault::origin_node() const
{
  return node(_fault()->origin_node());
}

// @brief 故障が励起して origin_node の出力まで伝搬する条件を求める．
AssignList
TpgFault::excitation_condition() const
{
  return AssignList(_network(), _fault()->excitation_condition());
}

// @brief origin_node を含む FFR の根のノードを返す．
TpgNode
TpgFault::ffr_root() const
{
  return node(_fault()->ffr_root());
}

// @brief 故障が励起してFFRの根まで伝搬する条件を求める．
AssignList
TpgFault::ffr_propagate_condition() const
{
  return AssignList(_network(), _fault()->ffr_propagate_condition());
}

// @brief ハッシュ用の値を返す．
SizeType
TpgFault::hash() const
{
  return _fault()->hash();
}

END_NAMESPACE_DRUID
