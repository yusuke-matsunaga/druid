
/// @file FaultStatusMgr.cc
/// @brief FaultStatusMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "FaultStatusMgr.h"
#include "TpgNetwork.h"
#include "TpgFault.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス FaultStatusMgr
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
FaultStatusMgr::FaultStatusMgr(
  const TpgNetwork& network
) : mStatusArray(network.max_fault_id())
{
}

// @brief デストラクタ
FaultStatusMgr::~FaultStatusMgr()
{
}

// @brief 故障の状態をセットする．
void
FaultStatusMgr::set(
  const TpgFault* fault,
  FaultStatus status
)
{
  mStatusArray[fault->id()] = status;
}

// @brief 故障の状態を得る．
FaultStatus
FaultStatusMgr::get(
  const TpgFault* fault
) const
{
  return mStatusArray[fault->id()];
}


END_NAMESPACE_DRUID
