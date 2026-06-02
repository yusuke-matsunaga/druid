
/// @file NaiveCandMgr.cc
/// @brief NaiveCandMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "NaiveCandMgr.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス NaiveCandMgr
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
NaiveCandMgr::NaiveCandMgr(
  const TpgFaultList& fault_list
) : CandMgr(fault_list),
    mSize{max_fault_size()},
    mArray(mSize * mSize, false),
    mDomCandListArray(mSize)
{
}

// @brief デストラクタ
NaiveCandMgr::~NaiveCandMgr()
{
}

// @brief 更新処理
bool
NaiveCandMgr::update(
  const std::vector<PackedVal>& dpat_array
)
{
  if ( mInitialized ) {
    bool change = false;
    for ( auto fault1: fault_list() ) {
      auto pat1 = dpat_array[fault1.id()];
      auto& old_list = mDomCandListArray[fault1.id()];
      TpgFaultList new_list;
      new_list.reserve(old_list.size());
      for ( auto fault2: old_list ) {
	auto pat2 = dpat_array[fault2.id()];
	if ( (pat1 & ~pat2) != PV_ALL0 ) {
	  auto idx1 = _index(fault1, fault2);
	  mArray[idx1] = true;
	}
	else {
	  new_list.push_back(fault2);
	}
      }
      if ( new_list.size() != old_list.size() ) {
	std::swap(new_list, old_list);
	change = true;
      }
    }
    return change;
  }
  else {
    for ( auto fault1: fault_list() ) {
      auto pat1 = dpat_array[fault1.id()];
      auto& new_list = mDomCandListArray[fault1.id()];
      for ( auto fault2: fault_list() ) {
	if ( fault2 == fault1 ) {
	  continue;
	}
	auto pat2 = dpat_array[fault2.id()];
	if ( (pat1 & ~pat2) != PV_ALL0 ) {
	  auto idx1 = _index(fault1, fault2);
	  mArray[idx1] = true;
	}
	else {
	  new_list.push_back(fault2);
	}
      }
    }
    mInitialized = true;
    return true;
  }
}

// @brief 終了処理
EqDomCand
NaiveCandMgr::end()
{
  EqDomCand cand;
  cand.init(fault_list());
  {
    std::vector<bool> mark(mSize, false);
    for ( auto fault: fault_list() ) {
      if ( mark[fault.id()] ) {
	continue;
      }
      auto eq_list = eqcand_list(fault);
      eq_list.push_back(fault);
      eq_list.sort();
      cand.add_eqgroup(eq_list);
      for ( auto fault1: eq_list ) {
	mark[fault1.id()] = true;
      }
    }
  }
  for ( auto fault: fault_list() ) {
    auto dom_list = mDomCandListArray[fault.id()];
    cand.set_domcand(fault, dom_list);
  }
  return cand;
}

// @brief 等価な可能性のある故障のリストを返す．
TpgFaultList
NaiveCandMgr::eqcand_list(
  const TpgFault& fault
) const
{
  TpgFaultList ans_list;
  for ( auto fault2: mDomCandListArray[fault.id()] ) {
    auto idx2 = _index(fault2, fault);
    if ( !mArray[idx2] ) {
      ans_list.push_back(fault2);
    }
  }
  return ans_list;
}

END_NAMESPACE_DRUID
