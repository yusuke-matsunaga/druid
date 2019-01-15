
/// @file SnXor.cc
/// @brief SnXor の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2017, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "SnXor.h"
#include "GateType.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID_FSIM

//////////////////////////////////////////////////////////////////////
// SnXor
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnXor::SnXor(int id,
	     const vector<SimNode*>& inputs) :
  SnGate(id, inputs)
{
}

// @brief デストラクタ
SnXor::~SnXor()
{
}

// @brief ゲートタイプを返す．
GateType
SnXor::gate_type() const
{
  return GateType::Xor;
}

// @brief ファンインの値のXORを計算する．
inline
FSIM_VALTYPE
SnXor::_calc_xor()
{
  auto val = _fanin(0)->val();
  for ( auto i: Range(1, _fanin_num()) ) {
    val ^= _fanin(i)->val();
  }
  return val;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnXor::_calc_val()
{
  return _calc_xor();
}

// @brief ゲートの入力から出力までの可観測性を計算する．(3値版)
PackedVal
SnXor::_calc_gobs(int ipos)
{
#if FSIM_VAL2
  // 2値なら常に観測可能
  return kPvAll1;
#elif FSIM_VAL3
  // 3値はめんどくさい
  // 条件は ipos 以外が X でないこと
  auto obs = kPvAll1;
  for ( auto i: Range(0, ipos) ) {
    auto ival = _fanin(i)->val();
    obs &= ival.val01();
  }
  for ( auto i: Range(ipos + 1, _fanin_num()) ) {
    auto ival = _fanin(i)->val();
    obs &= ival.val01();
  }
  return obs;
#endif
}


//////////////////////////////////////////////////////////////////////
// SnXor2
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnXor2::SnXor2(int id,
	       const vector<SimNode*>& inputs) :
  SnGate2(id, inputs)
{
}

// @brief デストラクタ
SnXor2::~SnXor2()
{
}

// @brief ゲートタイプを返す．
GateType
SnXor2::gate_type() const
{
  return GateType::Xor;
}

// @brief ファンインの値のXORを計算する．
inline
FSIM_VALTYPE
SnXor2::_calc_xor()
{
  auto val0 = _fanin(0)->val();
  auto val1 = _fanin(1)->val();
  return val0 ^ val1;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnXor2::_calc_val()
{
  return _calc_xor();
}

// @brief ゲートの入力から出力までの可観測性を計算する．
PackedVal
SnXor2::_calc_gobs(int ipos)
{
#if FSIM_VAL2
  // 2値なら常に観測可能
  return kPvAll1;
#elif FSIM_VAL3
  // 3値の場合，Xでないことが条件
  auto alt_pos = ipos ^ 1;
  auto ival = _fanin(alt_pos)->val();
  return ival.val01();
#endif
}


//////////////////////////////////////////////////////////////////////
// SnXnor
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnXnor::SnXnor(int id,
	       const vector<SimNode*>& inputs) :
  SnXor(id, inputs)
{
}

// @brief デストラクタ
SnXnor::~SnXnor()
{
}

// @brief ゲートタイプを返す．
GateType
SnXnor::gate_type() const
{
  return GateType::Xnor;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnXnor::_calc_val()
{
  return ~_calc_xor();
}


//////////////////////////////////////////////////////////////////////
// SnXnor2
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnXnor2::SnXnor2(int id,
		 const vector<SimNode*>& inputs) :
  SnXor2(id, inputs)
{
}

// @brief デストラクタ
SnXnor2::~SnXnor2()
{
}

// @brief ゲートタイプを返す．
GateType
SnXnor2::gate_type() const
{
  return GateType::Xnor;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnXnor2::_calc_val()
{
  return ~_calc_xor();
}

END_NAMESPACE_DRUID_FSIM
