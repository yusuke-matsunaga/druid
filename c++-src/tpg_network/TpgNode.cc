
/// @file TpgNode.cc
/// @brief TpgNode の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "TpgNode.h"
#include "TpgDFF.h"
#include "TpgFFR.h"
#include "Val3.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス TpgNode
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
TpgNode::TpgNode(
  const vector<const TpgNode*>& fanin_list
) : mFaninList{fanin_list}
{
}

// @brief 外部入力タイプの時 true を返す．
bool
TpgNode::is_primary_input() const
{
  return false;
}

// @brief 外部出力タイプの時 true を返す．
bool
TpgNode::is_primary_output() const
{
  return false;
}

// @brief DFF の入力に接続している出力タイプの時 true を返す．
bool
TpgNode::is_dff_input() const
{
  return false;
}

// @brief DFF の出力に接続している入力タイプの時 true を返す．
bool
TpgNode::is_dff_output() const
{
  return false;
}

// @brief DFF のクロック端子に接続している出力タイプの時 true を返す．
bool
TpgNode::is_dff_clock() const
{
  return false;
}

// @brief DFF のクリア端子に接続している出力タイプの時 true を返す．
bool
TpgNode::is_dff_clear() const
{
  return false;
}

// @brief DFF のプリセット端子に接続している出力タイプの時 true を返す．
bool
TpgNode::is_dff_preset() const
{
  return false;
}

// @brief 入力タイプの時 true を返す．
//
// 具体的には is_input() || is_dff_output()
bool
TpgNode::is_ppi() const
{
  return false;
}

// @brief 出力タイプの時 true を返す．
//
// 具体的には is_output() || is_dff_input()
bool
TpgNode::is_ppo() const
{
  return false;
}

// @brief logic タイプの時 true を返す．
bool
TpgNode::is_logic() const
{
  return false;
}

// @brief 外部入力タイプの時に入力番号を返す．
//
// node = TpgNetwork::ppi(node->input_id()
// の関係を満たす．
// is_ppi() が false の場合の返り値は不定
SizeType
TpgNode::input_id() const
{
  ASSERT_NOT_REACHED;

  return 0;
}

// @brief 外部出力タイプの時に出力番号を返す．
//
// node = TpgNetwork::ppo(node->output_id())
// の関係を満たす．
// is_ppo() が false の場合の返り値は不定
SizeType
TpgNode::output_id() const
{
  ASSERT_NOT_REACHED;

  return 0;
}

// @brief TFIサイズの昇順に並べた時の出力番号を返す．
SizeType
TpgNode::output_id2() const
{
  ASSERT_NOT_REACHED;

  return 0;
}

// @brief 接続している DFF 番号を返す．
SizeType
TpgNode::dff_id() const
{
  ASSERT_NOT_REACHED;

  return 0;
}

// @brief DFFに関する相方のノードを返す．
const TpgNode*
TpgNode::alt_node() const
{
  ASSERT_NOT_REACHED;

  return nullptr;
}

// @brief ゲートタイプを得る．
//
// is_logic() が false の場合の返り値は不定
PrimType
TpgNode::gate_type() const
{
  ASSERT_NOT_REACHED;

  return PrimType::None;
}

// @brief controling value を得る．
Val3
TpgNode::cval() const
{
  ASSERT_COND( is_ppo() );

  return Val3::_X;
}

// @brief noncontroling valueを得る．
Val3
TpgNode::nval() const
{
  ASSERT_COND( is_ppo() );

  return Val3::_X;
}

// @brief controling output value を得る．
Val3
TpgNode::coval() const
{
  ASSERT_COND( is_ppo() );

  return Val3::_X;
}

// @brief noncontroling output value を得る．
Val3
TpgNode::noval() const
{
  ASSERT_COND( is_ppo() );

  return Val3::_X;
}

// @brief side-input の値を得る．
Val3
TpgNode::side_val() const
{
  return Val3::_X;
}

// @brief 出力番号2をセットする．
// @param[in] id セットする番号
//
// 出力ノード以外では無効
void
TpgNode::set_output_id2(
  SizeType id
)
{
  ASSERT_NOT_REACHED;
}

// @brief immediate dominator をセットする．
void
TpgNode::set_imm_dom(
  const TpgNode* dom
)
{
  mImmDom = dom;
}

END_NAMESPACE_DRUID
