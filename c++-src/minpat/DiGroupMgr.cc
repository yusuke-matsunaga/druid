
/// @file DiGroupMgr.cc
/// @brief DiGroupMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "DiGroupMgr.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス DiGroupMgr
//////////////////////////////////////////////////////////////////////

// @brief 初期グループを作るコンストラクタ
DiGroupMgr::DiGroupMgr(
  const TpgFaultList& fault_list
)
{
  auto group = new_group(fault_list);
  group->set_conflict_list({group});
  group->set_dominate_list({group});
  mGroupList.push_back(group);
  mUndetGroup = group;
}

// @brief 細分化したグループを作るコンストラクタ
DiGroupMgr::DiGroupMgr(
  const DiGroupMgr& prev_mgr,
  const std::unordered_set<SizeType>& fault_set
)
{
  // 二分したグループを作る．
  // 結果は new_group_list と subgroup_list に入れる．
  // i 番目のグループの検出された部分グループが subgroup_list[i * 2 + 0] に
  // 検出されなかった部分グループが subgroup_list[i * 2 + 1] に入る．
  // ただし，結果が空リストの場合は nullptr となる．
  auto& prev_group_list = prev_mgr.group_list();
  auto ng = prev_group_list.size();
  std::vector<DiGroup*> new_group_list;
  new_group_list.reserve(ng * 2);
  std::vector<DiGroup*> subgroup_list(ng * 2, nullptr);
  for ( SizeType i = 0; i < ng; ++ i ) {
    auto src_group = prev_group_list[i];
    auto& fault_list = src_group->fault_list();
    TpgFaultList det_list;
    TpgFaultList undet_list;
    for ( auto fault: fault_list ) {
      if ( fault_set.count(fault.id()) > 0 ) {
	det_list.push_back(fault);
      }
      else {
	undet_list.push_back(fault);
      }
    }
    DiGroup* det_group = nullptr;
    if ( !det_list.empty() ) {
      det_group = new_group(det_list);
      new_group_list.push_back(det_group);
    }
    DiGroup* undet_group = nullptr;
    if ( !undet_list.empty() ) {
      undet_group = new_group(undet_list);
      new_group_list.push_back(undet_group);
    }
    subgroup_list[i * 2 + 0] = det_group;
    subgroup_list[i * 2 + 1] = undet_group;
    if ( src_group == prev_mgr.undet_group() ) {
      mUndetGroup = undet_group;
    }
  }

  // conflict list と dominate list を作る．
  for ( SizeType i = 0; i < ng; ++ i ) {
    auto src_group = prev_group_list[i];
    auto det_group = subgroup_list[i * 2 + 0];
    auto undet_group = subgroup_list[i * 2 + 1];

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

    { // 検出された部分グループの dominate_list は親の dominate_list
      // の検出された部分グループとなる．
      // 検出されなかった部分グループの dominate_list は親の dominate_list
      // の両方の部分グループ＋検出された部分グループとなる．
      std::vector<DiGroup*> det_dominate_list;
      std::vector<DiGroup*> undet_dominate_list;
      auto& src_dominate_list = src_group->dominate_list();
      det_dominate_list.reserve(src_dominate_list.size());
      undet_dominate_list.reserve(src_dominate_list.size() + 1);
      for ( auto src_dominate_group: src_dominate_list ) {
	auto src_id = src_dominate_group->id();
	auto det_dominate_subgroup = subgroup_list[src_id * 2 + 0];
	auto undet_dominate_subgroup = subgroup_list[src_id * 2 + 1];
	if ( det_dominate_subgroup != nullptr ) {
	  det_dominate_list.push_back(det_dominate_subgroup);
	  undet_dominate_list.push_back(det_dominate_subgroup);
	}
	if ( undet_dominate_subgroup != nullptr ) {
	  undet_dominate_list.push_back(undet_dominate_subgroup);
	}
      }
      if ( det_group != nullptr ) {
	det_group->set_dominate_list(std::move(det_dominate_list));
      }
      if ( undet_group != nullptr ) {
	if ( det_group != nullptr ) {
	  undet_dominate_list.push_back(det_group);
	}
	undet_group->set_dominate_list(std::move(undet_dominate_list));
      }
    }
  }
  mGroupList.reserve(new_group_list.size());
  for ( auto group: new_group_list ) {
    mGroupList.push_back(group);
  }
}

// @brief 故障グループを作る．
DiGroup*
DiGroupMgr::new_group(
  const TpgFaultList& fault_list
)
{
  auto id = mGroupArray.size();
  auto group = new DiGroup(id, fault_list);
  mGroupArray.push_back(std::unique_ptr<DiGroup>{group});
  return group;
 }

END_NAMESPACE_DRUID
