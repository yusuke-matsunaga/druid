
/// @file SnBuff.cc
/// @brief SnBuff の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2017, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "SnBuff.h"
#include "GateType.h"


BEGIN_NAMESPACE_DRUID_FSIM

//////////////////////////////////////////////////////////////////////
// SnBuff
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnBuff::SnBuff(int id,
	       const vector<SimNode*>& inputs) :
  SnGate1(id, inputs)
{
}

// @brief デストラクタ
SnBuff::~SnBuff()
{
}

// @brief ゲートタイプを返す．
GateType
SnBuff::gate_type() const
{
  return GateType::Buff;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnBuff::_calc_val()
{
  return _fanin()->val();
}

// @brief ゲートの入力から出力までの可観測性を計算する．
PackedVal
SnBuff::_calc_gobs(int ipos)
{
  return kPvAll1;
}


//////////////////////////////////////////////////////////////////////
// SnNot
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnNot::SnNot(int id,
	     const vector<SimNode*>& inputs) :
  SnBuff(id, inputs)
{
}

// @brief デストラクタ
SnNot::~SnNot()
{
}

// @brief ゲートタイプを返す．
GateType
SnNot::gate_type() const
{
  return GateType::Not;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnNot::_calc_val()
{
  return ~_fanin()->val();
}

END_NAMESPACE_DRUID_FSIM
