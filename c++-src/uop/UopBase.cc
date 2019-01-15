
/// @file UopBase.cc
/// @brief UopBase の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "UopBase.h"
#include "FaultStatusMgr.h"
#include "TpgFault.h"


BEGIN_NAMESPACE_DRUID

// @brief 'base' タイプを生成する．
// @param[in] fsmgr 故障マネージャ
UntestOp*
new_UopBase(FaultStatusMgr& fsmgr)
{
  return new UopBase(fsmgr);
}


//////////////////////////////////////////////////////////////////////
// クラス UopBase
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] fsmgr TpgFaultMgr
UopBase::UopBase(FaultStatusMgr& fsmgr) :
  mMgr(fsmgr)
{
}

// @brief デストラクタ
UopBase::~UopBase()
{
}

// @brief テスト不能故障と判定された時の処理
// @param[in] f 故障
void
UopBase::operator()(const TpgFault* f)
{
  mMgr.set(f, FaultStatus::Untestable);
}


END_NAMESPACE_DRUID
