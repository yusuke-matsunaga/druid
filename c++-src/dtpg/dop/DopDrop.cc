
/// @file DopDrop.cc
/// @brief DopDrop の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "DopDrop.h"
#include "DtpgMgr.h"
#include "Fsim.h"
#include "TpgFault.h"
#include "TestVector.h"


BEGIN_NAMESPACE_DRUID

// @brief 'drop' タイプを生成する．
DetectOp*
new_DopDrop(
  DtpgMgr& mgr,
  Fsim& fsim
)
{
  return new DopDrop{mgr, fsim};
}


//////////////////////////////////////////////////////////////////////
// クラス DopDrop
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
DopDrop::DopDrop(
  DtpgMgr& mgr,
  Fsim& fsim
) : mMgr{mgr},
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
  mFsim.sppfp(tv,
	      [&](const TpgFault* f, DiffBits _)
	      {
		mMgr.update_det(f, tv, 0.0, 0.0);
		mFsim.set_skip(f);
	      });
}

END_NAMESPACE_DRUID
