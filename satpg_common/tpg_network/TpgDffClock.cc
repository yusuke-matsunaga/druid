
/// @file TpgDffClock.cc
/// @brief TpgDffClock の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "TpgDffClock.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
// クラス TpgDffClock
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
// @param[in] dff 接続しているDFF
// @param[in] fanin ファンインのノード
TpgDffClock::TpgDffClock(ymuint id,
			 TpgDff* dff,
			 TpgNode* fanin) :
  TpgDffControl(id, dff, fanin)
{
}

// @brief デストラクタ
TpgDffClock::~TpgDffClock()
{
}

// @brief DFF のクロック端子に接続している出力タイプの時 true を返す．
bool
TpgDffClock::is_dff_clock() const
{
  return true;
}

END_NAMESPACE_YM_SATPG
