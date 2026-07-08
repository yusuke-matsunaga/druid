
/// @file DCM2Helper.cc
/// @brief DCM2Helper の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "DCM2Helper.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// @brief パタンを文字列にする．
std::string
pat_str(
  PackedVal pat
)
{
  std::ostringstream buf;
  buf << "[" << std::hex << pat << std::dec << "]";
  return buf.str();
}

END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// クラス DCM2Helper
//////////////////////////////////////////////////////////////////////

// @brief 細分化を行う．
bool
DCM2Helper::run(
  std::vector<DichoGroup::Ptr>& group_list,
  const std::vector<PackedVal>& dpat_array
)
{
  // もとのグループ数
  auto ng = group_list.size();

  mInfoArray.clear();
  mInfoArray.resize(ng);

  // 細分化したグループのリスト
  std::vector<DichoGroup::Ptr> new_group_list;

  // 細分化したサブグループを作る．
  for ( auto& group: group_list ) {
    // group の故障をパタンごとに分ける．
    auto& info = mInfoArray[group->id()];
    auto& subgroup_dict = info.mSubGroupDict;
    for ( auto fault: group->fault_list() ) {
      auto dpat = dpat_array[fault.id()];
      if ( subgroup_dict.count(dpat) == 0 ) {
	// 新しいグループを作る．
	auto id = new_group_list.size();
	auto new_group = new DichoGroup(id);
	new_group->add_fault(fault);
	new_group_list.push_back(DichoGroup::Ptr{new_group});
	subgroup_dict.emplace(dpat, new_group);
      }
      else {
	auto group = subgroup_dict.at(dpat);
	group->add_fault(fault);
      }
    }
  }

  // グループごとの DPatGraph を作る．
  for ( SizeType i = 0; i < ng; ++ i ) {
    auto& info = mInfoArray[i];
    auto& subgroup_dict = info.mSubGroupDict;
    std::vector<PackedVal> pat_list;
    pat_list.reserve(subgroup_dict.size());
    for ( auto& p: subgroup_dict ) {
      auto pat = p.first;
      pat_list.push_back(pat);
    }
    info.mPatGraph.rebuild(pat_list);
  }

  // 変化があったことを示すフラグ
  auto nng = new_group_list.size();
  bool changed = nng != ng;

  // グループ間の順序関係を求める．
  for ( auto& group: group_list ) {
    auto& succ_list = group->succ_list();
    auto& info = mInfoArray[group->id()];
    auto& subgroup_dict = info.mSubGroupDict;
    DichoGroup* subgroup0 = nullptr;
    for ( auto& p: subgroup_dict ) {
      auto dpat = p.first;
      auto subgroup = p.second;
      subgroup0 = subgroup;
      std::vector<DichoGroup*> sub_succ_list;
      // 同じ group から細分化したサブグループに対する順序関係
      auto& dpat_graph = info.mPatGraph;
      auto pat_list1 = dpat_graph.imm_succ_list(dpat);
      for ( auto pat1: pat_list1 ) {
	auto subgroup1 = subgroup_dict.at(pat1);
	sub_succ_list.push_back(subgroup1);
      }
      // group の後続グループから細分化したサブグループに対する順序関係
      std::vector<bool> mark(nng, false);
      add_succ_group(dpat, pat_list1, succ_list, mark, sub_succ_list);
      subgroup->set_succ_list(std::move(sub_succ_list));
    }
    if ( !changed && subgroup_dict.size() == 1 ) {
      // 後続リストの要素数が変化しているか調べる．
      auto& list1 = group->succ_list();
      auto& list2 = subgroup0->succ_list();
      auto n = list1.size();
      if ( n != list2.size() ) {
	changed = true;
      }
      else {
	for ( SizeType i = 0; i < n; ++ i ) {
	  auto g1 = list1[i];
	  auto g2 = list2[i];
	  if ( g1->id() != g2->id() ) {
	    changed = true;
	    break;
	  }
	}
      }
    }
  }

  // 変化があったら更新する．
  if ( changed ) {
    std::swap(group_list, new_group_list );
    return true;
  }
  return false;
}

// @brief 後続グループに対する順序関係を求める．
void
DCM2Helper::add_succ_group(
  PackedVal dpat,
  const std::vector<PackedVal>& block_pats,
  const std::vector<DichoGroup*>& succ_list,
  std::vector<bool>& mark,
  std::vector<DichoGroup*>& sub_succ_list
)
{
  for ( auto group: succ_list ) {
    if ( mark[group->id()] ) {
      continue;
    }
    mark[group->id()] = true;
    auto& info = mInfoArray[group->id()];
    auto& dpat_graph = info.mPatGraph;
    auto& subgroup_dict = info.mSubGroupDict;
    if ( subgroup_dict.count(dpat) > 0 ) {
      auto subgroup1 = subgroup_dict.at(dpat);
      sub_succ_list.push_back(subgroup1);
    }
    else {
      auto pat_list1 = dpat_graph.imm_succ_list(dpat, block_pats);
      for ( auto pat1: pat_list1 ) {
	auto subgroup1 = subgroup_dict.at(pat1);
	sub_succ_list.push_back(subgroup1);
      }
      auto block_pats1 = block_pats;
      block_pats1.insert(block_pats1.end(), pat_list1.begin(), pat_list1.end());
      add_succ_group(dpat, block_pats1, group->succ_list(),
		     mark, sub_succ_list);
    }
  }
}

END_NAMESPACE_DRUID
