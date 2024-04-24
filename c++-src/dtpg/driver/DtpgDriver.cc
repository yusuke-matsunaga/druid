
/// @file DtpgDriver.cc
/// @brief DtpgDriver の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgDriver.h"
#include "DtpgDriverImpl.h"
#include "TestVector.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
DtpgDriver::DtpgDriver(
  DtpgDriverImpl* impl
) : mImpl{impl}
{
}

// @brief デストラクタ
DtpgDriver::~DtpgDriver()
{
}

// @brief 故障を検出する条件を求める．
SatBool3
DtpgDriver::solve(
  const TpgFault* fault
)
{
  return mImpl->solve(fault);
}

// @brief 故障のテストパタンを求める．
TestVector
DtpgDriver::gen_pattern(
  const TpgFault* fault
)
{
  return mImpl->gen_pattern(fault);
}

END_NAMESPACE_DRUID
