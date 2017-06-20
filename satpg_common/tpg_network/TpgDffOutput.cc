
/// @file TpgDffOutput.cc
/// @brief TpgDffOutput の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "TpgDffOutput.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
// クラス TpgDffOutput
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
// @param[in] input_id 入力番号
// @param[in] dff 接続しているDFF
TpgDffOutput::TpgDffOutput(ymuint id,
			   ymuint input_id,
			   TpgDff* dff) :
  TpgPPI(id, input_id),
  mDff(dff)
{
}

// @brief デストラクタ
TpgDffOutput::~TpgDffOutput()
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
TpgDff*
TpgDffOutput::dff() const
{
  return mDff;
}

END_NAMESPACE_YM_SATPG
