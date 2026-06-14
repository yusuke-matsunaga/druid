
/// @file DichoCandMgr.cc
/// @brief DichoCandMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "CandMgr.h"
#include "DichoCandMgr.h"

#define DEBUG 0


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

inline
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
  auto group = new Group(0, 0, {}, fault_list);
  group->set_transitive_succ_list({group});
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
  // 同時にグループごとに現れた dpat のリストを作る．
  auto ng = mCurGroupList.size();
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
#if 0
    std::sort(dpat_list.begin(), dpat_list.end(), std::greater<>());
#endif
  }

#if DEBUG
  {
    std::cout << "DichoCandMgr: after dichotomy" << std::endl;
  }
#endif
  // 細分化したグループを作る．
  std::vector<std::unique_ptr<Group>> new_group_list;
  new_group_list.reserve(new_group_num);
  for ( auto& src_group: mCurGroupList ) {
#if DEBUG
    {
      const char* spc = "";
      for ( auto fault: src_group->fault_list() ) {
	std::cout << spc << fault.str();
	spc = " ";
      }
      std::cout << std::endl;
    }
#endif
    auto& fault_list = src_group->fault_list();
    // dpat をキーにして対象の故障を記録する辞書
    std::unordered_map<PackedVal, TpgFaultList> fault_list_dict;
    auto& dpat_list = dpat_list_array[src_group->id()];
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
      auto rank = src_group->rank() + count_ones(dpat);
      auto pat_list = src_group->pat_list();
      pat_list.push_back(dpat);
      auto new_group = new Group(id, rank, pat_list, fault_list);
      new_group_list.push_back(std::unique_ptr<Group>{new_group});
      src_group->add_subgroup(dpat, new_group);
#if DEBUG
      {
	std::cout << "  [" << pat_str(dpat) << "]:";
	const char* spc = "";
	for ( auto fault: fault_list ) {
	  std::cout << spc << fault.str();
	  spc = " ";
	}
	std::cout << std::endl;
      }
#endif
    }
  }
#if 0
  // 故障番号の昇順にソートする．
  std::sort(new_group_list.begin(), new_group_list.end(),
	    [](std::unique_ptr<Group>& a, std::unique_ptr<Group>& b) -> bool {
	      return a->fault_list()[0].id() < b->fault_list()[0].id();
	    });
#endif

  // 後続のリストを作る．
  for ( auto& src_group: mCurGroupList ) {
    auto& dpat_list = src_group->dpat_list();
    auto& src_succ_list = src_group->transitive_succ_list();
    for ( auto dpat: dpat_list ) {
      auto group = src_group->subgroup(dpat);
      std::vector<Group*> succ_list;
      for ( auto group1: src_succ_list ) {
	auto& dpat1_list = group1->dpat_list();
	for ( auto pat1: dpat1_list ) {
	  if ( (pat1 & dpat) == dpat ) {
	    auto subgroup1 = group1->subgroup(pat1);
	    succ_list.push_back(subgroup1);
	  }
	}
      }
#if 0
      std::sort(succ_list.begin(), succ_list.end(),
		[](Group* a, Group* b) -> bool {
		  return a->fault_list()[0].id() < b->fault_list()[0].id();
		});
#endif
      group->set_transitive_succ_list(std::move(succ_list));
    }
  }

#if DEBUG
  {
    std::cout << "DichoCandMgr::update()" << std::endl;
    print_group_list(std::cout, new_group_list);
    std::cout << std::endl;
  }
#endif

  // 変化があったら更新する．
  if ( check(new_group_list) ) {
    std::swap(mCurGroupList, new_group_list);
    return true;
  }
  // サブグループの情報を初期化しておく．
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
#if DEBUG
  {
    std::cout << "DichoCandMgr::end()" << std::endl;
    print_group_list(std::cout, mCurGroupList);
  }
#endif
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
    auto& succ_list = group1->transitive_succ_list();
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
std::vector<DichoCandMgr::Group*>
DichoCandMgr::sort(
  std::unordered_map<SizeType, SizeType>& id_map
) const
{
  // 故障リストを持つグループを抜き出す．
  std::vector<Group*> tmp_list;
  tmp_list.reserve(mCurGroupList.size());
  for ( auto& group: mCurGroupList ) {
    if ( group->fault_list().empty() ) {
      continue;
    }
    tmp_list.push_back(group.get());
  }
  // fault_list() の先頭の故障番号の昇順でソートする．
  std::sort(tmp_list.begin(), tmp_list.end(),
	    [](Group* a,
	       Group* b) -> bool {
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

BEGIN_NONAMESPACE

// 内容が異なっていたら true を返す．
bool
check_fault_list(
  const TpgFaultList& fault_list1,
  const TpgFaultList& fault_list2
)
{
  auto n1 = fault_list1.size();
  auto n2 = fault_list2.size();
  if ( n1 != n2 ) {
    return true;
  }
  for ( SizeType i = 0; i < n1; ++ i ) {
    if ( fault_list1[i] != fault_list2[i] ) {
      return true;
    }
  }
  return false;
}

END_NONAMESPACE

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
#if 0
  // この場合，グループの順序は同じはず．
  for ( SizeType i = 0; i < ng; ++ i ) {
    auto group1 = mCurGroupList[i].get();
    auto group2 = new_group_list[i].get();
    if ( check_fault_list(group1->fault_list(), group2->fault_list()) ) {
      return true;
    }
    auto& succ_list1 = group1->transitive_succ_list();
    auto& succ_list2 = group2->transitive_succ_list();
    auto n1 = succ_list1.size();
    auto n2 = succ_list2.size();
    if ( n1 != n2 ) {
      return true;
    }
    for ( SizeType j = 0; j < n1; ++ j ) {
      auto& g1 = succ_list1[j];
      auto& g2 = succ_list2[j];
      if ( check_fault_list(g1->fault_list(), g2->fault_list()) ) {
	return true;
      }
    }
  }
#else
  // この場合，グループの順序は同じはず．
  for ( SizeType i = 0; i < ng; ++ i ) {
    auto group1 = mCurGroupList[i].get();
    auto group2 = new_group_list[i].get();
    auto n1 = group1->transitive_succ_list().size();
    auto n2 = group2->transitive_succ_list().size();
    if ( n1 != n2 ) {
      return true;
    }
  }
#endif
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
  const Group* group
)
{
  const char* spc = "";
  for ( auto fault: group->fault_list() ) {
    s << spc << fault.str();
    spc = " ";
  }
  s << std::endl
    << "  ==> ";
  auto succ_list = group->transitive_succ_list();
  std::sort(succ_list.begin(), succ_list.end(),
	    [](DichoCandMgr::Group* a,
	       DichoCandMgr::Group* b) -> bool {
	      return a->id() < b->id();
	    });
  const char* comma = "";
  for ( auto group1: succ_list ) {
    if ( group1 == group ) {
      continue;
    }
    s << comma << group1->fault_list()[0].str();
    comma = ", ";
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
