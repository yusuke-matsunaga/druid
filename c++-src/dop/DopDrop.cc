
/// @file DopDrop.cc
/// @brief DopDrop の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "DopDrop.h"
#include "TpgFaultMgr.h"
#include "Fsim.h"
#include "TpgFault.h"
#include "TestVector.h"


BEGIN_NAMESPACE_DRUID

// @brief 'drop' タイプを生成する．
DetectOp*
new_DopDrop(
  TpgFaultMgr& fmgr,
  Fsim& fsim
)
{
  return new DopDrop{fmgr, fsim};
}


//////////////////////////////////////////////////////////////////////
// クラス DopDrop
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
DopDrop::DopDrop(
  TpgFaultMgr& fmgr,
  Fsim& fsim
) : mFaultMgr{fmgr},
    mFsim{fsim}
{
}

// @brief デストラクタ
DopDrop::~DopDrop()
{
}

// @brief テストパタンが見つかった時の処理
void
DopDrop::operator()(
  const TpgFault& f,
  const TestVector& tv
)
{
  mFsim.set_skip(f);
  SizeType n = mFsim.sppfp(tv); // n は未使用
  for ( auto f: mFsim.det_fault_list() ) {
    ASSERT_COND( mFaultMgr.get_status(f) != FaultStatus::Untestable );

    mFaultMgr.set_status(f, FaultStatus::Detected);
    mFsim.set_skip(f);
  }
}

END_NAMESPACE_DRUID
