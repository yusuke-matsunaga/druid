
/// @file TpgPPO.cc
/// @brief TpgPPO の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "TpgPPO.h"
#include "TpgOutput.h"
#include "TpgDffInput.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス TpgPPO
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
TpgPPO::TpgPPO(
  SizeType output_id,
  const TpgNode* fanin
) : TpgNode{{fanin}},
    mOutputId{output_id}
{
  mOutputId2 = 0;
}

// @brief 出力タイプの時 true を返す．
//
// 具体的には is_primary_output() || is_dff_input()
bool
TpgPPO::is_ppo() const
{
  return true;
}

// @brief 外部出力タイプの時に出力番号を返す．
//
// node = TpgNetwork::output(node->output_id())
// の関係を満たす．
// is_output() が false の場合の返り値は不定
SizeType
TpgPPO::output_id() const
{
  return mOutputId;
}

// @brief TFIサイズの昇順に並べた時の出力番号を返す．
SizeType
TpgPPO::output_id2() const
{
  return mOutputId2;
}

// @brief ゲートタイプを得る．
//
// is_logic() が false の場合の返り値は不定
PrimType
TpgPPO::gate_type() const
{
  return PrimType::Buff;
}

// @brief 出力番号2をセットする．
// @param[in] id セットする番号
//
// 出力ノード以外では無効
void
TpgPPO::set_output_id2(
  SizeType id
)
{
  mOutputId2 = id;
}


//////////////////////////////////////////////////////////////////////
// クラス TpgOutput
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
TpgOutput::TpgOutput(
  SizeType output_id,
  const TpgNode* fanin
) : TpgPPO{output_id, fanin}
{
}

// @brief 外部出力タイプの時 true を返す．
bool
TpgOutput::is_primary_output() const
{
  return true;
}


//////////////////////////////////////////////////////////////////////
// クラス TpgDffInput
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
TpgDffInput::TpgDffInput(
  SizeType output_id,
  const TpgDff* dff,
  const TpgNode* fanin
) : TpgPPO{output_id, fanin},
    mDff(dff)
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
