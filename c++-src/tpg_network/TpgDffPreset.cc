
/// @file TpgDffPreset.cc
/// @brief TpgDffPreset の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "TpgDffPreset.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス TpgDffPreset
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
// @param[in] dff 接続しているDFF
// @param[in] fanin ファンインのノード
TpgDffPreset::TpgDffPreset(int id,
			   const TpgDff* dff,
			   TpgNode* fanin) :
  TpgDffControl(id, dff, fanin)
{
}

// @brief デストラクタ
TpgDffPreset::~TpgDffPreset()
{
}

// @brief DFF のプリセット端子に接続している力タイプの時 true を返す．
bool
TpgDffPreset::is_dff_preset() const
{
  return true;
}

END_NAMESPACE_DRUID
