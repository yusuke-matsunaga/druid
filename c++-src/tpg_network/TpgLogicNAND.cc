
/// @file TpgLogicNAND.cc
/// @brief TpgLogicNAND[x] の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "TpgLogicNAND.h"
#include "GateType.h"
#include "Val3.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス TpgLogicNAND2
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
// @param[in] fanin_list ファンインのリスト
TpgLogicNAND2::TpgLogicNAND2(int id,
			     const vector<TpgNode*>& fanin_list) :
  TpgLogic2(id, fanin_list)
{
}

// @brief デストラクタ
TpgLogicNAND2::~TpgLogicNAND2()
{
}

// @brief ゲートタイプを得る．
//
// is_logic() が false の場合の返り値は不定
GateType
TpgLogicNAND2::gate_type() const
{
  return GateType::Nand;
}

// @brief controling value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicNAND2::cval() const
{
  return Val3::_0;
}

// @brief noncontroling valueを得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicNAND2::nval() const
{
  return Val3::_1;
}

// @brief controling output value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicNAND2::coval() const
{
  return Val3::_1;
}

// @brief noncontroling output value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicNAND2::noval() const
{
  return Val3::_0;
}


//////////////////////////////////////////////////////////////////////
// クラス TpgLogicNAND3
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
// @param[in] fanin_list ファンインのリスト
TpgLogicNAND3::TpgLogicNAND3(int id,
			     const vector<TpgNode*>& fanin_list) :
  TpgLogic3(id, fanin_list)
{
}

// @brief デストラクタ
TpgLogicNAND3::~TpgLogicNAND3()
{
}

// @brief ゲートタイプを得る．
//
// is_logic() が false の場合の返り値は不定
GateType
TpgLogicNAND3::gate_type() const
{
  return GateType::Nand;
}

// @brief controling value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicNAND3::cval() const
{
  return Val3::_0;
}

// @brief noncontroling valueを得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicNAND3::nval() const
{
  return Val3::_1;
}

// @brief controling output value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicNAND3::coval() const
{
  return Val3::_1;
}

// @brief noncontroling output value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicNAND3::noval() const
{
  return Val3::_0;
}


//////////////////////////////////////////////////////////////////////
// クラス TpgLogicNAND4
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
// @param[in] fanin_list ファンインのリスト
TpgLogicNAND4::TpgLogicNAND4(int id,
			     const vector<TpgNode*>& fanin_list) :
  TpgLogic4(id, fanin_list)
{
}

// @brief デストラクタ
TpgLogicNAND4::~TpgLogicNAND4()
{
}

// @brief ゲートタイプを得る．
//
// is_logic() が false の場合の返り値は不定
GateType
TpgLogicNAND4::gate_type() const
{
  return GateType::Nand;
}

// @brief controling value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicNAND4::cval() const
{
  return Val3::_0;
}

// @brief noncontroling valueを得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicNAND4::nval() const
{
  return Val3::_1;
}

// @brief controling output value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicNAND4::coval() const
{
  return Val3::_1;
}

// @brief noncontroling output value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicNAND4::noval() const
{
  return Val3::_0;
}


//////////////////////////////////////////////////////////////////////
// クラス TpgLogicNANDN
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
TpgLogicNANDN::TpgLogicNANDN(int id) :
  TpgLogicN(id)
{
}

// @brief デストラクタ
TpgLogicNANDN::~TpgLogicNANDN()
{
}

// @brief ゲートタイプを得る．
//
// is_logic() が false の場合の返り値は不定
GateType
TpgLogicNANDN::gate_type() const
{
  return GateType::Nand;
}

// @brief controling value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicNANDN::cval() const
{
  return Val3::_0;
}

// @brief noncontroling valueを得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicNANDN::nval() const
{
  return Val3::_1;
}

// @brief controling output value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicNANDN::coval() const
{
  return Val3::_1;
}

// @brief noncontroling output value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicNANDN::noval() const
{
  return Val3::_0;
}

END_NAMESPACE_DRUID
