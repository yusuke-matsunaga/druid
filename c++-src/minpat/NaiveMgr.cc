
/// @file NaiveMgr.cc
/// @brief NaiveMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "NaiveMgr.h"
#include "types/PackedVal.h"


BEGIN_NAMESPACE_DRUID

// @brief 故障シミュレーションの結果を登録する．
bool
NaiveMgr::add(
  const FsimResults& res
)
{
  auto ntv = res.tv_num();
  // 各故障に対するビットパタンを作る．
  std::vector<PackedVal> pat_array(mSize, 0);
  for ( SizeType i = 0; i < ntv; ++ i ) {
    PackedVal pat = 1ULL << i;
    for ( auto fault: res.fault_list(i) ) {
      pat_array[fault.id()] |= pat;
    }
  }

  auto nf = mFaultList.size();
  if ( mInitialized ) {
    bool change = false;
    for ( auto fault1: mFaultList ) {
      auto pat1 = pat_array[fault1.id()];
      auto& old_list = mDomCandListArray[fault1.id()];
      TpgFaultList new_list;
      new_list.reserve(old_list.size());
      for ( auto fault2: old_list ) {
	auto pat2 = pat_array[fault2.id()];
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
    for ( auto fault1: mFaultList ) {
      auto pat1 = pat_array[fault1.id()];
      auto& new_list = mDomCandListArray[fault1.id()];
      for ( auto fault2: mFaultList ) {
	if ( fault2 == fault1 ) {
	  continue;
	}
	auto pat2 = pat_array[fault2.id()];
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

// @brief 等価故障の可能性のあるリストを返す．
TpgFaultList
NaiveMgr::eqcand_list(
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

// @brief 支配する可能性のある故障のリストを返す．
TpgFaultList
NaiveMgr::domcand_list(
  const TpgFault& fault
) const
{
  return mDomCandListArray[fault.id()];
}

END_NAMESPACE_DRUID
