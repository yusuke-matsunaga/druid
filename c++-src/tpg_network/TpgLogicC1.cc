
/// @file TpgLogicC1.cc
/// @brief TpgLogicC1 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018 Yusuke Matsunaga
/// All rights reserved.

#include "TpgLogicC1.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス TpgLogcC1
//////////////////////////////////////////////////////////////////////

// @brief ゲートタイプを得る．
//
// is_logic() が false の場合の返り値は不定
PrimType
TpgLogicC1::gate_type() const
{
  return PrimType::Const1;
}

END_NAMESPACE_DRUID
