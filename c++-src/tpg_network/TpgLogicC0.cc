
/// @file TpgLogicC0.cc
/// @brief TpgLogicC0 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "TpgLogicC0.h"
#include "GateType.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス TpgLogcC0
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
TpgLogicC0::TpgLogicC0(int id) :
  TpgLogic0(id)
{
}

// @brief デストラクタ
TpgLogicC0::~TpgLogicC0()
{
}

// @brief ゲートタイプを得る．
//
// is_logic() が false の場合の返り値は不定
GateType
TpgLogicC0::gate_type() const
{
  return GateType::Const0;
}

END_NAMESPACE_DRUID
