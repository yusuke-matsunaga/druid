
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
  // パタンをキーした故障リストの辞書
  using FaultsDict = std::unordered_map<PackedVal, TpgFaultList>;

  auto ng = mCurGroupList.size();

  // まずグループの数を数える．
  // 同時に現れた dpat のリストを作る．

  // サブグループの情報の配列
  // キーは現在のグループ番号
  std::vector<SubGroupInfo> subgroup_info_array(ng);

  // グループごとのパタンに対する故障リストの辞書の配列
  std::vector<FaultsDict> faults_dict_array(ng);
  SizeType new_group_num = 0;
  for ( auto& group: mCurGroupList ) {
    auto& faults_dict = faults_dict_array[group->id()];
    auto& subgroup_info = subgroup_info_array[group->id()];
    auto& dpat_list = subgroup_info.dpat_list;
    for ( auto fault: group->fault_list() ) {
      auto dpat = dpat_array[fault.id()];
      if ( faults_dict.count(dpat) == 0 ) {
	faults_dict.emplace(dpat, TpgFaultList());
	dpat_list.push_back(dpat);
	++ new_group_num;
      }
      auto& fault_list = faults_dict.at(dpat);
      fault_list.push_back(fault);
    }
    // 機能的には意味がない
    std::sort(dpat_list.begin(), dpat_list.end(), std::greater<>());
  }

  // 細分化したグループのリスト
  std::vector<std::unique_ptr<Group>> new_group_list;
  // 細分化したサブグループを作る．
  new_group_list.reserve(new_group_num);
  for ( auto& group: mCurGroupList ) {
    auto& fault_list = group->fault_list();
    auto& faults_dict = faults_dict_array[group->id()];
    auto& subgroup_info = subgroup_info_array[group->id()];
    auto& dpat_list = subgroup_info.dpat_list;
    for ( auto dpat: dpat_list ) {
      auto& fault_list = faults_dict.at(dpat);
      if ( fault_list.empty() ) {
	continue;
      }
      auto id = new_group_list.size();
      auto new_group = new Group(id, fault_list);
      new_group_list.push_back(std::unique_ptr<Group>{new_group});
      subgroup_info.group_list.push_back(new_group);
    }
  }

  // dominance list を作る．
  for ( auto& group: mCurGroupList ) {
    auto& succ_list = group->succ_list();
    auto& sg_info = subgroup_info_array[group->id()];
    auto& dpat_list = sg_info.dpat_list;
    auto& subgroup_list = sg_info.group_list;
    auto ng = subgroup_list.size();
    for ( SizeType i = 0; i < ng; ++ i ) {
      auto dpat = dpat_list[i];
      auto subgroup = subgroup_list[i];
      std::vector<Group*> sub_succ_list;
      for ( auto succ_group: succ_list ) {
	auto& sg_info1 = subgroup_info_array[succ_group->id()];
	auto& dpat1_list = sg_info.dpat_list;
	auto& subgroup1_list = sg_info.group_list;
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
