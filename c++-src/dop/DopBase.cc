
/// @file DopBase.cc
/// @brief DopBase の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "DopBase.h"
#include "FaultStatusMgr.h"


BEGIN_NAMESPACE_DRUID

// @brief 'base' タイプを生成する．
// @param[in] fsmgr 故障マネージャ
DetectOp*
new_DopBase(FaultStatusMgr& fsmgr)
{
  return new DopBase(fsmgr);
}


//////////////////////////////////////////////////////////////////////
// クラス DopBase
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] fsmgr 故障マネージャ
DopBase::DopBase(FaultStatusMgr& fsmgr) :
  mFaultStatusMgr(fsmgr)
{
}

// @brief デストラクタ
DopBase::~DopBase()
{
}

// @brief テストパタンが見つかった時の処理
// @param[in] f 故障
// @param[in] tv テストベクタ
void
DopBase::operator()(const TpgFault* f,
		    const TestVector& tv)
{
  mFaultStatusMgr.set(f, FaultStatus::Detected);
}

END_NAMESPACE_DRUID
