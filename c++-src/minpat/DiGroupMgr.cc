
/// @file DiGroupMgr.cc
/// @brief DiGroupMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "DiGroupMgr.h"
#include "types/TpgNetwork.h"
#include "types/PackedVal.h"
#include "ym/MtMgr.h"
#include "ym/IdPool.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// 分割されたグループの情報
struct SubGroup {
  PackedVal d_pat; // 検出パタン
  DiGroup* group;  // 分割されたグループ
};

struct SubGroupInfo {
  std::vector<SubGroup> sglist;
};

END_NONAMESPACE

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

// @brief prev_mgr の故障グループを det_list1, det_list2 に基づいて細分化する．
DiGroupMgr
DiGroupMgr::dichotomy(
  const DiGroupMgr& mgr,
  const FsimResults& res,
  const ConfigParam& option
)
{
  auto network = res.network();
  auto ntv = res.tv_num();
  std::vector<PackedVal> det_mark(network.max_fault_id(), 0);
  for ( SizeType i = 0; i < ntv; ++ i ) {
    PackedVal bit = 1 << i;
    for ( auto fault: res.fault_list(i) ) {
      det_mark[fault.id()] |= bit;
    }
  }

  // 細分化したグループを作る．
  auto& src_group_list = mgr.group_list();

  // まずグループの数を数える．
  auto ng = mgr.group_num();
  std::vector<std::vector<PackedVal>> d_list_array(ng);
  SizeType new_group_num = 0;
  for ( auto src_group: src_group_list ) {
    std::unordered_set<PackedVal> d_hash;
    auto& d_list = d_list_array[src_group->id()];
    for ( auto fault: src_group->fault_list() ) {
      auto d = det_mark[fault.id()];
      if ( d_hash.count(d) == 0 ) {
	d_hash.insert(d);
	d_list.push_back(d);
	++ new_group_num;
      }
    }
    std::sort(d_list.begin(), d_list.end(), std::greater<>());
  }

  DiGroupMgr new_mgr;
  new_mgr.mGroupArray.reserve(new_group_num);
  new_mgr.mGroupList.reserve(new_group_num);
  std::vector<SubGroupInfo> sginfo_array(ng);
  for ( SizeType id = 0; id < ng; ++ id ) {
    auto src_group = src_group_list[id];
    auto& fault_list = src_group->fault_list();
    std::unordered_map<SizeType, TpgFaultList> fault_list_dict;
    auto& d_list = d_list_array[id];
    for ( auto d: d_list ) {
      fault_list_dict.emplace(d, TpgFaultList());
    }
    for ( auto fault: fault_list ) {
      auto d = det_mark[fault.id()];
      fault_list_dict.at(d).push_back(fault);
    }
    auto& sginfo = sginfo_array[id];
    for ( auto d: d_list ) {
      auto& fault_list = fault_list_dict.at(d);
      auto new_group = new_mgr._new_group(fault_list);
      sginfo.sglist.push_back({d, new_group});
    }
  }

  // dominance list を作る．
  for ( SizeType id = 0; id < ng; ++ id ) {
    auto src_group = src_group_list[id];
    auto& sginfo = sginfo_array[id];
    std::vector<DiGroup*> dom_list;
    auto& src_dom_list = src_group->dominance_list();
    for ( auto& sg: sginfo.sglist ) {
      auto d = sg.d_pat;
      auto group = sg.group;
      for ( auto src_dom_group: src_dom_list ) {
	auto& src_dom_sginfo = sginfo_array[src_dom_group->id()];
	for ( auto& sg1: src_dom_sginfo.sglist ) {
	  auto d1 = sg1.d_pat;
	  if ( (d & d1) == d ) {
	    auto src_dom_subgroup = sg1.group;
	    dom_list.push_back(src_dom_subgroup);
	  }
	}
      }
      group->set_dominance_list(std::move(dom_list));
    }
  }

  return new_mgr;
}

// @brief 各グループの dominance_list() の要素数の総和
SizeType
DiGroupMgr::dominance_num() const
{
  SizeType d_num = 0;
  for ( auto group: group_list() ) {
    d_num += group->dominance_list().size();
  }
  return d_num;
}

// @brief 内容を出力する．
void
DiGroupMgr::print(
  std::ostream& s
) const
{
  s << "================================" << std::endl
    << " # of Groups: " << group_num() << std::endl;
  SizeType i = 0;
  SizeType nf = 0;
  for ( auto group: group_list() ) {
    s << "Group#" << i << ":" << std::endl;
    for ( auto fault: group->fault_list() ) {
      s << "    " << fault.str() << std::endl;
    }
    s << std::endl << "  dominance_list:";
    for ( auto g: group->dominance_list() ) {
      s << " " << g->id();
    }
    s << std::endl << std::endl;
    nf += group->fault_list().size();
    ++ i;
  }
  s << "# of faults: " << nf << std::endl
    << "================================" << std::endl;
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
  if ( dominance_num() != right.dominance_num() ) {
    return false;
  }
  return true;
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

END_NAMESPACE_DRUID
