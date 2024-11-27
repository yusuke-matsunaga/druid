
/// @file FFRFaultList.cc
/// @brief FFRFaultList の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "FFRFaultList.h"
#include "TpgNetwork.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
FFRFaultList::FFRFaultList(
  const TpgNetwork& network,
  const vector<const TpgFault*>& fault_list
) : mFaultList{fault_list}
{
  for ( auto fault: fault_list ) {
    auto ffr = network.ffr(fault);
    auto ffr_id = ffr->id();
    if ( mFaultListMap.count(ffr_id) == 0 ) {
      mFaultListMap.emplace(ffr_id, vector<const TpgFault*>{fault});
      mFFRList.push_back(ffr);
    }
    else {
      mFaultListMap.at(ffr_id).push_back(fault);
    }
  }
}

// @brief FFR 内の故障リスト
const vector<const TpgFault*>&
FFRFaultList::fault_list(
  const TpgFFR* ffr
) const
{
  return mFaultListMap.at(ffr->id());
}

END_NAMESPACE_DRUID
