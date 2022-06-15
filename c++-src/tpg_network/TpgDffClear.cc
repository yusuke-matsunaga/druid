
/// @file TpgDffClear.cc
/// @brief TpgDffClear の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "TpgDffClear.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス TpgDffClear
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
TpgDffClear::TpgDffClear(
  const TpgDff* dff,
  TpgNode* fanin
) : TpgDffControl{dff, fanin}
{
}

// @brief DFF のクリア端子に接続している力タイプの時 true を返す．
bool
TpgDffClear::is_dff_clear() const
{
  return true;
}

END_NAMESPACE_DRUID
