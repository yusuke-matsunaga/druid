
/// @file TpgPPI.cc
/// @brief TpgPPI の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "TpgPPI.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス TpgPPI
//////////////////////////////////////////////////////////////////////

// @brief 入力タイプの時 true を返す．
//
// 具体的には is_primary_input() || is_dff_output()
bool
TpgPPI::is_ppi() const
{
  return true;
}

// @brief 外部入力タイプの時に入力番号を返す．
//
// node = TpgNetwork::input(node->input_id()
// の関係を満たす．
// is_input() が false の場合の返り値は不定
SizeType
TpgPPI::input_id() const
{
  return mInputId;
}

// @brief ゲートタイプを得る．
PrimType
TpgPPI::gate_type() const
{
  return PrimType::None;
}


//////////////////////////////////////////////////////////////////////
// クラス TpgInput
//////////////////////////////////////////////////////////////////////

// @brief 外部入力タイプの時 true を返す．
bool
TpgInput::is_primary_input() const
{
  return true;
}


//////////////////////////////////////////////////////////////////////
// クラス TpgDffOutput
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
TpgDffOutput::TpgDffOutput(
  SizeType dff_id
) : mDffId{dff_id}
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
SizeType
TpgDffOutput::dff_id() const
{
  return mDffId;
}

// @brief DFFに関する相方のノードを返す．
const TpgNode*
TpgDffOutput::alt_node() const
{
  return mAltNode;
}

END_NAMESPACE_DRUID
