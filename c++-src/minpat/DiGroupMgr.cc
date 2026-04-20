
/// @file DiGroupMgr.cc
/// @brief DiGroupMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "DiGroupMgr.h"
#include "types/TpgNetwork.h"
#include "ym/MtMgr.h"
#include "ym/IdPool.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス DiGroupMgr
//////////////////////////////////////////////////////////////////////

// @brief 初期グループを作るコンストラクタ
DiGroupMgr::DiGroupMgr(
  const TpgFaultList& fault_list
)
{
  auto group = _new_group(fault_list);
  if ( group != nullptr ) {
    group->set_conflict_list({group});
    group->set_dominance_list({group});
  }
}

// @brief コピーコンストラクタ
DiGroupMgr::DiGroupMgr(
  const DiGroupMgr& src
)
{
  _copy(src);
}

// @brief コピー代入演算子
DiGroupMgr&
DiGroupMgr::operator=(
  const DiGroupMgr& src
)
{
  mGroupArray.clear();
  mGroupList.clear();
  _copy(src);
  return *this;
}

// @brief prev_mgr の故障グループを det_list に基づいて細分化する．
DiGroupMgr
DiGroupMgr::dichotomy(
  const DiGroupMgr& mgr,
  const TpgFaultList& det_list
)
{
  if ( det_list.size() == 0 ) {
    return mgr;
  }

  auto network = det_list.network();
  std::vector<bool> det_mark(network.max_fault_id(), false);
  for ( auto fault: det_list ) {
    det_mark[fault.id()] = true;
  }

  // 二分したグループを作る．
  // 結果は subgroup_list に入れる．
  // i 番目のグループの検出された部分グループが subgroup_list[i * 2 + 0] に
  // 検出されなかった部分グループが subgroup_list[i * 2 + 1] に入る．
  // ただし，結果が空リストの場合は nullptr となる．
  auto& prev_group_list = mgr.group_list();
  auto ng = prev_group_list.size();

  DiGroupMgr new_mgr;
  new_mgr.mGroupArray.reserve(ng * 2);
  new_mgr.mGroupList.reserve(ng * 2);
  std::vector<DiGroup*> det_group_list(ng, nullptr);
  std::vector<DiGroup*> undet_group_list(ng, nullptr);

  // 細分化した部分グループを作る．
  for ( SizeType id = 0; id < ng; ++ id ) {
    auto src_group = prev_group_list[id];
    auto& fault_list = src_group->fault_list();
    TpgFaultList det_list;
    TpgFaultList undet_list;
    for ( auto fault: fault_list ) {
      if ( det_mark[fault.id()] ) {
	det_list.push_back(fault);
      }
      else {
	undet_list.push_back(fault);
      }
    }
    auto det_group = new_mgr._new_group(det_list);
    auto undet_group = new_mgr._new_group(undet_list);
    det_group_list[id] = det_group;
    undet_group_list[id] = undet_group;
  }

  // conflict list と dominance list を作る．
  for ( SizeType id = 0; id < ng; ++ id ) {
    auto src_group = prev_group_list[id];
    auto det_group = det_group_list[id];
    auto undet_group = undet_group_list[id];

#if 0
    { // 検出された部分グループの conflict_list は親の conflict_list
      // の検出されなかった部分グループとなる．
      // 検出されなかった部分グループの conflict_list は親の conflict_list
      // の両方の部分グループとなる．
      std::vector<DiGroup*> det_conflict_list;
      std::vector<DiGroup*> undet_conflict_list;
      auto& src_conflict_list = src_group->conflict_list();
      det_conflict_list.reserve(src_conflict_list.size());
      undet_conflict_list.reserve(src_conflict_list.size());
      for ( auto src_conflict_group: src_conflict_list ) {
	auto src_id = src_conflict_group->id();
	auto det_conflict_subgroup = subgroup_list[src_id * 2 + 0];
	auto undet_conflict_subgroup = subgroup_list[src_id * 2 + 1];
	if ( undet_conflict_subgroup != nullptr ) {
	  det_conflict_list.push_back(undet_conflict_subgroup);
	  undet_conflict_list.push_back(undet_conflict_subgroup);
	}
	if ( det_conflict_subgroup != nullptr ) {
	  undet_conflict_list.push_back(det_conflict_subgroup);
	}
      }
      if ( det_group != nullptr ) {
	det_group->set_conflict_list(std::move(det_conflict_list));
      }
      if ( undet_group != nullptr ) {
	undet_group->set_conflict_list(std::move(undet_conflict_list));
      }
    }
#endif

    { // 検出された部分グループの dominance_list は親の dominance_list
      // の検出された部分グループとなる．
      // 検出されなかった部分グループの dominance_list は親の dominance_list
      // の両方の部分グループ＋検出された部分グループとなる．
      std::vector<DiGroup*> det_dominance_list;
      std::vector<DiGroup*> undet_dominance_list;
      auto& src_dominance_list = src_group->dominance_list();
      det_dominance_list.reserve(src_dominance_list.size());
      undet_dominance_list.reserve(src_dominance_list.size() + 1);
      for ( auto src_dominance_group: src_dominance_list ) {
	auto src_id = src_dominance_group->id();
	auto det_dominance_subgroup = det_group_list[src_id];
	auto undet_dominance_subgroup = undet_group_list[src_id];
	if ( det_dominance_subgroup != nullptr ) {
	  det_dominance_list.push_back(det_dominance_subgroup);
	  undet_dominance_list.push_back(det_dominance_subgroup);
	}
	if ( undet_dominance_subgroup != nullptr ) {
	  undet_dominance_list.push_back(undet_dominance_subgroup);
	}
      }
      if ( det_group != nullptr ) {
	det_group->set_dominance_list(std::move(det_dominance_list));
      }
      if ( undet_group != nullptr ) {
	if ( det_group != nullptr ) {
	  undet_dominance_list.push_back(det_group);
	}
	undet_group->set_dominance_list(std::move(undet_dominance_list));
      }
    }
  }

  return new_mgr;
}

