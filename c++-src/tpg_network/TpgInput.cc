
/// @file TpgInput.cc
/// @brief TpgInput の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "TpgInput.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス TpgInput
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
TpgInput::TpgInput(
  int input_id
) : TpgPPI{input_id}
{
}

// @brief 外部入力タイプの時 true を返す．
bool
TpgInput::is_primary_input() const
{
  return true;
}

END_NAMESPACE_DRUID
