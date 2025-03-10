
/// @file DtpgDriver_FFREncDriver.cc
/// @brief DtpgDriver_FFREncDriver の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgDriver_FFREnc.h"
#include "TpgNetwork.h"
#include "TpgFFR.h"
#include "TpgFault.h"
#include "TestVector.h"
#include "ym/Timer.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス DtpgDriver_FFREncDriver
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
DtpgDriver_FFREnc::DtpgDriver_FFREnc(
  const TpgNetwork& network,
  const TpgFFR* ffr,
  const JsonValue& option
) : DtpgDriver_Enc(network, ffr->root(), option)
{
}

// @brief デストラクタ
DtpgDriver_FFREnc::~DtpgDriver_FFREnc()
{
}

// @brief 故障の伝搬条件を得る．
AssignList
DtpgDriver_FFREnc::fault_prop_condition(
  const TpgFault* fault
)
{
  return fault->ffr_propagate_condition();
}

END_NAMESPACE_DRUID
