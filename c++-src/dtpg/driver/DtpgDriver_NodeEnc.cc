
/// @file DtpgDriver_NodeEnc.cc
/// @brief DtpgDriver_NodeEnc の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgDriver_NodeEnc.h"
#include "types/TpgFault.h"
#include "types/TestVector.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス DtpgDriver_NodeEnc
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
DtpgDriver_NodeEnc::DtpgDriver_NodeEnc(
  const TpgNode& node,
  const JsonValue& option
) : DtpgDriver_Enc(node, option)
{
}

// @brief デストラクタ
DtpgDriver_NodeEnc::~DtpgDriver_NodeEnc()
{
}

// @brief 故障の伝搬条件を得る．
AssignList
DtpgDriver_NodeEnc::fault_prop_condition(
  const TpgFault& fault
)
{
  return fault.excitation_condition();
}

END_NAMESPACE_DRUID
