
/// @file SnAnd.cc
/// @brief SnAnd の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2017, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "SnAnd.h"
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
  return val;
#elif FSIM_VAL3
  return val.val1();
#endif
}
END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// SnAnd
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnAnd::SnAnd(int id,
	     const vector<SimNode*>& inputs) :
  SnGate(id, inputs)
{
}

// @brief デストラクタ
SnAnd::~SnAnd()
{
}

// @brief ゲートタイプを返す．
GateType
SnAnd::gate_type() const
{
  return GateType::And;
}

// @brief ファンインの値のANDを計算する．
inline
FSIM_VALTYPE
SnAnd::_calc_and()
{
  auto val = _fanin(0)->val();
  for ( auto i: Range(1, _fanin_num()) ) {
    val &= _fanin(i)->val();
  }
  return val;
}

// @brief 故障値の計算を行う．
FSIM_VALTYPE
SnAnd::_calc_val()
{
  return _calc_and();
}

// @brief ゲートの入力から出力までの可観測性を計算する．
PackedVal
SnAnd::_calc_gobs(int ipos)
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
// SnAnd2
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnAnd2::SnAnd2(int id,
	       const vector<SimNode*>& inputs) :
  SnGate2(id, inputs)
{
}

// @brief デストラクタ
SnAnd2::~SnAnd2()
{
}

// @brief ゲートタイプを返す．
GateType
SnAnd2::gate_type() const
{
  return GateType::And;
}

// @brief ファンインの値のANDを計算する．
inline
FSIM_VALTYPE
SnAnd2::_calc_and()
{
  auto val0 = _fanin(0)->val();
  auto val1 = _fanin(1)->val();
  return val0 & val1;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnAnd2::_calc_val()
{
  return _calc_and();
}

// @brief ゲートの入力から出力までの可観測性を計算する．
PackedVal
SnAnd2::_calc_gobs(int ipos)
{
  auto alt_pos = ipos ^ 1;
  return _obs_val(_fanin(alt_pos)->val());
}


//////////////////////////////////////////////////////////////////////
// SnAnd3
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnAnd3::SnAnd3(int id,
	       const vector<SimNode*>& inputs) :
  SnGate3(id, inputs)
{
}

// @brief デストラクタ
SnAnd3::~SnAnd3()
{
}

// @brief ゲートタイプを返す．
GateType
SnAnd3::gate_type() const
{
  return GateType::And;
}

// @brief ファンインの値のANDを計算する．
inline
FSIM_VALTYPE
SnAnd3::_calc_and()
{
  auto val0 = _fanin(0)->val();
  auto val1 = _fanin(1)->val();
  auto val2 = _fanin(2)->val();
  return val0 & val1 & val2;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnAnd3::_calc_val()
{
  return _calc_and();
}

// @brief ゲートの入力から出力までの可観測性を計算する．
PackedVal
SnAnd3::_calc_gobs(int ipos)
{
  int pos0;
  int pos1;
  switch ( ipos ) {
  case 0: pos0 = 1; pos1 = 2; break;
  case 1: pos0 = 0; pos1 = 2; break;
  case 2: pos0 = 0; pos1 = 1; break;
  default: ASSERT_NOT_REACHED; break;
  }
  auto val0 = _fanin(pos0)->val();
  auto val1 = _fanin(pos1)->val();
  return _obs_val(val0) & _obs_val(val1);
}


//////////////////////////////////////////////////////////////////////
// SnAnd4
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnAnd4::SnAnd4(int id,
	       const vector<SimNode*>& inputs) :
  SnGate4(id, inputs)
{
}

// @brief デストラクタ
SnAnd4::~SnAnd4()
{
}

// @brief ゲートタイプを返す．
GateType
SnAnd4::gate_type() const
{
  return GateType::And;
}

// @brief ファンインの値のANDを計算する．
inline
FSIM_VALTYPE
SnAnd4::_calc_and()
{
  auto val0 = _fanin(0)->val();
  auto val1 = _fanin(1)->val();
  auto val2 = _fanin(2)->val();
  auto val3 = _fanin(3)->val();
  return val0 & val1 & val2 & val3;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnAnd4::_calc_val()
{
  return _calc_and();
}

// @brief ゲートの入力から出力までの可観測性を計算する．
PackedVal
SnAnd4::_calc_gobs(int ipos)
{
  int pos0;
  int pos1;
  int pos2;
  switch ( ipos ) {
  case 0: pos0 = 1; pos1 = 2; pos2 = 3; break;
  case 1: pos0 = 0; pos1 = 2; pos2 = 3; break;
  case 2: pos0 = 0; pos1 = 1; pos2 = 3; break;
  case 3: pos0 = 0; pos1 = 1; pos2 = 2; break;
  default: ASSERT_NOT_REACHED; break;
  }
  auto val0 = _fanin(pos0)->val();
  auto val1 = _fanin(pos1)->val();
  auto val2 = _fanin(pos2)->val();
  return _obs_val(val0) & _obs_val(val1) & _obs_val(val2);
}


//////////////////////////////////////////////////////////////////////
// SnNand
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnNand::SnNand(int id,
	       const vector<SimNode*>& inputs) :
  SnAnd(id, inputs)
{
}

// @brief デストラクタ
SnNand::~SnNand()
{
}

// @brief ゲートタイプを返す．
GateType
SnNand::gate_type() const
{
  return GateType::Nand;
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

// @brief コンストラクタ
SnNand2::SnNand2(int id,
		 const vector<SimNode*>& inputs) :
  SnAnd2(id, inputs)
{
}

// @brief デストラクタ
SnNand2::~SnNand2()
{
}

// @brief ゲートタイプを返す．
GateType
SnNand2::gate_type() const
{
  return GateType::Nand;
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

// @brief コンストラクタ
SnNand3::SnNand3(int id,
		 const vector<SimNode*>& inputs) :
  SnAnd3(id, inputs)
{
}

// @brief デストラクタ
SnNand3::~SnNand3()
{
}

// @brief ゲートタイプを返す．
GateType
SnNand3::gate_type() const
{
  return GateType::Nand;
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

// @brief コンストラクタ
SnNand4::SnNand4(int id,
		 const vector<SimNode*>& inputs) :
  SnAnd4(id, inputs)
{
}

// @brief デストラクタ
SnNand4::~SnNand4()
{
}

// @brief ゲートタイプを返す．
GateType
SnNand4::gate_type() const
{
  return GateType::Nand;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnNand4::_calc_val()
{
  return ~_calc_and();
}

END_NAMESPACE_DRUID_FSIM
