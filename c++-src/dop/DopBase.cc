
/// @file DopBase.cc
/// @brief DopBase の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "DopBase.h"
#include "TpgFaultMgr.h"


BEGIN_NAMESPACE_DRUID

// @brief 'base' タイプを生成する．
DetectOp*
new_DopBase(
  TpgFaultMgr& fmgr
)
{
  return new DopBase{fmgr};
}


//////////////////////////////////////////////////////////////////////
// クラス DopBase
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
DopBase::DopBase(
  TpgFaultMgr& fmgr
) : mFaultMgr{fmgr}
{
}

// @brief デストラクタ
DopBase::~DopBase()
{
}

// @brief テストパタンが見つかった時の処理
void
DopBase::operator()(
  const TpgFault& f,
  const TestVector& tv
)
{
  mFaultMgr.set_status(f, FaultStatus::Detected);
}

END_NAMESPACE_DRUID
