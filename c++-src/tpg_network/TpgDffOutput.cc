
/// @file TpgDffOutput.cc
/// @brief TpgDffOutput の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "TpgDffOutput.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス TpgDffOutput
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
TpgDffOutput::TpgDffOutput(
  int input_id,
  const TpgDff* dff
) : TpgPPI{input_id},
    mDff{dff}
{
}

// @brief DFF の出力に接続している外部入力タイプの時 true を返す．
bool
TpgDffOutput::is_dff_output() const
{
  return true;
}

// @brief 接続している DFF を返す．
//
// is_dff_input() | is_dff_output() | is_dff_clock() | is_dff_clear() | is_dff_preset()
// の時に意味を持つ．
const TpgDff*
TpgDffOutput::dff() const
{
  return mDff;
}

END_NAMESPACE_DRUID