// @brief 内容を出力する．
void
DiGroupMgr::print(
  std::ostream& s
) const
{
  SizeType i = 0;
  SizeType nf = 0;
  for ( auto group: group_list() ) {
    s << "Group#" << i << ":";
    for ( auto fault: group->fault_list() ) {
      s << " " << fault.str();
    }
    s << std::endl;
    nf += group->fault_list().size();
    ++ i;
  }
  s << "# of faults: " << nf << std::endl;
}

// @brief 等価比較演算子
bool
DiGroupMgr::operator==(
  const DiGroupMgr& right
) const
{
  if ( group_num() != right.group_num() ) {
    return false;
  }

  SizeType d_count1 = 0;
  for ( auto group: right.group_list() ) {
    d_count1 += group->dominance_list().size();
  }
  SizeType d_count2 = 0;
  for ( auto group: group_list() ) {
    d_count2 += group->dominance_list().size();
  }
  return d_count1 == d_count2;
}

// @brief 複製する．
void
DiGroupMgr::_copy(
  const DiGroupMgr& src
)
{
  auto ng = src.group_num();
  mGroupArray.reserve(ng);
  mGroupList.reserve(ng);
  for ( auto src_group: src.mGroupList ) {
    _new_group(src_group->fault_list());
  }

  for ( SizeType i = 0; i < ng; ++ i ) {
    auto src_group = src.mGroupList[i];
    auto dst_group = mGroupArray[i].get(); // mGroupList は const なので使えない．
#if 0
    // mConflictList をコピーする．
    dst_group->mConflictList.reserve(src_group->mConflictList.size());
    for ( auto src: src_group->mConflictList ) {
      auto dst = mGroupArray[src->id()].get();
      dst_group->mConflictList.push_back(dst);
    }
#endif
    // mDominanceList をコピーする．
    dst_group->mDominanceList.reserve(src_group->mDominanceList.size());
    for ( auto src: src_group->mDominanceList ) {
      auto dst = mGroupArray[src->id()].get();
      dst_group->mDominanceList.push_back(dst);
    }
  }
}

// @brief 故障グループを作る．
DiGroup*
DiGroupMgr::_new_group(
  const TpgFaultList& fault_list
)
{
  if ( fault_list.empty() ) {
    return nullptr;
  }
  auto id = mGroupArray.size();
  auto group = new DiGroup(id, fault_list);
  mGroupArray.push_back(std::unique_ptr<DiGroup>{group});
  mGroupList.push_back(group);
  return group;
}

#if 0
//////////////////////////////////////////////////////////////////////
// クラス DiGroupBuilder
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
DiGroupBuilder::DiGroupBuilder(
  const std::vector<const DiGroup*>& src_group_list,
  const std::vector<bool>& det_mark
) : mN{src_group_list.size()},
    mSrcGroupList{src_group_list},
    mDetMark{det_mark},
    mDetGroupList(mN, nullptr),
    mUndetGroupList(mN, nullptr)
{
  mSubGroupList.reserve(mN * 2);
}

