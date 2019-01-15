
/// @file TpgDffInput.cc
/// @brief TpgDffInput の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "TpgDffInput.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス TpgDffInput
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
// @param[in] output_id 出力番号
// @param[in] dff 接続しているDFF
// @param[in] fanin ファンインのノード
TpgDffInput::TpgDffInput(int id,
			 int output_id,
			 const TpgDff* dff,
			 TpgNode* fanin) :
  TpgPPO(id, output_id, fanin),
  mDff(dff)
{
}

// @brief デストラクタ
TpgDffInput::~TpgDffInput()
{
}

// @brief DFF の入力に接続している外部出力タイプの時 true を返す．
bool
TpgDffInput::is_dff_input() const
{
  return true;
}

// @brief 接続している DFF を返す．
//
// is_dff_input() | is_dff_output() | is_dff_clock() | is_dff_clear() | is_dff_preset()
// の時に意味を持つ．
const TpgDff*
TpgDffInput::dff() const
{
  return mDff;
}

END_NAMESPACE_DRUID
