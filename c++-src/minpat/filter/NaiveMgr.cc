
/// @file NaiveMgr.cc
/// @brief NaiveMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "EqGroupMgr.h"
#include "NaiveMgr.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス EqGroupMgr
//////////////////////////////////////////////////////////////////////

// @brief 新しいオブジェクトを作る．
std::unique_ptr<EqGroupMgr>
EqGroupMgr::new_naive_mgr(
  FaultInfo& fault_info,
  Fsim& fsim,
  const ConfigParam& option
)
{
  return std::unique_ptr<EqGroupMgr>{new NaiveMgr(fault_info, fsim, option)};
}


//////////////////////////////////////////////////////////////////////
// クラス NaiveMgr
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
NaiveMgr::NaiveMgr(
  FaultInfo& fault_info,
  Fsim& fsim,
  const ConfigParam& option
) : EqGroupMgr(fault_info, fsim, option),
    mSize{max_fault_size()},
    mArray(mSize * mSize, false),
    mDomCandListArray(mSize)
{
}

// @brief デストラクタ
NaiveMgr::~NaiveMgr()
{
}

// @brief 更新処理
bool
NaiveMgr::update(
  const std::vector<PackedVal>& dpat_array
)
{
  auto fault_list = fault_info().rep_fault_list();
  if ( mInitialized ) {
    bool change = false;
    for ( auto fault1: fault_list ) {
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
	mHasGroup = false;
	change = true;
      }
    }
    return change;
  }
  else {
    for ( auto fault1: fault_list ) {
      auto pat1 = dpat_array[fault1.id()];
      auto& new_list = mDomCandListArray[fault1.id()];
      for ( auto fault2: fault_list ) {
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
    mHasGroup = false;
    return true;
  }
}

// @brief 終了処理
std::unique_ptr<EqDomCand>
NaiveMgr::end(
  bool reduce
) const
{
  _make_group();
  std::vector<TpgFaultList> group_list;
  group_list.reserve(mGroupArray.size());
  std::vector<std::pair<SizeType, SizeType>> dom_list;
  for ( auto& group1: mGroupArray ) {
    group_list.push_back(group1.mFaultList);
    auto id1 = group1.mId;
    for ( auto id2: group1.mSuccList ) {
      dom_list.push_back({id1, id2});
    }
  }
  return std::unique_ptr<EqDomCand>{new EqDomCand(group_list, dom_list, reduce)};
}

// @brief 等価グループを求める．
void
NaiveMgr::_make_group() const
{
  if ( mHasGroup ) {
    return;
  }

  mGroupArray.clear();
  mIdMap.clear();
  mIdMap.resize(mSize);
  // 等価グループを作る．
  std::vector<bool> mark(mSize, false);
  for ( auto fault: fault_info().rep_fault_list() ) {
    if ( mark[fault.id()] ) {
      continue;
    }
    if ( !is_rep(fault) ) {
      continue;
    }
    auto gid = mGroupArray.size();
    TpgFaultList eq_list;
    for ( auto fault2: mDomCandListArray[fault.id()] ) {
      auto idx2 = _index(fault2, fault);
      if ( !mArray[idx2] ) {
	eq_list.push_back(fault2);
      }
    }
    eq_list.push_back(fault);
    for ( auto fault1: eq_list ) {
      mark[fault1.id()] = true;
      mIdMap[fault1.id()] = gid;
    }
    mGroupArray.push_back(Group{gid, eq_list});
  }
  // 支配関係のリストを作る．
  auto ng = mGroupArray.size();
  for ( SizeType id1 = 0; id1 < ng; ++ id1 ) {
    std::unordered_set<SizeType> id_set;
    id_set.insert(id1);
    auto& group1 = mGroupArray[id1];
    auto fault1 = group1.mFaultList[0];
    for ( auto fault2: mDomCandListArray[fault1.id()] ) {
      auto id2 = mIdMap[fault2.id()];
      if ( id2 == id1 ) {
	// self-loop は記録しない．
	continue;
      }
      if ( id_set.count(id2) == 0 ) {
	id_set.insert(id2);
	auto& group2 = mGroupArray[id2];
	group1.mSuccList.push_back(id2);
	group2.mPrevList.push_back(id1);
      }
    }
  }
  mHasGroup = true;
}

// @brief 等価故障グループ数を返す．
SizeType
NaiveMgr::group_num() const
{
  _make_group();
  return mGroupArray.size();
}

// @brief 等価故障グループ番号を返す．
SizeType
NaiveMgr::group_id(
  const TpgFault& fault
) const
{
  _make_group();
  return mIdMap[fault.id()];
}

// @brief 等価故障グループの故障リストを返す．
TpgFaultList
NaiveMgr::fault_list(
  SizeType group_id
) const
{
  _make_group();
  return mGroupArray[group_id].mFaultList;
}

// @brief 後続グループ番号のリスト返す．
std::vector<SizeType>
NaiveMgr::succ_list(
  SizeType group_id
) const
{
  _make_group();
  return mGroupArray[group_id].mSuccList;
}

// @brief 先行グループ番号のリスト返す．
std::vector<SizeType>
NaiveMgr::prev_list(
  SizeType group_id
) const
{
  _make_group();
  return mGroupArray[group_id].mPrevList;
}

// @brief 順序関係の要素数を返す．
SizeType
NaiveMgr::domcand_num() const
{
  _make_group();
  SizeType num = 0;
  for ( auto& group: mGroupArray ) {
    num += group.mSuccList.size();
  }
  return num;
}

END_NAMESPACE_DRUID
