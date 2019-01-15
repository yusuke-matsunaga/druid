
/// @file TpgPPI.cc
/// @brief TpgPPI の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "TpgPPI.h"
#include "GateType.h"



BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス TpgPPI
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
// @param[in] input_id 入力番号
TpgPPI::TpgPPI(int id,
	       int input_id) :
  TpgNode(id),
  mInputId(input_id)
{
}

// @brief デストラクタ
TpgPPI::~TpgPPI()
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
int
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

// @brief ファンインのリストを得る．
Array<const TpgNode*>
TpgPPI::fanin_list() const
{
  return Array<const TpgNode*>(nullptr, 0, 0);
}

// @brief ファンイン数を得る．
int
TpgPPI::fanin_num() const
{
  return 0;
}

// @brief ファンインを得る．
// @param[in] pos 位置番号 ( 0 <= pos < fanin_num() )
TpgNode*
TpgPPI::fanin(int pos) const
{
  ASSERT_NOT_REACHED;
  return nullptr;
}

END_NAMESPACE_DRUID
