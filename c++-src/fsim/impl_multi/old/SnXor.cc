
/// @file SnXor.cc
/// @brief SnXor の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "SnXor.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID_FSIM

//////////////////////////////////////////////////////////////////////
// SnXor
//////////////////////////////////////////////////////////////////////

// @brief ゲートタイプを返す．
PrimType
SnXor::gate_type() const
{
  return PrimType::Xor;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnXor::_calc_val(
  const std::vector<FSIM_VALTYPE>& val_array
) const
{
  return _calc_xor(val_array);
}

// @brief ゲートの入力から出力までの可観測性を計算する．(3値版)
PackedVal
SnXor::_calc_gobs(
  const std::vector<FSIM_VALTYPE>& val_array,
  SizeType ipos
) const
{
#if FSIM_VAL2
  // 2値なら常に観測可能
  return PV_ALL1;
#elif FSIM_VAL3
  // 3値はめんどくさい
  // 条件は ipos 以外が X でないこと
  auto obs = PV_ALL1;
  for ( auto i: Range(0, ipos) ) {
    auto ival = val_array[_fanin(i)];
    obs &= ival.val01();
  }
  for ( auto i: Range(ipos + 1, _fanin_num()) ) {
    auto ival = val_array[_fanin(i)];
    obs &= ival.val01();
  }
  return obs;
#endif
}


//////////////////////////////////////////////////////////////////////
// SnXor2
//////////////////////////////////////////////////////////////////////

// @brief ゲートタイプを返す．
PrimType
SnXor2::gate_type() const
{
  return PrimType::Xor;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnXor2::_calc_val(
  const std::vector<FSIM_VALTYPE>& val_array
) const
{
  return _calc_xor(val_array);
}

// @brief ゲートの入力から出力までの可観測性を計算する．
PackedVal
SnXor2::_calc_gobs(
  const std::vector<FSIM_VALTYPE>& val_array,
  SizeType ipos
) const
{
#if FSIM_VAL2
  // 2値なら常に観測可能
  return PV_ALL1;
#elif FSIM_VAL3
  // 3値の場合，Xでないことが条件
  auto alt_pos = ipos ^ 1;
  auto ival = val_array[_fanin(alt_pos)];
  return ival.val01();
#endif
}


//////////////////////////////////////////////////////////////////////
// SnXnor
//////////////////////////////////////////////////////////////////////

// @brief ゲートタイプを返す．
PrimType
SnXnor::gate_type() const
{
  return PrimType::Xnor;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnXnor::_calc_val(
  const std::vector<FSIM_VALTYPE>& val_array
) const
{
  return ~_calc_xor(val_array);
}


//////////////////////////////////////////////////////////////////////
// SnXnor2
//////////////////////////////////////////////////////////////////////

// @brief ゲートタイプを返す．
PrimType
SnXnor2::gate_type() const
{
  return PrimType::Xnor;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnXnor2::_calc_val(
  const std::vector<FSIM_VALTYPE>& val_array
) const
{
  return ~_calc_xor(val_array);
}

END_NAMESPACE_DRUID_FSIM
