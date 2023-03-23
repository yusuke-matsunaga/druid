
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
PrimType
TpgLogicBUFF::gate_type() const
{
  return PrimType::Buff;
}

// @brief controling value を得る．
Val3
TpgLogicBUFF::cval() const
{
  return Val3::_X;
}

// @brief noncontroling valueを得る．
Val3
TpgLogicBUFF::nval() const
{
  return Val3::_X;
}

// @brief controling output value を得る．
Val3
TpgLogicBUFF::coval() const
{
  return Val3::_X;
}

// @brief noncontroling output value を得る．
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
PrimType
TpgLogicNOT::gate_type() const
{
  return PrimType::Not;
}

// @brief controling value を得る．
Val3
TpgLogicNOT::cval() const
{
  return Val3::_X;
}

// @brief noncontroling valueを得る．
Val3
TpgLogicNOT::nval() const
{
  return Val3::_X;
}

// @brief controling output value を得る．
Val3
TpgLogicNOT::coval() const
{
  return Val3::_X;
}

// @brief noncontroling output value を得る．
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
PrimType
TpgLogicAND::gate_type() const
{
  return PrimType::And;
}

// @brief controling value を得る．
Val3
TpgLogicAND::cval() const
{
  return Val3::_0;
}

// @brief noncontroling valueを得る．
Val3
TpgLogicAND::nval() const
{
  return Val3::_1;
}

// @brief controling output value を得る．
Val3
TpgLogicAND::coval() const
{
  return Val3::_0;
}

// @brief noncontroling output value を得る．
Val3
TpgLogicAND::noval() const
{
  return Val3::_1;
}

// @brief side-input の値を得る．
Val3
TpgLogicAND::side_val() const
{
  return Val3::_1;
}


//////////////////////////////////////////////////////////////////////
// クラス TpgLogicNAND
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
TpgLogicNAND::TpgLogicNAND(
  const vector<const TpgNode*>& fanin_list
) : TpgLogicAND{fanin_list}
{
}

// @brief ゲートタイプを得る．
PrimType
TpgLogicNAND::gate_type() const
{
  return PrimType::Nand;
}

// @brief controling output value を得る．
Val3
TpgLogicNAND::coval() const
{
  return Val3::_1;
}

// @brief noncontroling output value を得る．
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
PrimType
TpgLogicOR::gate_type() const
{
  return PrimType::Or;
}

// @brief controling value を得る．
Val3
TpgLogicOR::cval() const
{
  return Val3::_1;
}

// @brief noncontroling valueを得る．
Val3
TpgLogicOR::nval() const
{
  return Val3::_0;
}

// @brief controling output value を得る．
Val3
TpgLogicOR::coval() const
{
  return Val3::_1;
}

// @brief noncontroling output value を得る．
Val3
TpgLogicOR::noval() const
{
  return Val3::_0;
}

// @brief side-input の値を得る．
Val3
TpgLogicOR::side_val() const
{
  return Val3::_0;
}


//////////////////////////////////////////////////////////////////////
// クラス TpgLogicNOR
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
TpgLogicNOR::TpgLogicNOR(
  const vector<const TpgNode*>& fanin_list
) : TpgLogicOR{fanin_list}
{
}

// @brief ゲートタイプを得る．
PrimType
TpgLogicNOR::gate_type() const
{
  return PrimType::Nor;
}

// @brief controling output value を得る．
Val3
TpgLogicNOR::coval() const
{
  return Val3::_0;
}

// @brief noncontroling output value を得る．
Val3
TpgLogicNOR::noval() const
{
  return Val3::_1;
}


//////////////////////////////////////////////////////////////////////
// クラス TpgLogicXOR
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
TpgLogicXOR::TpgLogicXOR(
  const vector<const TpgNode*>& fanin_list
) : TpgLogic{fanin_list}
{
}

// @brief ゲートタイプを得る．
PrimType
TpgLogicXOR::gate_type() const
{
  return PrimType::Xor;
}

// @brief controling value を得る．
Val3
TpgLogicXOR::cval() const
{
  return Val3::_X;
}

// @brief noncontroling valueを得る．
Val3
TpgLogicXOR::nval() const
{
  return Val3::_X;
}

// @brief controling output value を得る．
Val3
TpgLogicXOR::coval() const
{
  return Val3::_X;
}

// @brief noncontroling output value を得る．
Val3
TpgLogicXOR::noval() const
{
  return Val3::_X;
}

// @brief side-input の値を得る．
Val3
TpgLogicXOR::side_val() const
{
  return Val3::_X;
}


//////////////////////////////////////////////////////////////////////
// クラス TpgLogicXNOR
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
TpgLogicXNOR::TpgLogicXNOR(
  const vector<const TpgNode*>& fanin_list
) : TpgLogicXOR{fanin_list}
{
}

// @brief ゲートタイプを得る．
PrimType
TpgLogicXNOR::gate_type() const
{
  return PrimType::Xnor;
}

END_NAMESPACE_DRUID
