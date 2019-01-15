
/// @file TpgOutput.cc
/// @brief TpgOutput の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "TpgOutput.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス TpgOutput
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
// @param[in] output_id 出力番号
// @param[in] fanin ファンインのノード
TpgOutput::TpgOutput(int id,
		     int output_id,
		     TpgNode* fanin) :
  TpgPPO(id, output_id, fanin)
{
}

// @brief デストラクタ
TpgOutput::~TpgOutput()
{
}

// @brief 外部出力タイプの時 true を返す．
bool
TpgOutput::is_primary_output() const
{
  return true;
}

END_NAMESPACE_DRUID
