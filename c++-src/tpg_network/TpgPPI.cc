
/// @file TpgPPI.cc
/// @brief TpgPPI の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "TpgPPI.h"
#include "TpgInput.h"
#include "TpgDffOutput.h"
#include "GateType.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス TpgPPI
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
TpgPPI::TpgPPI(
  SizeType input_id,
  SizeType fanout_num
) : TpgNode{{}, fanout_num},
    mInputId{input_id}
{
}

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
//
// - is_logic() が true の時はゲートタイプを返す．
// - is_ppi() が true の時は GateType::Input を返す．
// - is_ppo() が true の時は GateType::Buff を返す．
// - それ以外の返り値は不定
GateType
TpgPPI::gate_type() const
{
  return GateType::Input;
}


//////////////////////////////////////////////////////////////////////
// クラス TpgInput
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
TpgInput::TpgInput(
  SizeType input_id,
  SizeType fanout_num
) : TpgPPI{input_id, fanout_num}
{
}

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
  SizeType input_id,
  SizeType fanout_num,
  const TpgDff* dff
) : TpgPPI{input_id, fanout_num},
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
