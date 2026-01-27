
/// @file Node_PPI.cc
/// @brief Node_PPI の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "Node_PPI.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス NodeRep
//////////////////////////////////////////////////////////////////////

// @brief 入力ノードを作る．
NodeRep*
NodeRep::new_input(
  SizeType id,
  SizeType input_id
)
{
  return new Node_Input(id, input_id);
}

// @brief DFFの出力ノードを作る．
NodeRep*
NodeRep::new_dff_output(
  SizeType id,
  SizeType input_id,
  SizeType dff_id
)
{
  return new Node_DffOutput(id, input_id, dff_id);
}


//////////////////////////////////////////////////////////////////////
// クラス Node_PPI
//////////////////////////////////////////////////////////////////////

// @brief 入力タイプの時 true を返す．
//
// 具体的には is_primary_input() || is_dff_output()
bool
Node_PPI::is_ppi() const
{
  return true;
}

// @brief 外部入力タイプの時に入力番号を返す．
SizeType
Node_PPI::input_id() const
{
  return mInputId;
}

// @brief ゲートタイプを得る．
PrimType
Node_PPI::gate_type() const
{
  return PrimType::None;
}


//////////////////////////////////////////////////////////////////////
// クラス Node_Input
//////////////////////////////////////////////////////////////////////

// @brief 外部入力タイプの時 true を返す．
bool
Node_Input::is_primary_input() const
{
  return true;
}


//////////////////////////////////////////////////////////////////////
// クラス Node_DffOutput
//////////////////////////////////////////////////////////////////////

// @brief DFF の出力に接続している外部入力タイプの時 true を返す．
bool
Node_DffOutput::is_dff_output() const
{
  return true;
}

// @brief 接続している DFF を返す．
SizeType
Node_DffOutput::dff_id() const
{
  return mDffId;
}

// @brief DFFに関する相方のノードを返す．
const NodeRep*
Node_DffOutput::alt_node() const
{
  return mAltNode;
}

// @brief alt_node を設定する．
void
Node_DffOutput::set_alt_node(
  const NodeRep* node
)
{
  mAltNode = node;
}

END_NAMESPACE_DRUID
