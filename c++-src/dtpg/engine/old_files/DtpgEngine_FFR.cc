
/// @file DtpgEngine_FFR.cc
/// @brief DtpgEngine_FFR の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgEngine_FFR.h"
#include "TpgFFR.h"
#include "TpgFault.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
DtpgEngine_FFR::DtpgEngine_FFR(
  const TpgNetwork& network,
  const TpgFFR* ffr,
  const JsonValue& option
) : DtpgEngine{network, ffr->root(), option}
{
}

// @brief デストラクタ
DtpgEngine_FFR::~DtpgEngine_FFR()
{
}

// @brief 故障伝搬の起点ノードを返す．
const TpgNode*
DtpgEngine_FFR::fault_origin(
  const TpgFault* fault
)
{
  return fault->ffr_root();
}

// @brief gen_pattern() で用いる検出条件を作る．
AssignList
DtpgEngine_FFR::fault_condition(
  const TpgFault* fault
)
{
  return fault->ffr_propagate_condition();
}

END_NAMESPACE_DRUID
