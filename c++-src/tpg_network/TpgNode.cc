
/// @file TpgNode.cc
/// @brief TpgNode の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "TpgNode.h"
#include "TpgFFR.h"
#include "GateType.h"
#include "Val3.h"


BEGIN_NAMESPACE_DRUID

// @brief GateType のストリーム演算子
ostream&
operator<<(
  ostream& s,
  GateType gate_type
)
{
  switch ( gate_type ) {
  case GateType::Const0: s << "CONST-0"; break;
  case GateType::Const1: s << "CONST-1"; break;
  case GateType::Input:  s << "INPUT"; break;
  case GateType::Buff:   s << "BUFF"; break;
  case GateType::Not:    s << "NOT"; break;
  case GateType::And:    s << "AND"; break;
  case GateType::Nand:   s << "NAND"; break;
  case GateType::Or:     s << "OR"; break;
  case GateType::Nor:    s << "NOR"; break;
  case GateType::Xor:    s << "XOR"; break;
  case GateType::Xnor:   s << "XNOR"; break;
  default:               s << "---"; break;
  }
  return s;
}


//////////////////////////////////////////////////////////////////////
// クラス TpgNode
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
TpgNode::TpgNode(
  const vector<const TpgNode*>& fanin_list,
  SizeType fanout_num
) : mFaninList{fanin_list}
{
  mFanoutList.reserve(fanout_num);
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
int
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
int
TpgNode::output_id() const
{
  ASSERT_NOT_REACHED;

  return 0;
}

// @brief TFIサイズの昇順に並べた時の出力番号を返す．
int
TpgNode::output_id2() const
{
  ASSERT_NOT_REACHED;

  return 0;
}

// @brief 接続している DFF を返す．
//
// is_dff_input() | is_dff_output() | is_dff_clock() | is_dff_clear() | is_dff_preset()
// の時に意味を持つ．
const TpgDff*
TpgNode::dff() const
{
  ASSERT_NOT_REACHED;

  return nullptr;
}

// @brief ゲートタイプを得る．
//
// is_logic() が false の場合の返り値は不定
GateType
TpgNode::gate_type() const
{
  ASSERT_NOT_REACHED;

  return GateType::Const0;
}

// @brief controling value を得る．
// @note ない場合は Val3::_X を返す．
Val3
TpgNode::cval() const
{
  ASSERT_COND( is_ppo() );

  return Val3::_X;
}

// @brief noncontroling valueを得る．
// @note ない場合は kValX を返す．
Val3
TpgNode::nval() const
{
  ASSERT_COND( is_ppo() );

  return Val3::_X;
}

// @brief controling output value を得る．
// @note ない場合は kValX を返す．
Val3
TpgNode::coval() const
{
  ASSERT_COND( is_ppo() );

  return Val3::_X;
}

// @brief noncontroling output value を得る．
// @note ない場合は kValX を返す．
Val3
TpgNode::noval() const
{
  ASSERT_COND( is_ppo() );

  return Val3::_X;
}

// @brief 出力番号2をセットする．
// @param[in] id セットする番号
//
// 出力ノード以外では無効
void
TpgNode::set_output_id2(
  int id
)
{
  ASSERT_NOT_REACHED;
}

// @brief ファンアウトを設定する．
void
TpgNode::add_fanout(
  const TpgNode* fo_node
)
{
  mFanoutList.push_back(fo_node);
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
