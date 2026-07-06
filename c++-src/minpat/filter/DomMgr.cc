
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
  const EqGroupMgr& eqmgr
) : RedMgr(eqmgr.fault_info(), eqmgr.fsim()),
    mCandListArray(max_fault_size())
{
  // 各故障を支配故障の候補を作る．
  auto fault_list = fault_info().rep_fault_list();
  std::vector<SizeType> count_array(mCandListArray.size(), 0);
  for ( auto fault: fault_list ) {
    auto& cand_list = mCandListArray[fault.id()];
    auto gid = eqmgr.group_id(fault);
    for ( auto gid1: eqmgr.prev_list(gid) ) {
      for ( auto fault1: eqmgr.fault_list(gid1) ) {
	if ( fault_info().is_rep(fault1) ) {
	  cand_list.push_back(fault1);
	}
      }
    }
    count_array[fault.id()] = cand_list.size();
  }
  // ランクを求める．
  std::vector<SizeType> rank_array(mCandListArray.size(), 0);
  {
    std::vector<SizeType> id_list;
    for ( auto fault: fault_list ) {
      if ( count_array[fault.id()] == 0 ) {
	id_list.push_back(fault.id());
      }
    }
    for ( SizeType rank = 0; !id_list.empty(); ++ rank ) {
      for ( auto id: id_list ) {
	rank_array[id] = rank;
      }
      std::vector<SizeType> new_list;
      for ( auto id: id_list ) {
	auto& cand_list = mCandListArray[id];
	for ( auto fault1: cand_list ) {
	  auto id1 = fault1.id();
	  -- count_array[id1];
	  if ( count_array[id1] == 0 ) {
	    new_list.push_back(id1);
	  }
	}
      }
      std::swap(id_list, new_list);
    }
  }

  for ( auto fault: fault_list ) {
    auto& cand_list = mCandListArray[fault.id()];
    cand_list.sort([&](SizeType id1, SizeType id2) -> bool {
      return rank_array[id1] < rank_array[id2];
    });
  }
}

// @brief 故障シミュレーションの結果で候補リストを更新する．
bool
DomMgr::update(
  const std::vector<TestVector>& tv_list
)
{
  std::vector<DPat> dpat_array;
  simulate(tv_list, dpat_array);

  bool changed = false;
  for ( auto fault: fault_info().rep_fault_list() ) {
    auto& dpat = dpat_array[fault.id()];
    auto& cand_list = mCandListArray[fault.id()];
    TpgFaultList new_cand_list;
    new_cand_list.reserve(cand_list.size());
    for ( auto fault1: cand_list ) {
      if ( fault_info().is_rep(fault1) ) {
	auto& dpat1 = dpat_array[fault1.id()];
	if ( dpat1.check_contain(dpat) ) {
	  new_cand_list.push_back(fault1);
	}
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
