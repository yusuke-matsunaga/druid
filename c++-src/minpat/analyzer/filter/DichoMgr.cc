
/// @file DichoMgr.cc
/// @brief DichoMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "DichoMgr.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス EqGroupMgr
//////////////////////////////////////////////////////////////////////

// @brief 新しいオブジェクトを作る．
std::unique_ptr<EqGroupMgr>
EqGroupMgr::new_dichotomy_mgr(
  FaultInfo& fault_info,
  Fsim& fsim,
  const ConfigParam& option
)
{
  return std::unique_ptr<EqGroupMgr>{new DichoMgr(fault_info, fsim, option)};
}


//////////////////////////////////////////////////////////////////////
// クラス DichoMgr
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
DichoMgr::DichoMgr(
  FaultInfo& fault_info,
  Fsim& fsim,
  const ConfigParam& option
) : EqGroupMgr(fault_info, fsim, option)
{
  // 最初は１つのグループ
  auto group = new DichoGroup(0, fault_info.rep_fault_list());
  group->set_succ_list({group});
  mCurGroupList.push_back(DichoGroup::Ptr{group});
  _fix_group_map();
}

// @brief デストラクタ
DichoMgr::~DichoMgr()
{
}

// @brief 更新処理
bool
DichoMgr::update(
  const std::vector<DPat>& dpat_array
)
{
  // 細分化したサブグループの情報
  struct SubGroupInfo {
    DPat dpat;
    DichoGroup* group;
  };

  // パタンをキーしたGroupの辞書
  using GroupDict = std::unordered_map<DPat, DichoGroup*>;

  // もとのグループ数
  auto ng = mCurGroupList.size();

  // グループごとのSubGroupInfoのリストの配列
  std::vector<std::vector<SubGroupInfo>> sg_list_array(ng);

  // 細分化したグループのリスト
  std::vector<DichoGroup::Ptr> new_group_list;

  // 細分化したサブグループを作る．
  for ( auto& group: mCurGroupList ) {
    // group の故障をパタンごとに分ける．
    GroupDict group_dict;
    auto& sg_list = sg_list_array[group->id()];
    for ( auto fault: group->fault_list() ) {
      auto& dpat = dpat_array[fault.id()];
      if ( group_dict.count(dpat) == 0 ) {
	// 新しいグループを作る．
	auto id = new_group_list.size();
	auto new_group = new DichoGroup(id);
	new_group->add_fault(fault);
	new_group_list.push_back(DichoGroup::Ptr{new_group});
	group_dict.emplace(dpat, new_group);
	sg_list.push_back({dpat, new_group});
      }
      else {
	auto group = group_dict.at(dpat);
	group->add_fault(fault);
      }
    }
  }

  // 変化があったことを示すフラグ
  bool changed = new_group_list.size() != ng;

  // dominance list を作る．
  for ( auto& group: mCurGroupList ) {
    auto& succ_list = group->succ_list();
    auto& sg_list = sg_list_array[group->id()];
    for ( auto& sg: sg_list ) {
      auto& dpat = sg.dpat;
      auto subgroup = sg.group;
      std::vector<DichoGroup*> sub_succ_list;
      for ( auto succ_group: succ_list ) {
	auto& sg_list1 = sg_list_array[succ_group->id()];
	for ( auto& sg1: sg_list1 ) {
	  auto& dpat1 = sg1.dpat;
	  if ( dpat.check_contained(dpat1) ) {
	    auto subgroup1 = sg1.group;
	    sub_succ_list.push_back(subgroup1);
	  }
	}
      }
      subgroup->set_succ_list(std::move(sub_succ_list));
    }
    if ( !changed && sg_list.size() == 1 ) {
      // 後続リストの要素数が変化しているか調べる．
      auto& subgroup = sg_list.front().group;
      if ( group->succ_list().size() != subgroup->succ_list().size() ) {
	changed = true;
      }
    }
  }

  // 変化があったら更新する．
  if ( changed ) {
    std::swap(mCurGroupList, new_group_list);
    _fix_group_map();
    return true;
  }
  return false;
}

