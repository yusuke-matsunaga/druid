
/// @file DopBase.cc
/// @brief DopBase の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2024 Yusuke Matsunaga
/// All rights reserved.

#include "DopBase.h"


BEGIN_NAMESPACE_DRUID

// @brief 'base' タイプを生成する．
DetectOp*
new_DopBase(
  DtpgMgr& mgr
)
{
  return new DopBase{mgr};
}


//////////////////////////////////////////////////////////////////////
// クラス DopBase
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
DopBase::DopBase(
  DtpgMgr& mgr
) : mMgr{mgr}
{
}

// @brief デストラクタ
DopBase::~DopBase()
{
}

// @brief テストパタンが見つかった時の処理
void
DopBase::operator()(
  const TpgFault* f,
  const TestVector& tv
)
{

}

END_NAMESPACE_DRUID
