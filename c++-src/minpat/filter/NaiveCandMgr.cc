
/// @file NaiveCandMgr.cc
/// @brief NaiveCandMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "CandMgr.h"
#include "NaiveCandMgr.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス CandMgr
//////////////////////////////////////////////////////////////////////

// @brief 新しいオブジェクトを作る．
std::unique_ptr<CandMgr>
CandMgr::new_naive_mgr(
  const TpgFaultList& fault_list,
  const ConfigParam& option
)
{
  return std::unique_ptr<CandMgr>{new NaiveCandMgr(fault_list)};
}


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
std::unique_ptr<EqDomCand>
NaiveCandMgr::end(
  bool reduce
) const
{
  std::vector<TpgFaultList> group_list;
  std::unordered_map<SizeType, SizeType> id_map;
  { // 等価故障グループを求める．
    std::vector<bool> mark(mSize, false);
    for ( auto fault: fault_list() ) {
      if ( mark[fault.id()] ) {
	continue;
      }
      auto gid = group_list.size();
      auto eq_list = eqcand_list(fault);
      eq_list.push_back(fault);
      for ( auto fault1: eq_list ) {
	mark[fault1.id()] = true;
	id_map.emplace(fault1.id(), gid);
      }
      group_list.push_back(eq_list);
    }
  }
  // 支配関係のリストを作る．
  std::vector<std::pair<SizeType, SizeType>> dom_list;
  auto ng = group_list.size();
  for ( SizeType id1 = 0; id1 < ng; ++ id1 ) {
    std::unordered_set<SizeType> id_set;
    id_set.insert(id1);
    for ( auto fault1: group_list[id1] ) {
      for ( auto fault2: mDomCandListArray[fault1.id()] ) {
	auto id2 = id_map.at(fault2.id());
	if ( id_set.count(id2) == 0 ) {
	  id_set.insert(id2);
	  dom_list.push_back({id1, id2});
	}
      }
    }
  }
  return std::unique_ptr<EqDomCand>{new EqDomCand(group_list, dom_list, reduce)};
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