#if EQDOMCAND
// @brief 終了処理
std::unique_ptr<EqDomCand>
DichoMgr::end(
  bool reduce
) const
{
  // fault_list() の先頭の故障番号の昇順でソートする．
  // mCurGroupList は変更できないので tmp_list にコピーする．
  std::vector<DichoGroup*> tmp_list;
  tmp_list.reserve(mCurGroupList.size());
  for ( auto& group: mCurGroupList ) {
    tmp_list.push_back(group.get());
  }
  std::sort(tmp_list.begin(), tmp_list.end(),
	    [](DichoGroup* a,
	       DichoGroup* b) -> bool {
	      auto f1 = a->fault_list()[0];
	      auto f2 = b->fault_list()[0];
	      return f1.id() < f2.id();
	    });

  // グループ番号を付け直す．
  std::unordered_map<SizeType, SizeType> id_map;
  auto ng = tmp_list.size();
  for ( SizeType id = 0; id < ng; ++ id ) {
    auto group = tmp_list[id];
    id_map.emplace(group->id(), id);
  }

  // 故障グループを作る．
  std::vector<TpgFaultList> group_list;
  group_list.reserve(ng);
  for ( auto group: tmp_list ) {
    group_list.push_back(group->fault_list());
  }

  // 順序関係を表すペアのリストを作る．
  std::vector<std::pair<SizeType, SizeType>> succ_pair_list;
  for ( auto group1: tmp_list ) {
    auto id1 = id_map.at(group1->id());
    auto& succ_list = group1->succ_list();
    for ( auto group2: succ_list ) {
      auto id2 = id_map.at(group2->id());
      if ( id2 != id1 ) {
	succ_pair_list.push_back({id1, id2});
      }
    }
  }
  return std::unique_ptr<EqDomCand>{new EqDomCand(group_list, succ_pair_list, reduce)};
}
#endif

// @brief 等価故障グループ数を返す．
SizeType
DichoMgr::group_num() const
{
  return mCurGroupList.size();
}

// @brief 等価故障グループ番号を返す．
SizeType
DichoMgr::group_id(
  const TpgFault& fault
) const
{
  auto group = mGroupMap[fault.id()];
  return group->id();
}

// @brief 等価故障グループの故障リストを返す．
TpgFaultList
DichoMgr::fault_list(
  SizeType group_id
) const
{
  _check_group_id(group_id);
  auto& group = mCurGroupList[group_id];
  auto& src_list = group->fault_list();
  TpgFaultList ans_list;
  ans_list.reserve(src_list.size());
  for ( auto fault: src_list ) {
    if ( is_rep(fault) ) {
      ans_list.push_back(fault);
    }
  }
  return ans_list;
}

// @brief 後続グループ番号のリスト返す．
std::vector<SizeType>
DichoMgr::succ_list(
  SizeType group_id
) const
{
  _check_group_id(group_id);
  auto& group = mCurGroupList[group_id];
  std::vector<SizeType> ans_list;
  auto& src_list = group->succ_list();
  ans_list.reserve(src_list.size());
  for ( auto group1: src_list ) {
    if ( group1->id() == group->id() ) {
      // self-loop は記録しない．
      continue;
    }
    ans_list.push_back(group1->id());
  }
  return ans_list;
}

// @brief 先行グループ番号のリスト返す．
std::vector<SizeType>
DichoMgr::pred_list(
  SizeType group_id
) const
{
  _check_group_id(group_id);
  auto& group = mCurGroupList[group_id];
  std::vector<SizeType> ans_list;
  auto& src_list = group->pred_list();
  ans_list.reserve(src_list.size());
  for ( auto group1: src_list ) {
    if ( group1->id() == group->id() ) {
      // self-loop は記録しない．
      continue;
    }
    ans_list.push_back(group1->id());
  }
  return ans_list;
}

// @brief 順序関係の要素数を返す．
SizeType
DichoMgr::domcand_num() const
{
  SizeType num = 0;
  for ( auto& group: mCurGroupList ) {
    num += group->succ_list().size();
  }
  return num;
}

// @brief mGroupMap を作る．
void
DichoMgr::_fix_group_map()
{
  mGroupMap.clear();
  mGroupMap.resize(max_fault_size(), nullptr);
  for ( auto& group: mCurGroupList ) {
    for ( auto fault: group->fault_list() ) {
      mGroupMap[fault.id()] = group.get();
    }
    for ( auto succ_group: group->succ_list() ) {
      succ_group->add_pred(group.get());
    }
  }
}

// @brief 故障グループの情報を出力する．
void
DichoMgr::print_group_list(
  std::ostream& s,
  const std::vector<DichoGroup::Ptr>& group_list
)
{
  s << "-------------------------------------------" << std::endl;
  for ( auto& group: group_list ) {
    group->print(s);
  }
  s << "-------------------------------------------" << std::endl;
}

END_NAMESPACE_DRUID
