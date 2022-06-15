
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
TpgDffPreset::TpgDffPreset(
  const TpgDff* dff,
  TpgNode* fanin
) : TpgDffControl{dff, fanin}
{
}

// @brief DFF のプリセット端子に接続している力タイプの時 true を返す．
bool
TpgDffPreset::is_dff_preset() const
{
  return true;
}

END_NAMESPACE_DRUID
