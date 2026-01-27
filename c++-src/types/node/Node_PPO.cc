
/// @file Node_PPO.cc
/// @brief Node_PPO の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "Node_PPO.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス NodeRep
//////////////////////////////////////////////////////////////////////

// @grief 出力ノードを作る．
NodeRep*
NodeRep::new_output(
  SizeType id,
  SizeType output_id,
  const NodeRep* fanin
)
{
  return new Node_Output(id, output_id, fanin);
}

// @brief DFFの入力ノードを作る．
NodeRep*
NodeRep::new_dff_input(
  SizeType id,
  SizeType output_id,
  SizeType dff_id,
  const NodeRep* fanin
)
{
  return new Node_DffInput(id, output_id, dff_id, fanin);
}


//////////////////////////////////////////////////////////////////////
// クラス Node_PPO
//////////////////////////////////////////////////////////////////////

// @brief 出力タイプの時 true を返す．
bool
Node_PPO::is_ppo() const
{
  return true;
}

// @brief 外部出力タイプの時に出力番号を返す．
SizeType
Node_PPO::output_id() const
{
  return mOutputId;
}

// @brief TFIサイズの昇順に並べた時の出力番号を返す．
SizeType
Node_PPO::output_id2() const
{
  return mOutputId2;
}

// @brief ゲートタイプを得る．
PrimType
Node_PPO::gate_type() const
{
  return PrimType::Buff;
}

// @brief 出力番号2をセットする．
void
Node_PPO::set_output_id2(
  SizeType id
)
{
  mOutputId2 = id;
}


//////////////////////////////////////////////////////////////////////
// クラス Node_Output
//////////////////////////////////////////////////////////////////////

// @brief 外部出力タイプの時 true を返す．
bool
Node_Output::is_primary_output() const
{
  return true;
}


//////////////////////////////////////////////////////////////////////
// クラス Node_DffInput
//////////////////////////////////////////////////////////////////////

// @brief DFF の入力に接続している外部出力タイプの時 true を返す．
bool
Node_DffInput::is_dff_input() const
{
  return true;
}

// @brief 接続している DFF を返す．
SizeType
Node_DffInput::dff_id() const
{
  return mDffId;
}

// @brief DFFに関する相方のノードを返す．
const NodeRep*
Node_DffInput::alt_node() const
{
  return mAltNode;
}

// @brief alt_node を設定する．
void
Node_DffInput::set_alt_node(
  const NodeRep* node
)
{
  mAltNode = node;
}

END_NAMESPACE_DRUID
