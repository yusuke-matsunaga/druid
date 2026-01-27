
/// @file GateType_PPI.cc
/// @brief GateType_PPI の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "GateType_PPI.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス GateType
//////////////////////////////////////////////////////////////////////

// @brief 入力型のオブジェクトを作る．
GateType*
GateType::new_ppi(
  SizeType id
)
{
  return new GateType_PPI(id);
}


//////////////////////////////////////////////////////////////////////
// クラス GateType_PPI
//////////////////////////////////////////////////////////////////////

// @brief 入力数を返す．
SizeType
GateType_PPI::input_num() const
{
  return 0;
}

// @brief PPI のときに true を返す．
bool
GateType_PPI::is_ppi() const
{
  return true;
}

END_NAMESPACE_DRUID
