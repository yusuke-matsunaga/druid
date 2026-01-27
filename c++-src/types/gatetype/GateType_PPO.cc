
/// @file GateType_PPO.cc
/// @brief GateType_PPO の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "GateType_PPO.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス GateType
//////////////////////////////////////////////////////////////////////

// @brief 出力型のオブジェクトを作る．
GateType*
GateType::new_ppo(
  SizeType id
)
{
  return new GateType_PPO(id);
}


//////////////////////////////////////////////////////////////////////
// クラス GateType_PPO
//////////////////////////////////////////////////////////////////////

// @brief 入力数を返す．
SizeType
GateType_PPO::input_num() const
{
  return 1;
}

// @brief PPO のときに true を返す．
bool
GateType_PPO::is_ppo() const
{
  return true;
}

END_NAMESPACE_DRUID
