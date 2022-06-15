
/// @file TpgOutput.cc
/// @brief TpgOutput の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "TpgOutput.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス TpgOutput
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
TpgOutput::TpgOutput(
  int output_id,
  TpgNode* fanin
) : TpgPPO{output_id, fanin}
{
}

// @brief 外部出力タイプの時 true を返す．
bool
TpgOutput::is_primary_output() const
{
  return true;
}

END_NAMESPACE_DRUID
