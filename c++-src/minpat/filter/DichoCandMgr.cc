
/// @file DichoCandMgr.cc
/// @brief DichoCandMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "CandMgr.h"
#include "DichoCandMgr.h"
#include "DiGroup.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス DichoCandMgr
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
DichoCandMgr::DichoCandMgr(
  const TpgFaultList& fault_list
) : CandMgr(fault_list)
{
  // 最初は１つのグループ
  auto group = new DiGroup(0, fault_list);
  group->set_dominance_list({group});
  mCurGroupList.push_back(std::unique_ptr<DiGroup>{group});
}

// @brief デストラクタ
DichoCandMgr::~DichoCandMgr()
{
}

// @brief 更新処理
bool
DichoCandMgr::update(
  const std::vector<PackedVal>& dpat_array
)
{
  // まずグループの数を数える．
  // 同時にグループごとに現れた dpat のリストを作る．
  auto ng = mCurGroupList.size();
  std::vector<std::vector<PackedVal>> dpat_list_array(ng);
  SizeType new_group_num = 0;
  for ( auto& src_group: mCurGroupList ) {
    std::unordered_set<PackedVal> d_hash;
    auto& dpat_list = dpat_list_array[src_group->id()];
    for ( auto fault: src_group->fault_list() ) {
      auto dpat = dpat_array[fault.id()];
      if ( d_hash.count(dpat) == 0 ) {
	d_hash.insert(dpat);
	dpat_list.push_back(dpat);
	++ new_group_num;
      }
    }
    std::sort(dpat_list.begin(), dpat_list.end(), std::greater<>());
  }

  // 細分化したグループを作る．
  std::vector<std::unique_ptr<DiGroup>> new_group_list;
  new_group_list.reserve(new_group_num);
  for ( SizeType id = 0; id < ng; ++ id ) {
    auto& src_group = mCurGroupList[id];
    auto& fault_list = src_group->fault_list();
    // dpat をキーにして対象の故障を記録する辞書
    std::unordered_map<PackedVal, TpgFaultList> fault_list_dict;
    auto& dpat_list = dpat_list_array[id];
    for ( auto dpat: dpat_list ) {
      fault_list_dict.emplace(dpat, TpgFaultList());
    }
    for ( auto fault: fault_list ) {
      auto dpat = dpat_array[fault.id()];
      fault_list_dict.at(dpat).push_back(fault);
    }
    for ( auto dpat: dpat_list ) {
      auto& fault_list = fault_list_dict.at(dpat);
      if ( fault_list.empty() ) {
	continue;
      }
      auto id = new_group_list.size();
      auto new_group = new DiGroup(id, fault_list);
      new_group_list.push_back(std::unique_ptr<DiGroup>{new_group});
      src_group->add_subgroup(dpat, new_group);
    }
  }

  // dominance list を作る．
  for ( auto& src_group: mCurGroupList ) {
    std::vector<DiGroup*> dom_list;
    auto& src_dom_list = src_group->dominance_list();
    auto& dpat_list = src_group->dpat_list();
    for ( auto dpat: dpat_list ) {
      auto group = src_group->subgroup(dpat);
      for ( auto src_dom_group: src_dom_list ) {
	auto& dpat1_list = src_dom_group->dpat_list();
	for ( auto dpat1: dpat1_list ) {
	  if ( (dpat & dpat1) == dpat ) {
	    auto src_dom_subgroup = src_dom_group->subgroup(dpat1);
	    dom_list.push_back(src_dom_subgroup);
	  }
	}
      }
      group->set_dominance_list(std::move(dom_list));
    }
  }

  // 変化があったら更新する．
  if ( check(new_group_list) ) {
    std::swap(mCurGroupList, new_group_list);
    return true;
  }
  // サブグループの情報を初期化しておく．
  for ( auto& group: mCurGroupList ) {
    group->init();
  }
  return false;
}

// @brief 終了処理
EqDomCand
DichoCandMgr::end()
{
  // fault_list() の先頭の故障番号の昇順でソートする．
  std::sort(mCurGroupList.begin(), mCurGroupList.end(),
	    [](const std::unique_ptr<DiGroup>& a,
	       const std::unique_ptr<DiGroup>& b) -> bool {
	      auto f1 = a->fault_list()[0];
	      auto f2 = b->fault_list()[0];
	      return f1.id() < f2.id();
	    });
  // グループ番号を付け直す．
  auto ng = group_num();
  std::vector<TpgFaultList> group_list;
  group_list.reserve(ng);
  std::unordered_map<SizeType, SizeType> id_map;
  for ( SizeType id = 0; id < ng; ++ id ) {
    auto& group = mCurGroupList[id];
    id_map.emplace(group->id(), id);
    group_list.push_back(group->fault_list());
  }
  std::vector<std::pair<SizeType, SizeType>> dom_list;
  for ( SizeType id1 = 0; id1 < ng; ++ id1 ) {
    auto group1 = mCurGroupList[id1].get();
    auto& domgroup_list = group1->dominance_list();
    for ( auto dom_group: domgroup_list ) {
      auto id2 = id_map.at(dom_group->id());
      if ( id2 != id1 ) {
	dom_list.push_back({id1, id2});
      }
    }
  }
  return EqDomCand(group_list, dom_list, true);
}

// @brief 変化があったか調べる．
bool
DichoCandMgr::check(
  const std::vector<std::unique_ptr<DiGroup>>& new_group_list
) const
{
  if ( new_group_list.size() != mCurGroupList.size() ) {
    // グループ数が異なる．
    return true;
  }

  // 支配故障の候補数を調べる．
  SizeType dom_num1 = 0;
  for ( auto& group: mCurGroupList ) {
    dom_num1 += group->dominance_list().size();
  }
  SizeType dom_num2 = 0;
  for ( auto& group: new_group_list ) {
    dom_num2 += group->dominance_list().size();
  }
  return dom_num1 != dom_num2;
}

END_NAMESPACE_DRUID
