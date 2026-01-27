
/// @file TpgNode.cc
/// @brief TpgNode の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "types/TpgNode.h"
#include "types/TpgNodeList.h"
#include "types/TpgFFR.h"
#include "types/Val3.h"
#include "NodeRep.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス TpgNode
//////////////////////////////////////////////////////////////////////

// @brief 外部入力タイプの時 true を返す．
bool
TpgNode::is_primary_input() const
{
  return _node()->is_primary_input();
}

// @brief 外部出力タイプの時 true を返す．
bool
TpgNode::is_primary_output() const
{
  return _node()->is_primary_output();
}

// @brief DFF の入力に接続している出力タイプの時 true を返す．
bool
TpgNode::is_dff_input() const
{
  return _node()->is_dff_input();
}

// @brief DFF の出力に接続している入力タイプの時 true を返す．
bool
TpgNode::is_dff_output() const
{
  return _node()->is_dff_output();
}

// @brief 入力タイプの時 true を返す．
//
// 具体的には is_input() || is_dff_output()
bool
TpgNode::is_ppi() const
{
  return _node()->is_ppi();
}

// @brief 出力タイプの時 true を返す．
//
// 具体的には is_output() || is_dff_input()
bool
TpgNode::is_ppo() const
{
  return _node()->is_ppo();
}

// @brief logic タイプの時 true を返す．
bool
TpgNode::is_logic() const
{
  return _node()->is_logic();
}

// @brief 外部入力タイプの時に入力番号を返す．
SizeType
TpgNode::input_id() const
{
  return _node()->input_id();
}

// @brief 外部出力タイプの時に出力番号を返す．
SizeType
TpgNode::output_id() const
{
  return _node()->output_id();
}

// @brief TFIサイズの昇順に並べた時の出力番号を返す．
SizeType
TpgNode::output_id2() const
{
  return _node()->output_id2();
}

// @brief 接続している DFF 番号を返す．
SizeType
TpgNode::dff_id() const
{
  return _node()->dff_id();
}

// @brief DFFに関する相方のノードを返す．
TpgNode
TpgNode::alt_node() const
{
  return node(_node()->alt_node());
}

// @brief ゲートタイプを得る．
PrimType
TpgNode::gate_type() const
{
  return _node()->gate_type();
}

// @brief controling value を得る．
Val3
TpgNode::cval() const
{
  return _node()->cval();
}

// @brief noncontroling valueを得る．
Val3
TpgNode::nval() const
{
  return _node()->nval();
}

// @brief controling output value を得る．
Val3
TpgNode::coval() const
{
  return _node()->coval();
}

// @brief noncontroling output value を得る．
Val3
TpgNode::noval() const
{
  return _node()->noval();
}

// @brief ファンイン数を得る．
SizeType
TpgNode::fanin_num() const
{
  return _node()->fanin_num();
}

// @brief ファンインを得る．
TpgNode
TpgNode::fanin(
  SizeType index
) const
{
  return node(_node()->fanin(index));
}

// @brief ファンインのリストを得る．
TpgNodeList
TpgNode::fanin_list() const
{
  return node_list(_node()->fanin_list());
}

// @brief ファンアウト数を得る．
SizeType
TpgNode::fanout_num() const
{
  return _node()->fanout_num();
}

// @brief ファンアウトを得る．
TpgNode
TpgNode::fanout(
  SizeType index
) const
{
  return node(_node()->fanout(index));
}

// @brief ファンアウトのリストを得る．
TpgNodeList
TpgNode::fanout_list() const
{
  return node_list(_node()->fanout_list());
}

// @brief FFR の根のノードを得る．
TpgNode
TpgNode::ffr_root() const
{
  return node(_node()->ffr_root());
}

// @brief MFFCの根のノードを得る．
TpgNode
TpgNode::mffc_root() const
{
  return node(_node()->mffc_root());
}

// @brief 直近の dominator を得る．
TpgNode
TpgNode::imm_dom() const
{
  auto dom = _node()->imm_dom();
  if ( dom == nullptr ) {
    return TpgNode{};
  }
  return node(dom);
}

END_NAMESPACE_DRUID
