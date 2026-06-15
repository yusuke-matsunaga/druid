
/// @file DichoCandMgr.cc
/// @brief DichoCandMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "DichoCandMgr.h"


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

// @brief コンストラクタ
DichoCandMgr::DichoCandMgr(
  const TpgFaultList& fault_list
) : CandMgr(fault_list)
{
  // 最初は１つのグループ
  auto group = new Group(0, fault_list);
  group->set_succ_list({group});
  mCurGroupList.push_back(std::unique_ptr<Group>{group});
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
  // 同時に現れた dpat のリストを作る．
  auto ng = mCurGroupList.size();
  // グループごとの故障を持つ dpat のリストの配列
  std::vector<std::vector<PackedVal>> dpat_list_array(ng);
  SizeType new_group_num = 0;
  for ( auto& group: mCurGroupList ) {
    std::unordered_set<PackedVal> d_hash;
    auto& dpat_list = dpat_list_array[group->id()];
    for ( auto fault: group->fault_list() ) {
      auto dpat = dpat_array[fault.id()];
      if ( d_hash.count(dpat) == 0 ) {
	d_hash.insert(dpat);
	dpat_list.push_back(dpat);
	++ new_group_num;
      }
    }
    // 機能的には意味がない
    std::sort(dpat_list.begin(), dpat_list.end(), std::greater<>());
  }

  // 細分化したグループを作る．
  std::vector<std::unique_ptr<Group>> new_group_list;
  new_group_list.reserve(new_group_num);
  for ( auto& group: mCurGroupList ) {
    auto& fault_list = group->fault_list();
    std::unordered_map<SizeType, TpgFaultList> fault_list_dict;
    auto& dpat_list = dpat_list_array[group->id()];
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
      auto new_group = new Group(id, fault_list);
      new_group_list.push_back(std::unique_ptr<Group>{new_group});
      group->add_subgroup(dpat, new_group);
    }
  }

  // dominance list を作る．
  for ( SizeType id = 0; id < ng; ++ id ) {
    auto& group = mCurGroupList[id];
    auto& succ_list = group->succ_list();
    auto& dpat_list = group->dpat_list();
    auto& subgroup_list = group->subgroup_list();
    auto ng = subgroup_list.size();
    for ( SizeType i = 0; i < ng; ++ i ) {
      auto dpat = dpat_list[i];
      auto subgroup = subgroup_list[i];
      std::vector<Group*> sub_succ_list;
      for ( auto succ_group: succ_list ) {
	auto& dpat1_list = succ_group->dpat_list();
	auto& subgroup1_list = succ_group->subgroup_list();
	auto ng1 = subgroup1_list.size();
	for ( SizeType i = 0; i < ng1; ++ i ) {
	  auto dpat1 = dpat1_list[i];
	  if ( (dpat & dpat1) == dpat ) {
	    auto subgroup1 = subgroup1_list[i];
	    sub_succ_list.push_back(subgroup1);
	  }
	}
      }
      subgroup->set_succ_list(std::move(sub_succ_list));
    }
  }

  // 変化があったら更新する．
  if ( check(new_group_list) ) {
    std::swap(mCurGroupList, new_group_list);
    return true;
  }

  // 変化がなかったのでそのまま
  // ただしサブグループの情報を初期化しておく．
  for ( auto& group: mCurGroupList ) {
    group->clear_subgroup();
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
  std::vector<Group*> tmp_list;
  tmp_list.reserve(mCurGroupList.size());
  for ( auto& group: mCurGroupList ) {
    tmp_list.push_back(group.get());
  }
  std::sort(tmp_list.begin(), tmp_list.end(),
	    [](Group* a,
	       Group* b) -> bool {
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
  auto ng = tmp_list.size();
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

// @brief 変化があったか調べる．
bool
DichoCandMgr::check(
  const std::vector<std::unique_ptr<Group>>& new_group_list
) const
{
  auto ng = mCurGroupList.size();
  if ( new_group_list.size() != ng ) {
    // グループ数が異なる．
    return true;
  }

  // 支配故障の候補数を調べる．
  for ( SizeType g = 0; g < ng; ++ g ) {
    auto& group1 = mCurGroupList[g];
    auto& group2 = new_group_list[g];
    if ( group1->succ_list().size() != group2->succ_list().size() ) {
      // サブグループの後続グループ数が異なる．
      return true;
    }
  }
  return false;
}

// @brief 故障グループの情報を出力する．
void
DichoCandMgr::print_group_list(
  std::ostream& s,
  const std::vector<std::unique_ptr<Group>>& group_list
)
{
  s << "-------------------------------------------" << std::endl;
  for ( auto& group: group_list ) {
    print_group(s, group.get());
  }
  s << "-------------------------------------------" << std::endl;
}

// @brief 故障グループの情報を出力する．
void
DichoCandMgr::print_group(
  std::ostream& s,
  Group* group
)
{
  s << "[G#" << group->id() << "]:";
  if ( group->fault_list().empty() ) {
    s << "***";
  }
  else {
    for ( auto fault: group->fault_list() ) {
      s << " " << fault.str();
    }
  }
  s << std::endl
    << "  ==> ";
  auto& succ_list = group->succ_list();
  const char* comma = "";
  for ( auto group: succ_list ) {
    s << comma << "[G#" << group->id() << "]";
    comma = ", ";
    if ( !group->fault_list().empty() ) {
      s << "(" << group->fault_list()[0].str() << ")";
    };
  }
  s << std::endl;
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
