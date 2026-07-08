
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
  // 各故障の支配故障の候補を作る．
  auto fault_list = fault_info().rep_fault_list();
  for ( auto fault: fault_list ) {
    auto& cand_list = mCandListArray[fault.id()];
    auto gid = eqmgr.group_id(fault);
    for ( auto gid1: eqmgr.pred_list(gid) ) {
      for ( auto fault1: eqmgr.fault_list(gid1) ) {
	if ( fault_info().is_rep(fault1) ) {
	  cand_list.push_back(fault1);
	}
      }
    }
    // 単純に故障番号でソートしておく．
    cand_list.sort();
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
	if ( dpat1.check_contained(dpat) ) {
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
