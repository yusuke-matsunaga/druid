
/// @file DtpgDriver_FFREncDriver.cc
/// @brief DtpgDriver_FFREncDriver の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgDriver_FFREnc.h"
#include "types/TpgNetwork.h"
#include "types/TpgFFR.h"
#include "types/TpgFault.h"
#include "types/TestVector.h"
#include "ym/Timer.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス DtpgDriver_FFREncDriver
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
DtpgDriver_FFREnc::DtpgDriver_FFREnc(
  const TpgFFR& ffr,
  const JsonValue& option
) : DtpgDriver_Enc(ffr.root(), option)
{
}

// @brief デストラクタ
DtpgDriver_FFREnc::~DtpgDriver_FFREnc()
{
}

// @brief 故障の伝搬条件を得る．
AssignList
DtpgDriver_FFREnc::fault_prop_condition(
  const TpgFault& fault
)
{
  return fault.ffr_propagate_condition();
}

// @biref 値割当が必要なノードのリストを返す．
TpgNodeList
DtpgDriver_FFREnc::get_aux_side_inputs(
  const TpgFault& fault
)
{
  return fault.aux_side_inputs();
}

END_NAMESPACE_DRUID
