
/// @file TpgPPO.cc
/// @brief TpgPPO の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "TpgPPO.h"
#include "TpgDFF.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス TpgPPO
//////////////////////////////////////////////////////////////////////

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
  const TpgNode* fanin
) : TpgPPO{fanin}
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
  SizeType dff_id,
  const TpgNode* fanin
) : TpgPPO{fanin},
    mDffId{dff_id}
{
}

// @brief DFF の入力に接続している外部出力タイプの時 true を返す．
bool
TpgDffInput::is_dff_input() const
{
  return true;
}

// @brief 接続している DFF を返す．
SizeType
TpgDffInput::dff_id() const
{
  return mDffId;
}

// @brief DFFに関する相方のノードを返す．
const TpgNode*
TpgDffInput::alt_node() const
{
  return mAltNode;
}

END_NAMESPACE_DRUID
