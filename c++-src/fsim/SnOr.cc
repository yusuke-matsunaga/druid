
/// @file SnOr.cc
/// @brief SnOr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2017, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "SnOr.h"
#include "GateType.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID_FSIM

BEGIN_NONAMESPACE

// @brief 可観測性の条件を返す．
inline
PackedVal
_obs_val(FSIM_VALTYPE val)
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

// @brief コンストラクタ
SnOr::SnOr(int id,
	   const vector<SimNode*>& inputs) :
  SnGate(id, inputs)
{
}

// @brief デストラクタ
SnOr::~SnOr()
{
}

// @brief ゲートタイプを返す．
GateType
SnOr::gate_type() const
{
  return GateType::Or;
}

// @brief ファンインの値のORを計算する．
inline
FSIM_VALTYPE
SnOr::_calc_or()
{
  auto val = _fanin(0)->val();
  for ( auto i: Range(1, _fanin_num()) ) {
    val |= _fanin(i)->val();
  }
  return val;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnOr::_calc_val()
{
  return _calc_or();
}

// @brief ゲートの入力から出力までの可観測性を計算する．
PackedVal
SnOr::_calc_gobs(int ipos)
{
  auto obs = kPvAll1;
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

// @brief コンストラクタ
SnOr2::SnOr2(int id,
	     const vector<SimNode*>& inputs) :
  SnGate2(id, inputs)
{
}

// @brief デストラクタ
SnOr2::~SnOr2()
{
}

// @brief ゲートタイプを返す．
GateType
SnOr2::gate_type() const
{
  return GateType::Or;
}

// @brief ファンインの値のORを計算する．
inline
FSIM_VALTYPE
SnOr2::_calc_or()
{
  auto val0 = _fanin(0)->val();
  auto val1 = _fanin(1)->val();
  return val0 | val1;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnOr2::_calc_val()
{
  return _calc_or();
}

// @brief ゲートの入力から出力までの可観測性を計算する．
PackedVal
SnOr2::_calc_gobs(int ipos)
{
  auto alt_pos = ipos ^ 1;
  return _obs_val(_fanin(alt_pos)->val());
}


//////////////////////////////////////////////////////////////////////
// SnOr3
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnOr3::SnOr3(int id,
	     const vector<SimNode*>& inputs) :
  SnGate3(id, inputs)
{
}

// @brief デストラクタ
SnOr3::~SnOr3()
{
}

// @brief ゲートタイプを返す．
GateType
SnOr3::gate_type() const
{
  return GateType::Or;
}

// @brief ファンインの値のORを計算する．
inline
FSIM_VALTYPE
SnOr3::_calc_or()
{
  auto val0 = _fanin(0)->val();
  auto val1 = _fanin(1)->val();
  auto val2 = _fanin(2)->val();
  return val0 | val1 | val2;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnOr3::_calc_val()
{
  return _calc_or();
}

// @brief ゲートの入力から出力までの可観測性を計算する．
PackedVal
SnOr3::_calc_gobs(int ipos)
{
  int pos0;
  int pos1;
  switch ( ipos ) {
  case 0: pos0 = 1; pos1 = 2; break;
  case 1: pos0 = 0; pos1 = 2; break;
  case 2: pos0 = 0; pos1 = 1; break;
  default: ASSERT_NOT_REACHED;
  }
  auto val0 = _fanin(pos0)->val();
  auto val1 = _fanin(pos1)->val();
  return _obs_val(val0) & _obs_val(val1);
}


//////////////////////////////////////////////////////////////////////
// SnOr4
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnOr4::SnOr4(int id,
	     const vector<SimNode*>& inputs) :
  SnGate4(id, inputs)
{
}

// @brief デストラクタ
SnOr4::~SnOr4()
{
}

// @brief ゲートタイプを返す．
GateType
SnOr4::gate_type() const
{
  return GateType::Or;
}

// @brief ファンインの値のORを計算する．
inline
FSIM_VALTYPE
SnOr4::_calc_or()
{
  auto val0 = _fanin(0)->val();
  auto val1 = _fanin(1)->val();
  auto val2 = _fanin(2)->val();
  auto val3 = _fanin(3)->val();
  return val0 | val1 | val2 | val3;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnOr4::_calc_val()
{
  return _calc_or();
}

// @brief ゲートの入力から出力までの可観測性を計算する．
PackedVal
SnOr4::_calc_gobs(int ipos)
{
  int pos0;
  int pos1;
  int pos2;
  switch ( ipos ) {
  case 0: pos0 = 1; pos1 = 2; pos2 = 3; break;
  case 1: pos0 = 0; pos1 = 2; pos2 = 3; break;
  case 2: pos0 = 0; pos1 = 1; pos2 = 3; break;
  case 3: pos0 = 0; pos1 = 1; pos2 = 2; break;
  default: ASSERT_NOT_REACHED;
  }
  auto val0 = _fanin(pos0)->val();
  auto val1 = _fanin(pos1)->val();
  auto val2 = _fanin(pos2)->val();
  return _obs_val(val0) & _obs_val(val1) & _obs_val(val2);
}


//////////////////////////////////////////////////////////////////////
// SnNor
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnNor::SnNor(int id,
	     const vector<SimNode*>& inputs) :
  SnOr(id, inputs)
{
}

// @brief デストラクタ
SnNor::~SnNor()
{
}

// @brief ゲートタイプを返す．
GateType
SnNor::gate_type() const
{
  return GateType::Nor;
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

// @brief コンストラクタ
SnNor2::SnNor2(int id,
	       const vector<SimNode*>& inputs) :
  SnOr2(id, inputs)
{
}

// @brief デストラクタ
SnNor2::~SnNor2()
{
}

// @brief ゲートタイプを返す．
GateType
SnNor2::gate_type() const
{
  return GateType::Nor;
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

// @brief コンストラクタ
SnNor3::SnNor3(int id,
	       const vector<SimNode*>& inputs) :
  SnOr3(id, inputs)
{
}

// @brief デストラクタ
SnNor3::~SnNor3()
{
}

// @brief ゲートタイプを返す．
GateType
SnNor3::gate_type() const
{
  return GateType::Nor;
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

// @brief コンストラクタ
SnNor4::SnNor4(int id,
	       const vector<SimNode*>& inputs) :
  SnOr4(id, inputs)
{
}

// @brief デストラクタ
SnNor4::~SnNor4()
{
}

// @brief ゲートタイプを返す．
GateType
SnNor4::gate_type() const
{
  return GateType::Nor;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnNor4::_calc_val()
{
  return ~_calc_or();
}

END_NAMESPACE_DRUID_FSIM
