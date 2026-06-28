
/// @file DomMgr.cc
/// @brief DomMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "DomMgr.h"
#include "EqGroupMgr.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス DomMgr
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
DomMgr::DomMgr(
  const EqGroupMgr* eqmgr,
  FaultInfo& fault_info,
  Fsim& fsim
) : mFaultInfo{fault_info},
    mFsim{fsim},
    mCandListArray(fault_info.network().max_fault_id())
{
  for ( auto fault: mFaultInfo.rep_fault_list() ) {
    auto& cand_list = mCandListArray[fault.id()];
    auto gid = eqmgr->group_id(fault);
    for ( auto gid1: eqmgr->prev_list(gid) ) {
      for ( auto fault1: eqmgr->fault_list(gid1) ) {
	if ( mFaultInfo.is_rep(fault1) ) {
	  cand_list.push_back(fault1);
	}
      }
    }
  }
}

// @brief 故障シミュレーションの結果で候補リストを更新する．
bool
DomMgr::update(
  const std::vector<TestVector>& tv_list
)
{
  mFsimTimer.start();
  auto res = mFsim.run_multi(tv_list, true);
  mFsimTimer.stop();

  auto ntv = res.tv_num();
  std::vector<PackedVal> dpat_array(mFaultInfo.network().max_fault_id(), PV_ALL0);
  for ( SizeType i = 0; i < ntv; ++ i ) {
    PackedVal bit = 1ULL << i;
    for ( auto fault: res.fault_list(i) ) {
      auto fid = fault.id();
      dpat_array[fid] |= bit;
    }
  }

  bool changed = false;
  for ( auto fault: mFaultInfo.rep_fault_list() ) {
    auto dpat = dpat_array[fault.id()];
    auto& cand_list = mCandListArray[fault.id()];
    TpgFaultList new_cand_list;
    new_cand_list.reserve(cand_list.size());
    for ( auto fault1: cand_list ) {
      auto dpat1 = dpat_array[fault1.id()];
      if ( (dpat1 & dpat) == dpat1 ) {
	new_cand_list.push_back(fault1);
      }
    }
    if ( new_cand_list.size() != cand_list.size() ) {
      std::swap(cand_list, new_cand_list);
      changed = true;
    }
  }
  return changed;
}

END_NAMESPACE_DRUID
