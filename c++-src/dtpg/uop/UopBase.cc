
/// @file UopBase.cc
/// @brief UopBase の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "UopBase.h"
#include "TpgFaultMgr.h"
#include "TpgFault.h"


BEGIN_NAMESPACE_DRUID

// @brief 'base' タイプを生成する．
UntestOp*
new_UopBase(
  TpgFaultMgr& fmgr
)
{
  return new UopBase{fmgr};
}


//////////////////////////////////////////////////////////////////////
// クラス UopBase
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
UopBase::UopBase(
  TpgFaultMgr& fmgr
) : mMgr{fmgr}
{
}

// @brief デストラクタ
UopBase::~UopBase()
{
}

// @brief テスト不能故障と判定された時の処理
void
UopBase::operator()(
  const TpgFault& f
)
{
  mMgr.set_status(f, FaultStatus::Untestable);
}

END_NAMESPACE_DRUID
