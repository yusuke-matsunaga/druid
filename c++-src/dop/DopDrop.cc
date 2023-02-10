
/// @file DopDrop.cc
/// @brief DopDrop の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "DopDrop.h"
#include "FaultStatusMgr.h"
#include "Fsim.h"
#include "TpgFault.h"
#include "TestVector.h"


BEGIN_NAMESPACE_DRUID

// @brief 'drop' タイプを生成する．
DetectOp*
new_DopDrop(
  FaultStatusMgr& fsmgr,
  Fsim& fsim
)
{
  return new DopDrop{fsmgr, fsim};
}


//////////////////////////////////////////////////////////////////////
// クラス DopDrop
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
DopDrop::DopDrop(
  FaultStatusMgr& fsmgr,
  Fsim& fsim
) : mFaultStatusMgr{fsmgr},
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
  const TpgFault* f,
  const TestVector& tv
)
{
  mFsim.set_skip(f);
  SizeType n = mFsim.sppfp(tv); // n は未使用
  for ( auto f: mFsim.det_fault_list() ) {
    ASSERT_COND( mFaultStatusMgr.get(f) != FaultStatus::Untestable );

    mFaultStatusMgr.set(f, FaultStatus::Detected);
    mFsim.set_skip(f);
  }
}

END_NAMESPACE_DRUID
