
/// @file SnOr.cc
/// @brief SnOr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "SnOr.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID_FSIM

BEGIN_NONAMESPACE

// @brief 可観測性の条件を返す．
inline
PackedVal
_obs_val(
  FSIM_VALTYPE val
)
{
#if FSIM_VAL2
  return ~val;
#elif FSIM_VAL3
  return val.val0();
#endif
}

END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// SnOr
//////////////////////////////////////////////////////////////////////

// @brief ゲートタイプを返す．
PrimType
SnOr::gate_type() const
{
  return PrimType::Or;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnOr::_calc_val()
{
  return _calc_or();
}

// @brief ゲートの入力から出力までの可観測性を計算する．
PackedVal
SnOr::_calc_gobs(
  SizeType ipos
)
{
  auto obs = PV_ALL1;
  for ( auto i: Range(0, ipos) ) {
    obs &= _obs_val(_fanin(i)->val());
  }
  for ( auto i: Range(ipos + 1, _fanin_num()) ) {
    obs &= _obs_val(_fanin(i)->val());
  }
  return obs;
}


//////////////////////////////////////////////////////////////////////
// SnOr2
//////////////////////////////////////////////////////////////////////

// @brief ゲートタイプを返す．
PrimType
SnOr2::gate_type() const
{
  return PrimType::Or;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnOr2::_calc_val()
{
  return _calc_or();
}

// @brief ゲートの入力から出力までの可観測性を計算する．
PackedVal
SnOr2::_calc_gobs(
  SizeType ipos
)
{
  FSIM_VALTYPE val0 = _get_sideval(ipos);
  return _obs_val(val0);
}


//////////////////////////////////////////////////////////////////////
// SnOr3
//////////////////////////////////////////////////////////////////////

// @brief ゲートタイプを返す．
PrimType
SnOr3::gate_type() const
{
  return PrimType::Or;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnOr3::_calc_val()
{
  return _calc_or();
}

// @brief ゲートの入力から出力までの可観測性を計算する．
PackedVal
SnOr3::_calc_gobs(
  SizeType ipos
)
{
  FSIM_VALTYPE val0;
  FSIM_VALTYPE val1;
  _get_sideval(ipos, val0, val1);
  return _obs_val(val0) & _obs_val(val1);
}


//////////////////////////////////////////////////////////////////////
// SnOr4
//////////////////////////////////////////////////////////////////////

// @brief ゲートタイプを返す．
PrimType
SnOr4::gate_type() const
{
  return PrimType::Or;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnOr4::_calc_val()
{
  return _calc_or();
}

// @brief ゲートの入力から出力までの可観測性を計算する．
PackedVal
SnOr4::_calc_gobs(
  SizeType ipos
)
{
  FSIM_VALTYPE val0;
  FSIM_VALTYPE val1;
  FSIM_VALTYPE val2;
  _get_sideval(ipos, val0, val1, val2);
  return _obs_val(val0) & _obs_val(val1) & _obs_val(val2);
}


//////////////////////////////////////////////////////////////////////
// SnNor
//////////////////////////////////////////////////////////////////////

// @brief ゲートタイプを返す．
PrimType
SnNor::gate_type() const
{
  return PrimType::Nor;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnNor::_calc_val()
{
  return ~_calc_or();
}


//////////////////////////////////////////////////////////////////////
// SnNor2
//////////////////////////////////////////////////////////////////////

// @brief ゲートタイプを返す．
PrimType
SnNor2::gate_type() const
{
  return PrimType::Nor;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnNor2::_calc_val()
{
  return ~_calc_or();
}


//////////////////////////////////////////////////////////////////////
// SnNor3
//////////////////////////////////////////////////////////////////////

// @brief ゲートタイプを返す．
PrimType
SnNor3::gate_type() const
{
  return PrimType::Nor;
}

// @brief 出力値の計算を行う．(3値版)
FSIM_VALTYPE
SnNor3::_calc_val()
{
  return ~_calc_or();
}


//////////////////////////////////////////////////////////////////////
// SnNor4
//////////////////////////////////////////////////////////////////////

// @brief ゲートタイプを返す．
PrimType
SnNor4::gate_type() const
{
  return PrimType::Nor;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnNor4::_calc_val()
{
  return ~_calc_or();
}

END_NAMESPACE_DRUID_FSIM
