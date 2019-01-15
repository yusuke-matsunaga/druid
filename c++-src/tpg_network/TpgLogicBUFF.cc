
/// @file TpgLogicBUFF.cc
/// @brief TpgLogicBUFF の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "TpgLogicBUFF.h"
#include "GateType.h"
#include "Val3.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス TpgLogicBUFF
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
// @param[in] fanin ファンイン
TpgLogicBUFF::TpgLogicBUFF(int id,
			   TpgNode* fanin) :
  TpgLogic1(id, fanin)
{
}

// @brief デストラクタ
TpgLogicBUFF::~TpgLogicBUFF()
{
}

// @brief ゲートタイプを得る．
//
// is_logic() が false の場合の返り値は不定
GateType
TpgLogicBUFF::gate_type() const
{
  return GateType::Buff;
}

// @brief controling value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicBUFF::cval() const
{
  return Val3::_X;
}

// @brief noncontroling valueを得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicBUFF::nval() const
{
  return Val3::_X;
}

// @brief controling output value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicBUFF::coval() const
{
  return Val3::_X;
}

// @brief noncontroling output value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicBUFF::noval() const
{
  return Val3::_X;
}

END_NAMESPACE_DRUID
