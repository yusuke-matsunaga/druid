
/// @file TpgLogicXOR.cc
/// @brief TpgLogicXOR[x] の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "TpgLogicXOR.h"
#include "GateType.h"
#include "Val3.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス TpgLogicXOR2
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
TpgLogicXOR2::TpgLogicXOR2(
  const vector<const TpgNode*>& fanin_list
) : TpgLogic{fanin_list}
{
}

// @brief ゲートタイプを得る．
//
// is_logic() が false の場合の返り値は不定
GateType
TpgLogicXOR2::gate_type() const
{
  return GateType::Xor;
}

// @brief controling value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicXOR2::cval() const
{
  return Val3::_X;
}

// @brief noncontroling valueを得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicXOR2::nval() const
{
  return Val3::_X;
}

// @brief controling output value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicXOR2::coval() const
{
  return Val3::_X;
}

// @brief noncontroling output value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicXOR2::noval() const
{
  return Val3::_X;
}

END_NAMESPACE_DRUID
