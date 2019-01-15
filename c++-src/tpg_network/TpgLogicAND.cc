
/// @file TpgLogicAND.cc
/// @brief TpgLogicAND[x] の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "TpgLogicAND.h"
#include "GateType.h"
#include "Val3.h"


BEGIN_NAMESPACE_DRUID


//////////////////////////////////////////////////////////////////////
// クラス TpgLogicAND2
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
// @param[in] fanin_list ファンインのリスト
TpgLogicAND2::TpgLogicAND2(int id,
			   const vector<TpgNode*>& fanin_list) :
  TpgLogic2(id, fanin_list)
{
}

// @brief デストラクタ
TpgLogicAND2::~TpgLogicAND2()
{
}

// @brief ゲートタイプを得る．
//
// is_logic() が false の場合の返り値は不定
GateType
TpgLogicAND2::gate_type() const
{
  return GateType::And;
}

// @brief controling value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicAND2::cval() const
{
  return Val3::_0;
}

// @brief noncontroling valueを得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicAND2::nval() const
{
  return Val3::_1;
}

// @brief controling output value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicAND2::coval() const
{
  return Val3::_0;
}

// @brief noncontroling output value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicAND2::noval() const
{
  return Val3::_1;
}


//////////////////////////////////////////////////////////////////////
// クラス TpgLogicAND3
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
// @param[in] fanin_list ファンインのリスト
TpgLogicAND3::TpgLogicAND3(int id,
			   const vector<TpgNode*>& fanin_list) :
  TpgLogic3(id, fanin_list)
{
}

// @brief デストラクタ
TpgLogicAND3::~TpgLogicAND3()
{
}

// @brief ゲートタイプを得る．
//
// is_logic() が false の場合の返り値は不定
GateType
TpgLogicAND3::gate_type() const
{
  return GateType::And;
}

// @brief controling value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicAND3::cval() const
{
  return Val3::_0;
}

// @brief noncontroling valueを得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicAND3::nval() const
{
  return Val3::_1;
}

// @brief controling output value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicAND3::coval() const
{
  return Val3::_0;
}

// @brief noncontroling output value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicAND3::noval() const
{
  return Val3::_1;
}


//////////////////////////////////////////////////////////////////////
// クラス TpgLogicAND4
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
// @param[in] fanin_list ファンインのリスト
TpgLogicAND4::TpgLogicAND4(int id,
			   const vector<TpgNode*>& fanin_list) :
  TpgLogic4(id, fanin_list)
{
}

// @brief デストラクタ
TpgLogicAND4::~TpgLogicAND4()
{
}

// @brief ゲートタイプを得る．
//
// is_logic() が false の場合の返り値は不定
GateType
TpgLogicAND4::gate_type() const
{
  return GateType::And;
}

// @brief controling value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicAND4::cval() const
{
  return Val3::_0;
}

// @brief noncontroling valueを得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicAND4::nval() const
{
  return Val3::_1;
}

// @brief controling output value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicAND4::coval() const
{
  return Val3::_0;
}

// @brief noncontroling output value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicAND4::noval() const
{
  return Val3::_1;
}


//////////////////////////////////////////////////////////////////////
// クラス TpgLogicANDN
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
TpgLogicANDN::TpgLogicANDN(int id) :
  TpgLogicN(id)
{
}

// @brief デストラクタ
TpgLogicANDN::~TpgLogicANDN()
{
}

// @brief ゲートタイプを得る．
//
// is_logic() が false の場合の返り値は不定
GateType
TpgLogicANDN::gate_type() const
{
  return GateType::And;
}

// @brief controling value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicANDN::cval() const
{
  return Val3::_0;
}

// @brief noncontroling valueを得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicANDN::nval() const
{
  return Val3::_1;
}

// @brief controling output value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicANDN::coval() const
{
  return Val3::_0;
}

// @brief noncontroling output value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicANDN::noval() const
{
  return Val3::_1;
}

END_NAMESPACE_DRUID
