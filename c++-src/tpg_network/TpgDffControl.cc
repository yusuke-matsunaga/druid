
/// @file TpgDffControl.cc
/// @brief TpgDffControl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "TpgDffControl.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス TpgDffControl
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
// @param[in] dff 接続しているDFF
TpgDffControl::TpgDffControl(int id,
			     const TpgDff* dff,
			     TpgNode* fanin) :
  TpgNode(id),
  mFanin(fanin),
  mDff(dff)
{
}

// @brief デストラクタ
TpgDffControl::~TpgDffControl()
{
}

// @brief ファンインのリストを得る．
Array<const TpgNode*>
TpgDffControl::fanin_list() const
{
  return Array<const TpgNode*>(const_cast<const TpgNode**>(&mFanin), 0, 1);
}

// @brief ファンイン数を得る．
int
TpgDffControl::fanin_num() const
{
  return 1;
}

// @brief ファンインを得る．
// @param[in] pos 位置番号 ( 0 <= pos < fanin_num() )
const TpgNode*
TpgDffControl::fanin(int pos) const
{
  ASSERT_COND( pos == 0 );

  return mFanin;
}

// @brief 接続している DFF を返す．
//
// is_dff_input() | is_dff_output() | is_dff_clock() | is_dff_clear() | is_dff_preset()
// の時に意味を持つ．
const TpgDff*
TpgDffControl::dff() const
{
  return mDff;
}

END_NAMESPACE_DRUID
