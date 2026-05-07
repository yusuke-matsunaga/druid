
/// @file SnBuff.cc
/// @brief SnBuff の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "SnBuff.h"


BEGIN_NAMESPACE_DRUID_FSIM

//////////////////////////////////////////////////////////////////////
// SnBuff
//////////////////////////////////////////////////////////////////////

// @brief ゲートタイプを返す．
PrimType
SnBuff::gate_type() const
{
  return PrimType::Buff;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnBuff::_calc_val(
  const std::vector<FSIM_VALTYPE>& val_array
) const
{
  return val_array[_fanin()];
}

// @brief ゲートの入力から出力までの可観測性を計算する．
PackedVal
SnBuff::_calc_gobs(
  const std::vector<FSIM_VALTYPE>& val_array,
  SizeType ipos
) const
{
  return PV_ALL1;
}


//////////////////////////////////////////////////////////////////////
// SnNot
//////////////////////////////////////////////////////////////////////

// @brief ゲートタイプを返す．
PrimType
SnNot::gate_type() const
{
  return PrimType::Not;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnNot::_calc_val(
  const std::vector<FSIM_VALTYPE>& val_array
) const
{
  return ~val_array[_fanin()];
}

END_NAMESPACE_DRUID_FSIM
