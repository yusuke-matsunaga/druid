
/// @file NodeRep.cc
/// @brief NodeRep の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "NodeRep.h"
#include "types/TpgFFR.h"
#include "types/Val3.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス TpgNode
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
NodeRep::NodeRep(
  SizeType id,
  const std::vector<const NodeRep*>& fanin_list
) : mId{id},
    mFaninList{fanin_list}
{
}

// @brief 外部入力タイプの時 true を返す．
bool
NodeRep::is_primary_input() const
{
  return false;
}

// @brief 外部出力タイプの時 true を返す．
bool
NodeRep::is_primary_output() const
{
  return false;
}

// @brief DFF の入力に接続している出力タイプの時 true を返す．
bool
NodeRep::is_dff_input() const
{
  return false;
}

// @brief DFF の出力に接続している入力タイプの時 true を返す．
bool
NodeRep::is_dff_output() const
{
  return false;
}

// @brief 入力タイプの時 true を返す．
//
// 具体的には is_input() || is_dff_output()
bool
NodeRep::is_ppi() const
{
  return false;
}

// @brief 出力タイプの時 true を返す．
//
// 具体的には is_output() || is_dff_input()
bool
NodeRep::is_ppo() const
{
  return false;
}

// @brief logic タイプの時 true を返す．
bool
NodeRep::is_logic() const
{
  return false;
}

// @brief 外部入力タイプの時に入力番号を返す．
SizeType
NodeRep::input_id() const
{
  throw std::logic_error{"Not a PPI"};
}

// @brief 外部出力タイプの時に出力番号を返す．
SizeType
NodeRep::output_id() const
{
  throw std::logic_error{"Not a PPO"};
}

// @brief TFIサイズの昇順に並べた時の出力番号を返す．
SizeType
NodeRep::output_id2() const
{
  throw std::logic_error{"Not a PPO"};
}

// @brief 接続している DFF 番号を返す．
SizeType
NodeRep::dff_id() const
{
  throw std::logic_error{"Not a FF type"};
}

// @brief DFFに関する相方のノードを返す．
const NodeRep*
NodeRep::alt_node() const
{
  throw std::logic_error{"Not a FF type"};
}

// @brief ゲートタイプを得る．
//
// is_logic() が false の場合の返り値は不定
PrimType
NodeRep::gate_type() const
{
  throw std::logic_error{"Not a LOGIC type"};
}

// @brief controling value を得る．
Val3
NodeRep::cval() const
{
  return Val3::_X;
}

// @brief noncontroling valueを得る．
Val3
NodeRep::nval() const
{
  return Val3::_X;
}

// @brief controling output value を得る．
Val3
NodeRep::coval() const
{
  ASSERT_COND( is_ppo() );

  return Val3::_X;
}

// @brief noncontroling output value を得る．
Val3
NodeRep::noval() const
{
  ASSERT_COND( is_ppo() );

  return Val3::_X;
}

// @brief 出力番号2をセットする．
void
NodeRep::set_output_id2(
  SizeType id
)
{
  throw std::logic_error{"Not a PPO"};
}

// @brief immediate dominator をセットする．
void
NodeRep::set_imm_dom(
  const NodeRep* dom
)
{
  mImmDom = dom;
}

// @brief DFF に関連したノードの相方のノードをセットする．
void
NodeRep::set_alt_node(
  const NodeRep* node
)
{
  throw std::logic_error{"not a DFF-input/output"};
}

END_NAMESPACE_DRUID