// @brief 細分化された部分グループを作る．
void
DiGroupBuilder::make_subgroup(
  SizeType id
)
{
  auto src_group = mSrcGroupList[id];
  auto& fault_list = src_group->fault_list();
  TpgFaultList det_list;
  TpgFaultList undet_list;
  for ( auto fault: fault_list ) {
    if ( mDetMark[fault.id()] ) {
      det_list.push_back(fault);
    }
    else {
      undet_list.push_back(fault);
    }
  }
  auto det_group = new_group(det_list);
  auto undet_group = new_group(undet_list);
  mDetGroupList[id] = det_group;
  mUndetGroupList[id] = undet_group;
}

// @brief 細分化された部分グループの dominance list を作る．
void
DiGroupBuilder::make_dominance_list(
  SizeType id
)
{
  auto src_group = mSrcGroupList[id];
  auto det_group = mDetGroupList[id];
  auto undet_group = mUndetGroupList[id];

#if 0
  { // 検出された部分グループの conflict_list は親の conflict_list
    // の検出されなかった部分グループとなる．
    // 検出されなかった部分グループの conflict_list は親の conflict_list
    // の両方の部分グループとなる．
    std::vector<DiGroup*> det_conflict_list;
    std::vector<DiGroup*> undet_conflict_list;
    auto& src_conflict_list = src_group->conflict_list();
    det_conflict_list.reserve(src_conflict_list.size());
    undet_conflict_list.reserve(src_conflict_list.size());
    for ( auto src_conflict_group: src_conflict_list ) {
      auto src_id = src_conflict_group->id();
      auto det_conflict_subgroup = subgroup_list[src_id * 2 + 0];
      auto undet_conflict_subgroup = subgroup_list[src_id * 2 + 1];
      if ( undet_conflict_subgroup != nullptr ) {
	det_conflict_list.push_back(undet_conflict_subgroup);
	undet_conflict_list.push_back(undet_conflict_subgroup);
      }
      if ( det_conflict_subgroup != nullptr ) {
	undet_conflict_list.push_back(det_conflict_subgroup);
      }
    }
    if ( det_group != nullptr ) {
      det_group->set_conflict_list(std::move(det_conflict_list));
    }
    if ( undet_group != nullptr ) {
      undet_group->set_conflict_list(std::move(undet_conflict_list));
    }
  }
#endif

  { // 検出された部分グループの dominance_list は親の dominance_list
    // の検出された部分グループとなる．
    // 検出されなかった部分グループの dominance_list は親の dominance_list
    // の両方の部分グループ＋検出された部分グループとなる．
    std::vector<DiGroup*> det_dominance_list;
    std::vector<DiGroup*> undet_dominance_list;
    auto& src_dominance_list = src_group->dominance_list();
    det_dominance_list.reserve(src_dominance_list.size());
    undet_dominance_list.reserve(src_dominance_list.size() + 1);
    for ( auto src_dominance_group: src_dominance_list ) {
      auto src_id = src_dominance_group->id();
      auto det_dominance_subgroup = mDetGroupList[src_id];
      auto undet_dominance_subgroup = mUndetGroupList[src_id];
      if ( det_dominance_subgroup != nullptr ) {
	det_dominance_list.push_back(det_dominance_subgroup);
	undet_dominance_list.push_back(det_dominance_subgroup);
      }
      if ( undet_dominance_subgroup != nullptr ) {
	undet_dominance_list.push_back(undet_dominance_subgroup);
      }
    }
    if ( det_group != nullptr ) {
      det_group->set_dominance_list(std::move(det_dominance_list));
    }
    if ( undet_group != nullptr ) {
      if ( det_group != nullptr ) {
	undet_dominance_list.push_back(det_group);
      }
      undet_group->set_dominance_list(std::move(undet_dominance_list));
    }
  }
}

// @brief 結果の DiGroupMgr を生成する．
DiGroupMgr
DiGroupBuilder::make_mgr()
{
  return DiGroupMgr(mSubGroupList);
}

// @brief 新しいグループを作る．
DiGroup*
DiGroupBuilder::new_group(
  const TpgFaultList& fault_list
)
{
  if ( fault_list.empty() ) {
    return nullptr;
  }
  DiGroup* group = nullptr;
  mLock.run([&]() {
    auto id = mSubGroupList.size();
    group = new DiGroup(id, fault_list);
    mSubGroupList.push_back(group);
  });
  return group;
}
#endif

END_NAMESPACE_DRUID
