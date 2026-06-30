
/// @file DomMgr.cc
/// @brief DomMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "DomMgr.h"
#include "EqGroupMgr.h"
#include "POSet.h"


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
  POSet::Builder builder;
  auto fault_list = fault_info.rep_fault_list();
  for ( auto fault: fault_list ) {
    auto gid = eqmgr->group_id(fault);
    for ( auto gid1: eqmgr->prev_list(gid) ) {
      for ( auto fault1: eqmgr->fault_list(gid1) ) {
	if ( fault_info.is_rep(fault1) ) {
	  builder.add(fault1.id(), fault.id());
	}
      }
    }
  }
  POSet poset(builder);
  auto network = fault_info.network();
  for ( auto fault: fault_list ) {
    auto pred_list = poset.pred_list(fault.id());
    std::sort(pred_list.begin(), pred_list.end(),
	      [&](SizeType id1, SizeType id2) -> bool {
		return poset.rank(id1) < poset.rank(id2);
	      });
    auto& cand_list = mCandListArray[fault.id()];
    for ( auto id: pred_list ) {
      auto fault1 = network.fault(id);
      cand_list.push_back(fault1);
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

// @brief 内容を出力する．
void
DomMgr::print(
  std::ostream& s
) const
{
  for ( auto fault: fault_info().rep_fault_list() ) {
    s << fault.str() << ":";
    for ( auto dom_fault: mCandListArray[fault.id()] ) {
      s << " " << dom_fault.str();
    }
    s << std::endl;
  }
}

END_NAMESPACE_DRUID
