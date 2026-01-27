
/// @file SnAnd.cc
/// @brief SnAnd の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "SnAnd.h"
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
  return val;
#elif FSIM_VAL3
  return val.val1();
#endif
}
END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// SnAnd
//////////////////////////////////////////////////////////////////////

// @brief ゲートタイプを返す．
PrimType
SnAnd::gate_type() const
{
  return PrimType::And;
}

// @brief 故障値の計算を行う．
FSIM_VALTYPE
SnAnd::_calc_val()
{
  return _calc_and();
}

// @brief ゲートの入力から出力までの可観測性を計算する．
PackedVal
SnAnd::_calc_gobs(
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
// SnAnd2
//////////////////////////////////////////////////////////////////////

// @brief ゲートタイプを返す．
PrimType
SnAnd2::gate_type() const
{
  return PrimType::And;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnAnd2::_calc_val()
{
  return _calc_and();
}

// @brief ゲートの入力から出力までの可観測性を計算する．
PackedVal
SnAnd2::_calc_gobs(
  SizeType ipos
)
{
  FSIM_VALTYPE val0 = _get_sideval(ipos);
  return _obs_val(val0);
}


//////////////////////////////////////////////////////////////////////
// SnAnd3
//////////////////////////////////////////////////////////////////////

// @brief ゲートタイプを返す．
PrimType
SnAnd3::gate_type() const
{
  return PrimType::And;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnAnd3::_calc_val()
{
  return _calc_and();
}

// @brief ゲートの入力から出力までの可観測性を計算する．
PackedVal
SnAnd3::_calc_gobs(
  SizeType ipos
)
{
  FSIM_VALTYPE val0;
  FSIM_VALTYPE val1;
  _get_sideval(ipos, val0, val1);
  return _obs_val(val0) & _obs_val(val1);
}


//////////////////////////////////////////////////////////////////////
// SnAnd4
//////////////////////////////////////////////////////////////////////

// @brief ゲートタイプを返す．
PrimType
SnAnd4::gate_type() const
{
  return PrimType::And;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnAnd4::_calc_val()
{
  return _calc_and();
}

// @brief ゲートの入力から出力までの可観測性を計算する．
PackedVal
SnAnd4::_calc_gobs(
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
// SnNand
//////////////////////////////////////////////////////////////////////

// @brief ゲートタイプを返す．
PrimType
SnNand::gate_type() const
{
  return PrimType::Nand;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnNand::_calc_val()
{
  return ~_calc_and();
}


//////////////////////////////////////////////////////////////////////
// SnNand2
//////////////////////////////////////////////////////////////////////

// @brief ゲートタイプを返す．
PrimType
SnNand2::gate_type() const
{
  return PrimType::Nand;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnNand2::_calc_val()
{
  return ~_calc_and();
}


//////////////////////////////////////////////////////////////////////
// SnNand3
//////////////////////////////////////////////////////////////////////

// @brief ゲートタイプを返す．
PrimType
SnNand3::gate_type() const
{
  return PrimType::Nand;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnNand3::_calc_val()
{
  return ~_calc_and();
}


//////////////////////////////////////////////////////////////////////
// SnNand4
//////////////////////////////////////////////////////////////////////

// @brief ゲートタイプを返す．
PrimType
SnNand4::gate_type() const
{
  return PrimType::Nand;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnNand4::_calc_val()
{
  return ~_calc_and();
}

END_NAMESPACE_DRUID_FSIM
