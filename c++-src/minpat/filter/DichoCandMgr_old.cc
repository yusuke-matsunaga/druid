
/// @file DichoCandMgr.cc
/// @brief DichoCandMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "DichoCandMgr.h"
#include "DiGroup.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス CandMgr
//////////////////////////////////////////////////////////////////////

// @brief 新しいオブジェクトを作る．
std::unique_ptr<CandMgr>
CandMgr::new_dichotomy_mgr(
  const TpgFaultList& fault_list,
  const ConfigParam& option
)
{
  return std::unique_ptr<CandMgr>{new DichoCandMgr(fault_list)};
}


//////////////////////////////////////////////////////////////////////
// クラス DichoCandMgr
//////////////////////////////////////////////////////////////////////

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
  // 同時に現れた d_pat のリストを作る．
  auto ng = mCurGroupList.size();
  std::vector<std::vector<PackedVal>> d_list_array(ng);
  SizeType new_group_num = 0;
  for ( auto& src_group: mCurGroupList ) {
    std::unordered_set<PackedVal> d_hash;
    auto& d_list = d_list_array[src_group->id()];
    for ( auto fault: src_group->fault_list() ) {
      auto d = dpat_array[fault.id()];
      if ( d_hash.count(d) == 0 ) {
	d_hash.insert(d);
	d_list.push_back(d);
	++ new_group_num;
      }
    }
    std::sort(d_list.begin(), d_list.end(), std::greater<>());
  }

  // 細分化したグループを作る．
  std::vector<std::unique_ptr<DiGroup>> new_group_list;
  new_group_list.reserve(new_group_num);
  std::vector<SubGroupInfo> sginfo_array(ng);
  for ( SizeType id = 0; id < ng; ++ id ) {
    auto& src_group = mCurGroupList[id];
    auto& fault_list = src_group->fault_list();
    std::unordered_map<SizeType, TpgFaultList> fault_list_dict;
    auto& d_list = d_list_array[id];
    for ( auto d: d_list ) {
      fault_list_dict.emplace(d, TpgFaultList());
    }
    for ( auto fault: fault_list ) {
      auto d = dpat_array[fault.id()];
      fault_list_dict.at(d).push_back(fault);
    }
    auto& sginfo = sginfo_array[id];
    for ( auto d: d_list ) {
      auto& fault_list = fault_list_dict.at(d);
      if ( fault_list.empty() ) {
	continue;
      }
      auto id = new_group_list.size();
      auto new_group = new DiGroup(id, fault_list);
      new_group_list.push_back(std::unique_ptr<DiGroup>{new_group});
      sginfo.sglist.push_back({d, new_group});
    }
  }

  // dominance list を作る．
  for ( SizeType id = 0; id < ng; ++ id ) {
    auto& src_group = mCurGroupList[id];
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

  // 変化があったら更新する．
  if ( check(new_group_list) ) {
    std::swap(mCurGroupList, new_group_list);
    return true;
  }
  return false;
}

// @brief 終了処理
std::unique_ptr<EqDomCand>
DichoCandMgr::end(
  bool reduce
) const
{
  // fault_list() の先頭の故障番号の昇順でソートする．
  std::unordered_map<SizeType, SizeType> id_map;
  auto tmp_list = sort(id_map);
  // グループ番号を付け直す．
  auto ng = tmp_list.size();
  std::vector<TpgFaultList> group_list;
  group_list.reserve(ng);
  for ( auto group: tmp_list ) {
    group_list.push_back(group->fault_list());
  }
  std::vector<std::pair<SizeType, SizeType>> dom_list;
  for ( auto group1: tmp_list ) {
    auto id1 = id_map.at(group1->id());
    auto& succ_list = group1->dominance_list();
    for ( auto group2: succ_list ) {
      auto id2 = id_map.at(group2->id());
      if ( id2 != id1 ) {
	dom_list.push_back({id1, id2});
      }
    }
  }
  return std::unique_ptr<EqDomCand>{new EqDomCand(group_list, dom_list, reduce)};
}

// @brief 故障番号の昇順にソートする．
std::vector<DiGroup*>
DichoCandMgr::sort(
  std::unordered_map<SizeType, SizeType>& id_map
) const
{
  // 故障リストを持つグループを抜き出す．
  std::vector<DiGroup*> tmp_list;
  tmp_list.reserve(mCurGroupList.size());
  for ( auto& group: mCurGroupList ) {
    if ( group->fault_list().empty() ) {
      continue;
    }
    tmp_list.push_back(group.get());
  }
  // fault_list() の先頭の故障番号の昇順でソートする．
  std::sort(tmp_list.begin(), tmp_list.end(),
	    [](DiGroup* a,
	       DiGroup* b) -> bool {
	      auto f1 = a->fault_list()[0];
	      auto f2 = b->fault_list()[0];
	      return f1.id() < f2.id();
	    });
  // グループ番号を付け直す．
  auto ng = tmp_list.size();
  id_map.clear();
  for ( SizeType id = 0; id < ng; ++ id ) {
    auto group = tmp_list[id];
    id_map.emplace(group->id(), id);
  }
  return tmp_list;
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
