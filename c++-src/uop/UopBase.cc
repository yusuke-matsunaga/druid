
/// @file UopBase.cc
/// @brief UopBase の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "UopBase.h"
#include "FaultStatusMgr.h"
#include "TpgFault.h"


BEGIN_NAMESPACE_DRUID

// @brief 'base' タイプを生成する．
UntestOp*
new_UopBase(
  FaultStatusMgr& fsmgr
)
{
  return new UopBase{fsmgr};
}


//////////////////////////////////////////////////////////////////////
// クラス UopBase
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
UopBase::UopBase(
  FaultStatusMgr& fsmgr
) : mMgr{fsmgr}
{
}

// @brief デストラクタ
UopBase::~UopBase()
{
}

// @brief テスト不能故障と判定された時の処理
void
UopBase::operator()(
  const TpgFault* f
)
{
  mMgr.set(f, FaultStatus::Untestable);
}

END_NAMESPACE_DRUID
