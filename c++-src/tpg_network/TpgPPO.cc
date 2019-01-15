
/// @file TpgPPO.cc
/// @brief TpgPPO の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "TpgPPO.h"
#include "GateType.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス TpgPPO
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
// @param[in] output_id 出力番号
// @param[in] fanin ファンインのノード
TpgPPO::TpgPPO(int id,
	       int output_id,
	       TpgNode* fanin) :
  TpgNode(id),
  mOutputId(output_id),
  mFanin(fanin)
{
  mOutputId2 = 0;
}

// @brief デストラクタ
TpgPPO::~TpgPPO()
{
}

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
int
TpgPPO::output_id() const
{
  return mOutputId;
}

// @brief TFIサイズの昇順に並べた時の出力番号を返す．
int
TpgPPO::output_id2() const
{
  return mOutputId2;
}

// @brief ゲートタイプを得る．
//
// is_logic() が false の場合の返り値は不定
GateType
TpgPPO::gate_type() const
{
  return GateType::Buff;
}

// @brief ファンインのリストを得る．
Array<const TpgNode*>
TpgPPO::fanin_list() const
{
  return Array<const TpgNode*>(const_cast<const TpgNode**>(&mFanin), 0, 1);
}

// @brief ファンイン数を得る．
int
TpgPPO::fanin_num() const
{
  return 1;
}

// @brief ファンインを得る．
// @param[in] pos 位置番号 ( 0 <= pos < fanin_num() )
TpgNode*
TpgPPO::fanin(int pos) const
{
  ASSERT_COND( pos == 0 );

  return mFanin;
}

// @brief 出力番号2をセットする．
// @param[in] id セットする番号
//
// 出力ノード以外では無効
void
TpgPPO::set_output_id2(int id)
{
  mOutputId2 = id;
}

END_NAMESPACE_DRUID
