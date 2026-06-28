
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
) : RedMgr(fault_info, fsim),
    mCandListArray(fault_info.network().max_fault_id())
{
  for ( auto fault: fault_info.rep_fault_list() ) {
    auto& cand_list = mCandListArray[fault.id()];
    auto gid = eqmgr->group_id(fault);
    for ( auto gid1: eqmgr->prev_list(gid) ) {
      for ( auto fault1: eqmgr->fault_list(gid1) ) {
	if ( is_rep(fault1) ) {
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
  std::vector<PackedVal> dpat_array;
  simulate(tv_list, dpat_array);

  bool changed = false;
  for ( auto fault: fault_info().rep_fault_list() ) {
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
