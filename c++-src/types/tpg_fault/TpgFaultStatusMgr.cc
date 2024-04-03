
/// @file TpgFaultStatusMgr.cc
/// @brief TpgFaultStatusMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "TpgFaultStatusMgr.h"
#include "TpgFault.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

SizeType
max_id(
  const vector<const TpgFault*>& fault_list
)
{
  SizeType max_val = 0;
  for ( auto fault: fault_list ) {
    auto id = fault->id();
    max_val = std::max(max_val, id);
  }
  return max_val;
}

END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// クラス TpgFaultStatusMgr
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
TpgFaultStatusMgr::TpgFaultStatusMgr(
  const vector<const TpgFault*>& fault_list
) : mStatusArray(max_id(fault_list), FaultStatus::Undetected)
{
  mRemainCount = fault_list.size();
  mDetCount = 0;
  mUntestCount = 0;
}

// @brief 故障の状態を得る．
FaultStatus
TpgFaultStatusMgr::get_status(
  const TpgFault* fault
) const
{
  auto id = fault->id();
  ASSERT_COND( 0 <= id && id < mStatusArray.size() );

  return mStatusArray[id];
}

// @brief 故障を状態を設定する．
void
TpgFaultStatusMgr::set_status(
  const TpgFault* fault,
  FaultStatus status
)
{
  auto id = fault->id();
  ASSERT_COND( 0 <= id && id < mStatusArray.size() );

  if ( mStatusArray[id] != status ) {
    switch ( mStatusArray[id] ) {
    case FaultStatus::Undetected: -- mRemainCount; break;
    case FaultStatus::Detected:   -- mDetCount; break;
    case FaultStatus::Untestable: -- mUntestCount; break;
    }
    mStatusArray[id] = status;
    switch ( status ) {
    case FaultStatus::Undetected: ++ mRemainCount; break;
    case FaultStatus::Detected:   ++ mDetCount; break;
    case FaultStatus::Untestable: ++ mUntestCount; break;
    }
  }
}

END_NAMESPACE_DRUID
