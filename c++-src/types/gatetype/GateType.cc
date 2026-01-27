
/// @file GateType.cc
/// @brief GateType の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "GateType.h"
#include "types/Val3.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス GateType
//////////////////////////////////////////////////////////////////////

// @brief PPI のときに true を返す．
bool
GateType::is_ppi() const
{
  return false;
}

// @brief PPO のときに true を返す．
bool
GateType::is_ppo() const
{
  return false;
}

// @brief 組み込みタイプのときに true を返す．
bool
GateType::is_primitive() const
{
  return false;
}

// @brief 論理式タイプのときに true を返す．
bool
GateType::is_expr() const
{
  return false;
}

// @brief ゲートタイプを返す．
PrimType
GateType::primitive_type() const
{
  throw std::invalid_argument{"not a Primitive type"};
}

// @brief 論理式を返す．
Expr
GateType::expr() const
{
  throw std::invalid_argument{"not an Expr type"};
}

// @brief 追加ノード数を返す．
SizeType
GateType::extra_node_num() const
{
  return 0;
}

// @brief 制御値を返す．
Val3
GateType::cval(
  SizeType pos,
  Val3 val
) const
{
  return Val3::_X;
}

END_NAMESPACE_DRUID
