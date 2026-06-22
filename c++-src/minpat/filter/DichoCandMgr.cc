
/// @file DichoCandMgr.cc
/// @brief DichoCandMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "DichoCandMgr.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス EqDomCandMgr
//////////////////////////////////////////////////////////////////////

// @brief 新しいオブジェクトを作る．
std::unique_ptr<EqDomCandMgr>
EqDomCandMgr::new_dichotomy_mgr(
  const TpgFaultList& fault_list,
  const ConfigParam& option
)
{
  return std::unique_ptr<EqDomCandMgr>{new DichoCandMgr(fault_list)};
}


//////////////////////////////////////////////////////////////////////
// クラス DichoCandMgr
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
DichoCandMgr::DichoCandMgr(
  const TpgFaultList& fault_list
) : EqDomCandMgr(fault_list)
{
  // 最初は１つのグループ
  auto group = new DichoGroup(0, fault_list);
  group->set_succ_list({group});
  mCurGroupList.push_back(DichoGroup::Ptr{group});
  _fix_group_map();
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
  // 細分化したサブグループの情報
  struct SubGroupInfo {
    PackedVal dpat;
    DichoGroup* group;
  };

  // パタンをキーしたGroupの辞書
  using GroupDict = std::unordered_map<PackedVal, DichoGroup*>;

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
      auto dpat = dpat_array[fault.id()];
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
      auto dpat = sg.dpat;
      auto subgroup = sg.group;
      std::vector<DichoGroup*> sub_succ_list;
      for ( auto succ_group: succ_list ) {
	auto& sg_list1 = sg_list_array[succ_group->id()];
	for ( auto& sg1: sg_list1 ) {
	  auto dpat1 = sg1.dpat;
	  if ( (dpat & dpat1) == dpat ) {
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

// @brief 終了処理
std::unique_ptr<EqDomCand>
DichoCandMgr::end(
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

// @brief 等価故障グループの候補を返す．
TpgFaultList
DichoCandMgr::eqcand(
  const TpgFault& fault
) const
{
  auto group = mGroupMap[fault.id()];
  return group->fault_list();
}

// @brief mGroupMap を作る．
void
DichoCandMgr::_fix_group_map()
{
  mGroupMap.clear();
  mGroupMap.resize(max_fault_size(), nullptr);
  for ( auto& group: mCurGroupList ) {
    for ( auto fault: group->fault_list() ) {
      mGroupMap[fault.id()] = group.get();
    }
  }
}

// @brief 故障グループの情報を出力する．
void
DichoCandMgr::print_group_list(
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

// @brief パタンを文字列にする．
std::string
DichoCandMgr::pat_str(
  PackedVal pat
)
{
  std::ostringstream buf;
  buf << "[" << std::hex << pat << std::dec << "]";
  return buf.str();
}

// @brief パタンのリストを文字列にする．
std::string
DichoCandMgr::pat_list_str(
  const std::vector<PackedVal>& pat_list
)
{
  std::ostringstream buf;
  buf << "[" << std::hex;
  for ( auto pat: pat_list ) {
    buf << pat;
  }
  buf << std::dec << "]";
  return buf.str();
}

END_NAMESPACE_DRUID
