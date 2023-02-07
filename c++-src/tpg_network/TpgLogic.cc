
/// @file TpgLogic.cc
/// @brief TpgLogic の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "TpgLogic.h"
#include "TpgLogicC0.h"
#include "TpgLogicC1.h"
#include "TpgLogicBUFF.h"
#include "TpgLogicNOT.h"
#include "TpgLogicAND.h"
#include "TpgLogicNAND.h"
#include "TpgLogicOR.h"
#include "TpgLogicNOR.h"
#include "TpgLogicXOR.h"
#include "TpgLogicXNOR.h"

#include "Val3.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス TpgLogic
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
TpgLogic::TpgLogic(
  const vector<const TpgNode*>& fanin_list
) : TpgNode{fanin_list}
{
}

// @brief logic タイプの時 true を返す．
bool
TpgLogic::is_logic() const
{
  return true;
}


//////////////////////////////////////////////////////////////////////
// クラス TpgLogcC0
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
TpgLogicC0::TpgLogicC0(
) : TpgLogic{{}}

{
}

// @brief ゲートタイプを得る．
//
// is_logic() が false の場合の返り値は不定
PrimType
TpgLogicC0::gate_type() const
{
  return PrimType::C0;
}


//////////////////////////////////////////////////////////////////////
// クラス TpgLogcC1
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
TpgLogicC1::TpgLogicC1(
) : TpgLogic{{}}
{
}

// @brief ゲートタイプを得る．
//
// is_logic() が false の場合の返り値は不定
PrimType
TpgLogicC1::gate_type() const
{
  return PrimType::C1;
}


//////////////////////////////////////////////////////////////////////
// クラス TpgLogicBUFF
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
TpgLogicBUFF::TpgLogicBUFF(
  const TpgNode* fanin
) : TpgLogic{{fanin}}
{
}

// @brief ゲートタイプを得る．
//
// is_logic() が false の場合の返り値は不定
PrimType
TpgLogicBUFF::gate_type() const
{
  return PrimType::Buff;
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


//////////////////////////////////////////////////////////////////////
// クラス TpgLogicNOT
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
TpgLogicNOT::TpgLogicNOT(
  const TpgNode* fanin
) : TpgLogic{{fanin}}
{
}

// @brief ゲートタイプを得る．
//
// is_logic() が false の場合の返り値は不定
PrimType
TpgLogicNOT::gate_type() const
{
  return PrimType::Not;
}

// @brief controling value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicNOT::cval() const
{
  return Val3::_X;
}

// @brief noncontroling valueを得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicNOT::nval() const
{
  return Val3::_X;
}

// @brief controling output value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicNOT::coval() const
{
  return Val3::_X;
}

// @brief noncontroling output value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicNOT::noval() const
{
  return Val3::_X;
}


//////////////////////////////////////////////////////////////////////
// クラス TpgLogicAND
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
TpgLogicAND::TpgLogicAND(
  const vector<const TpgNode*>& fanin_list
) : TpgLogic{fanin_list}
{
}

// @brief ゲートタイプを得る．
//
// is_logic() が false の場合の返り値は不定
PrimType
TpgLogicAND::gate_type() const
{
  return PrimType::And;
}

// @brief controling value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicAND::cval() const
{
  return Val3::_0;
}

// @brief noncontroling valueを得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicAND::nval() const
{
  return Val3::_1;
}

// @brief controling output value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicAND::coval() const
{
  return Val3::_0;
}

// @brief noncontroling output value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicAND::noval() const
{
  return Val3::_1;
}


//////////////////////////////////////////////////////////////////////
// クラス TpgLogicNAND
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
TpgLogicNAND::TpgLogicNAND(
  const vector<const TpgNode*>& fanin_list
) : TpgLogic{fanin_list}
{
}

// @brief ゲートタイプを得る．
//
// is_logic() が false の場合の返り値は不定
PrimType
TpgLogicNAND::gate_type() const
{
  return PrimType::Nand;
}

// @brief controling value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicNAND::cval() const
{
  return Val3::_0;
}

// @brief noncontroling valueを得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicNAND::nval() const
{
  return Val3::_1;
}

// @brief controling output value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicNAND::coval() const
{
  return Val3::_1;
}

// @brief noncontroling output value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicNAND::noval() const
{
  return Val3::_0;
}


//////////////////////////////////////////////////////////////////////
// クラス TpgLogicOR
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
TpgLogicOR::TpgLogicOR(
  const vector<const TpgNode*>& fanin_list
) : TpgLogic{fanin_list}
{
}

// @brief ゲートタイプを得る．
//
// is_logic() が false の場合の返り値は不定
PrimType
TpgLogicOR::gate_type() const
{
  return PrimType::Or;
}

// @brief controling value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicOR::cval() const
{
  return Val3::_1;
}

// @brief noncontroling valueを得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicOR::nval() const
{
  return Val3::_0;
}

// @brief controling output value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicOR::coval() const
{
  return Val3::_1;
}

// @brief noncontroling output value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicOR::noval() const
{
  return Val3::_0;
}


//////////////////////////////////////////////////////////////////////
// クラス TpgLogicNOR2
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
TpgLogicNOR::TpgLogicNOR(
  const vector<const TpgNode*>& fanin_list
) : TpgLogic{fanin_list}
{
}

// @brief ゲートタイプを得る．
//
// is_logic() が false の場合の返り値は不定
PrimType
TpgLogicNOR::gate_type() const
{
  return PrimType::Nor;
}

// @brief controling value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicNOR::cval() const
{
  return Val3::_1;
}

// @brief noncontroling valueを得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicNOR::nval() const
{
  return Val3::_0;
}

// @brief controling output value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicNOR::coval() const
{
  return Val3::_0;
}

// @brief noncontroling output value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicNOR::noval() const
{
  return Val3::_1;
}


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
PrimType
TpgLogicXOR2::gate_type() const
{
  return PrimType::Xor;
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


//////////////////////////////////////////////////////////////////////
// クラス TpgLogicXNOR2
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
TpgLogicXNOR2::TpgLogicXNOR2(
  const vector<const TpgNode*>& fanin_list
) : TpgLogic{fanin_list}
{
}

// @brief ゲートタイプを得る．
//
// is_logic() が false の場合の返り値は不定
PrimType
TpgLogicXNOR2::gate_type() const
{
  return PrimType::Xnor;
}

// @brief controling value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicXNOR2::cval() const
{
  return Val3::_X;
}

// @brief noncontroling valueを得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicXNOR2::nval() const
{
  return Val3::_X;
}

// @brief controling output value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicXNOR2::coval() const
{
  return Val3::_X;
}

// @brief noncontroling output value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は Val3::_X を返す．
Val3
TpgLogicXNOR2::noval() const
{
  return Val3::_X;
}

END_NAMESPACE_DRUID
