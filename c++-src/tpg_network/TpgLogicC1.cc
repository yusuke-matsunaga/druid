
/// @file TpgLogicC1.cc
/// @brief TpgLogicC1 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "TpgLogicC1.h"
#include "GateType.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス TpgLogcC1
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
TpgLogicC1::TpgLogicC1(int id) :
  TpgLogic0(id)
{
}

// @brief デストラクタ
TpgLogicC1::~TpgLogicC1()
{
}

// @brief ゲートタイプを得る．
//
// is_logic() が false の場合の返り値は不定
GateType
TpgLogicC1::gate_type() const
{
  return GateType::Const1;
}

END_NAMESPACE_DRUID
