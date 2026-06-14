
/// @file DichoCandMgr2.cc
/// @brief DichoCandMgr2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "CandMgr.h"
#include "DichoCandMgr2.h"
#include "DPatGraph.h"

#define DEBUG 0

BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// pat_list1 が pat_list2 を支配している時 true を返す．
inline
bool
check_pat_list(
  const std::vector<PackedVal>& pat_list1,
  const std::vector<PackedVal>& pat_list2
)
{
  auto n = pat_list1.size();
  if ( pat_list2.size() != n ) {
    throw std::invalid_argument{"pat_list size mismatch"};
  }
  for ( SizeType i = 0; i < n; ++ i ) {
    auto pat1 = pat_list1[i];
    auto pat2 = pat_list2[i];
    if ( (pat1 & pat2) != pat1 ) {
      return false;
    }
  }
  return true;
}

END_NONAMESPACE


//////////////////////////////////////////////////////////////////////
// クラス DichoCandMgr2
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
DichoCandMgr2::DichoCandMgr2(
  const TpgFaultList& fault_list
) : CandMgr(fault_list)
{
}

// @brief デストラクタ
DichoCandMgr2::~DichoCandMgr2()
{
}

BEGIN_NONAMESPACE

void
dfs(
  DichoCandMgr2::Group* group,
  std::unordered_set<SizeType>& mark,
  std::vector<DichoCandMgr2::Group*>& ans_list
)
{
  if ( mark.count(group->id()) > 0 ) {
    return;
  }
  mark.insert(group->id());
  if ( !group->fault_list().empty() ) {
    ans_list.push_back(group);
    return;
  }
  for ( auto group1: group->immediate_succ_list() ) {
    dfs(group1, mark, ans_list);
  }
}

bool
dfs2(
  DichoCandMgr2::Group* group,
  std::unordered_set<SizeType>& mark,
  DichoCandMgr2::Group* target
)
{
  if ( group == target ) {
    return true;
  }
  if ( mark.count(group->id()) > 0 ) {
    return false;
  }
  mark.insert(group->id());
  if ( group->rank() >= target->rank() ) {
    return false;
  }
  for ( auto group1: group->immediate_succ_list() ) {
    if ( dfs2(group1, mark, target) ) {
      return true;
    }
  }
  return false;
}

bool
reachable(
  const std::vector<DichoCandMgr2::Group*>& group_list,
  SizeType start,
  DichoCandMgr2::Group* target
)
{
  std::unordered_set<SizeType> mark;
  auto n = group_list.size();
  for ( SizeType i = start; i < n; ++ i ) {
    auto group1 = group_list[i];
    if ( dfs2(group1, mark, target) ) {
      return true;
    }
  }
  return false;
}

// 空のグループと推移的な後続を取り除く
std::vector<DichoCandMgr2::Group*>
true_succ_list(
  const std::vector<DichoCandMgr2::Group*>& src_list
)
{
  // まず単純にDFSで到達可能なグループを求める．
  std::unordered_set<SizeType> mark;
  std::vector<DichoCandMgr2::Group*> tmp_list;
  for ( auto group: src_list ) {
    dfs(group, mark, tmp_list);
  }
  // tmp_list に含まれるグループの最大 rank までBFSを行い，
  // 冗長な枝を取り除く．
  std::sort(tmp_list.begin(), tmp_list.end(),
	    [](DichoCandMgr2::Group* a,
	       DichoCandMgr2::Group* b) -> bool {
	      return a->rank() > b->rank();
	    });
  auto n = tmp_list.size();
  std::vector<DichoCandMgr2::Group*> ans_list;
  ans_list.reserve(n);
  for ( SizeType i = 0; i < n; ++ i ) {
    auto group1 = tmp_list[i];
    if ( !reachable(tmp_list, i + 1, group1) ) {
      ans_list.push_back(group1);
    }
  }
  // 故障番号の昇順にソートする．
  std::sort(ans_list.begin(), ans_list.end(),
	    [](DichoCandMgr2::Group* a, DichoCandMgr2::Group* b) -> bool {
	      return a->fault_list()[0].id() < b->fault_list()[0].id();
	    });
  return ans_list;
}

void
mark_succ(
  DichoCandMgr2::Group* group,
  std::unordered_set<SizeType>& mark
)
{
  for ( auto group1: group->transitive_succ_list() ) {
    if ( group1 == group ) {
      continue;
    }
    if ( mark.count(group1->id()) == 0 ) {
      mark.insert(group1->id());
      mark_succ(group1, mark);
    }
  }
}

std::vector<DichoCandMgr2::Group*>
get_imm_succ(
  DichoCandMgr2::Group* group
)
{
  auto& succ_list = group->transitive_succ_list();
  std::unordered_set<SizeType> mark;
  mark.insert(group->id());
  for ( auto group1: succ_list ) {
    if ( group1 != group ) {
      mark_succ(group1, mark);
    }
  }
  std::vector<DichoCandMgr2::Group*> ans_list;
  for ( auto group1: succ_list ) {
    if ( mark.count(group1->id()) == 0 ) {
      ans_list.push_back(group1);
    }
  }
  std::sort(ans_list.begin(), ans_list.end(),
	    [](DichoCandMgr2::Group* a, DichoCandMgr2::Group* b) -> bool {
	      return a->id() < b->id();
	    });
  return ans_list;
}

void
check_succ_list(
  DichoCandMgr2::Group* group
)
{
  auto succ_list1 = group->immediate_succ_list();
  std::sort(succ_list1.begin(), succ_list1.end(),
	    [](DichoCandMgr2::Group* a, DichoCandMgr2::Group* b) -> bool {
	      return a->id() < b->id();
	    });
  auto succ_list2 = get_imm_succ(group);
  if ( succ_list1 != succ_list2 ) {
    std::cout << "Group#" << group->id() << std::endl;
    std::cout << "  immediate: ";
    for ( auto group: succ_list1 ) {
      std::cout << " Group#" << group->id();
    }
    std::cout << std::endl;
    std::cout << "  immediate2: ";
    for ( auto group: succ_list2 ) {
      std::cout << " Group#" << group->id();
    }
    std::cout << std::endl;
    std::cout << "  transitve: ";
    for ( auto group: group->transitive_succ_list() ) {
      std::cout << " Group#" << group->id();
    }
    std::cout << std::endl;
    abort();
  }
}

END_NONAMESPACE

// @brief 更新処理
bool
DichoCandMgr2::update(
  const std::vector<PackedVal>& dpat_array
)
{
  DPatGraph dpat_graph(dpat_array);

  // 重複のないパタンのリストを求める．
  auto all_dpat_list = dpat_graph.pat_list();

#if DEBUG
  {
    std::cout << "DichoCandMgr2: after dichotomy" << std::endl;
  }
#endif

  // 全てのグループに対して all_dpat_list に基づいた細分化を行う．
  auto npat = all_dpat_list.size();
  auto ng = mCurGroupList.size();
  auto new_group_num = ng * npat;
  std::vector<std::unique_ptr<Group>> new_group_list;
  new_group_list.reserve(new_group_num);
  for ( auto& group: mCurGroupList ) {
#if DEBUG
    {
      const char* spc = "";
      for ( auto fault: group->fault_list() ) {
	std::cout << spc << fault.str();
	spc = " ";
      }
      std::cout << std::endl;
    }
#endif
    auto& fault_list = group->fault_list();
    // dpat をキーにして対象の故障リストを持つ辞書
    std::unordered_map<PackedVal, TpgFaultList> fault_list_dict;
    for ( auto pat: all_dpat_list ) {
      fault_list_dict.emplace(pat, TpgFaultList());
    }
    for ( auto fault: fault_list ) {
      auto pat = dpat_array[fault.id()];
      fault_list_dict.at(pat).push_back(fault);
    }
    for ( auto pat: all_dpat_list ) {
      // fault_list は空の場合もある．
      auto& fault_list = fault_list_dict.at(pat);
      auto id = new_group_list.size();
      auto rank = group->rank() + count_ones(pat);
      auto pat_list = group->pat_list();
      pat_list.push_back(pat);
      auto new_group = new Group(id, rank, pat_list, fault_list);
      new_group_list.push_back(std::unique_ptr<Group>{new_group});
      group->add_subgroup(pat, new_group);
#if DEBUG
      {
	if ( !fault_list.empty() ) {
	  std::cout << "  [" << pat_str(pat) << "]:";
	  const char* spc = "";
	  for ( auto fault: fault_list ) {
	    std::cout << spc << fault.str();
	    spc = " ";
	  }
	  std::cout << std::endl;
	}
      }
#endif
    }
  }

  // successor list を作る．
  for ( auto& src_group: mCurGroupList ) {
    auto& src_succ_list = src_group->immediate_succ_list();
    for ( auto dpat: all_dpat_list ) {
      auto subgroup = src_group->subgroup(dpat);
      // 後続グループのリスト
      std::vector<Group*> succ_list;
      // 同じ src_group から細分化されたサブグループの直接の後続を求める．
      for ( auto pat1: dpat_graph.imm_succ_list(dpat) ) {
	auto subgroup1 = src_group->subgroup(pat1);
	succ_list.push_back(subgroup1);
      }

      // group の後続に対する細分化
      for ( auto src_group1: src_succ_list ) {
	auto subgroup1 = src_group1->subgroup(dpat);
	succ_list.push_back(subgroup1);
      }
      subgroup->set_immediate_succ_list(std::move(succ_list));
    }
  }

#if DEBUG
  std::cout << "DichoCandMgr2: after initial link" << std::endl;
  print_group_list(std::cout, new_group_list);
#endif

  // 故障を持たないグループをスキップして succ_list を再構築する．
  for ( auto& group: new_group_list ) {
    if ( !group->fault_list().empty() ) {
      auto imm_succ_list = true_succ_list(group->immediate_succ_list());
      group->set_immediate_succ_list(std::move(imm_succ_list));
    }
  }

#if DEBUG
  // 検証用に推移的な後続を求める．
  // DichoCandMgr と同じアルゴリズム
  for ( auto& group: mCurGroupList ) {
    for ( auto pat: all_dpat_list ) {
      auto subgroup = group->subgroup(pat);
      std::vector<Group*> succ_list;
      if ( subgroup->fault_list().empty() ) {
	continue;
      }
      for ( auto group1: group->transitive_succ_list() ) {
	if ( group1->fault_list().empty() ) {
	  continue;
	}
	for ( auto pat1: all_dpat_list ) {
	  if ( (pat1 & pat) == pat ) {
	    auto subgroup1 = group1->subgroup(pat1);
	    if ( !subgroup1->fault_list().empty() ) {
	      succ_list.push_back(subgroup1);
	    }
	  }
	}
      }
      subgroup->set_transitive_succ_list(std::move(succ_list));
    }
  }
  // 推移的な後続から直接の後続を求めチェックする．
  for ( auto& group: mCurGroupList ) {
    for ( auto pat: all_dpat_list ) {
      auto subgroup = group->subgroup(pat);
      if ( !subgroup->fault_list().empty() ) {
	check_succ_list(subgroup);
      }
    }
  }

  // 第２の検証
  // pat_list() から支配関係を計算する．
  std::vector<Group*> new_group_list2;
  new_group_list2.reserve(new_group_list.size());
  for ( auto& group: new_group_list ) {
    if ( !group->fault_list().empty() ) {
      new_group_list2.push_back(group.get());
    }
  }
  auto ng2 = new_group_list2.size();
  std::unordered_map<SizeType, std::vector<Group*>> succ_list_dict;
  for ( SizeType i1 = 0; i1 < ng2; ++ i1 ) {
    auto group1 = new_group_list2[i1];
    succ_list_dict.emplace(group1->id(), std::vector<Group*>{});
    auto& succ_list = succ_list_dict.at(group1->id());
    for ( SizeType i2 = 0; i2 < ng2; ++ i2 ) {
      if ( i1 == i2 ) {
	continue;
      }
      auto group2 = new_group_list2[i2];
      auto r = check_pat_list(group1->pat_list(), group2->pat_list());
      if ( r ) {
	succ_list.push_back(group2);
      }
    }
  }
  std::unordered_map<SizeType, std::vector<Group*>> imm_succ_list_dict;
  for ( SizeType i = 0; i < ng2; ++ i ) {
    auto group1 = new_group_list2[i];
    auto& succ_list = succ_list_dict.at(group1->id());
    std::unordered_set<SizeType> mark;
    for ( auto group1: succ_list ) {
      auto& succ_list1 = succ_list_dict.at(group1->id());
      for ( auto group2: succ_list1 ) {
	mark.insert(group2->id());
      }
    }
    imm_succ_list_dict.emplace(group1->id(), std::vector<Group*>{});
    auto& imm_succ_list = imm_succ_list_dict.at(group1->id());
    for ( auto group1: succ_list ) {
      if ( mark.count(group1->id()) == 0 ) {
	imm_succ_list.push_back(group1);
      }
    }
    std::sort(imm_succ_list.begin(), imm_succ_list.end(),
	      [](Group* a, Group* b) -> bool {
		return a->id() < b->id();
	      });
    auto imm_succ_list2 = group1->immediate_succ_list();
    std::sort(imm_succ_list2.begin(), imm_succ_list2.end(),
	      [](Group* a, Group* b) -> bool {
		return a->id() < b->id();
	      });
    if ( imm_succ_list != imm_succ_list2 ) {
      std::cout << "Group#" << group1->id() << std::endl;
      std::cout << "  group1->immediate_succ_list(): ";
      for ( auto group: imm_succ_list2 ) {
	std::cout << " Group#" << group->id();
      }
      std::cout << std::endl;
      std::cout << "  imm_succ_list                : ";
      for ( auto group: imm_succ_list ) {
	std::cout << " Group#" << group->id();
      }
      std::cout << std::endl;
      std::cout << "  transitive_succ_list         : ";
      auto& succ_list = succ_list_dict.at(group1->id());
      for ( auto group: succ_list ) {
	std::cout << " Group#" << group->id();
      }
      std::cout << std::endl;
      abort();
    }
  }
#endif

  normalize(new_group_list);

#if DEBUG
  std::cout << "DichoCandMgr2: after clean up" << std::endl;
  print_group_list(std::cout, new_group_list);
  std::cout << std::endl;
#endif

  // 変化があったら更新する．
  if ( check(new_group_list) ) {
    std::swap(mCurGroupList, new_group_list );
#if DEBUG
    {
      std::cout << "DichoCandMgr2::update()" << std::endl;
      print(std::cout);
    }
#endif
    return true;
  }

#if DEBUG
  {
    std::cout << "DichoCandMgr2::update(NO CHANGE)" << std::endl;
    print(std::cout);
  }
#endif

  // 変化がない場合はサブグループの情報を消去する．
  for ( auto& group: mCurGroupList ) {
    group->clear_subgroup();
  }
  return false;
}

// @brief 終了処理
std::unique_ptr<EqDomCand>
DichoCandMgr2::end(
  bool reduce
) const
{
#if DEBUG
  {
    std::cout << "DichoCandMgr2::end()" << std::endl;
    print(std::cout);
  }
#endif

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
    auto& succ_list = group1->immediate_succ_list();
    for ( auto group2: succ_list ) {
      auto id2 = id_map.at(group2->id());
      if ( id2 != id1 ) {
	dom_list.push_back({id1, id2});
      }
    }
  }
  return std::unique_ptr<EqDomCand>{new EqDomCand(group_list, dom_list)};
}

// @brief 故障番号の昇順にソートする．
std::vector<DichoCandMgr2::Group*>
DichoCandMgr2::sort(
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

// @brief グループのリストを正規化する．
void
DichoCandMgr2::normalize(
  std::vector<std::unique_ptr<Group>>& group_list
)
{
  // 故障を持たないグループを削除して故障番号の昇順にソートする．
  { // std::unique_ptr なのでめんどくさい
    auto n = group_list.size();
    SizeType last = 0;
    for ( SizeType i = 0; i < n; ++ i ) {
      auto& group = group_list[i];
      if ( !group->fault_list().empty() ) {
	if ( i != last ) {
	  std::swap(group_list[i], group_list[last]);
	}
	++ last;
      }
    }
    if ( last < n ) {
      group_list.erase(group_list.begin() + last, group_list.end());
    }
  }
  std::sort(group_list.begin(), group_list.end(),
	    [](std::unique_ptr<Group>& a, std::unique_ptr<Group>& b) -> bool {
	      return a->fault_list()[0].id() < b->fault_list()[0].id();
	    });
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
DichoCandMgr2::check(
  const std::vector<std::unique_ptr<Group>>& new_group_list
) const
{
  auto ng = mCurGroupList.size();
  if ( new_group_list.size() != ng ) {
    // グループ数が異なる．
    return true;
  }
  // この場合，グループの順序は同じはず．
  for ( SizeType i = 0; i < ng; ++ i ) {
    auto group1 = mCurGroupList[i].get();
    auto group2 = new_group_list[i].get();
    if ( check_fault_list(group1->fault_list(), group2->fault_list()) ) {
      return true;
    }
    auto& succ_list1 = group1->immediate_succ_list();
    auto& succ_list2 = group2->immediate_succ_list();
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
  return false;
}

// @brief 内容を出力する．
void
DichoCandMgr2::print(
  std::ostream& s
) const
{
  s << "-------------------------------------------" << std::endl;
  for ( auto& group: mCurGroupList ) {
    if ( group->fault_list().empty() ) {
      continue;
    }
    const char* spc = "";
    for ( auto fault: group->fault_list() ) {
      s << spc << fault.str();
      spc = " ";
    }
    s << std::endl;
    auto succ_list = group->immediate_succ_list();
    std::sort(succ_list.begin(), succ_list.end(),
	      [](Group* a, Group* b) -> bool {
		return a->id() < b->id();
	      });
    const char* comma = "";
    s << "  ==>";
    for ( auto group1: succ_list ) {
      s << comma << group1->fault_list()[0].str();
      comma = ", ";
    }
    s << std::endl;
  }
  s << "-------------------------------------------" << std::endl;
}

// @brief 故障グループの情報を出力する．
void
DichoCandMgr2::print_group_list(
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
DichoCandMgr2::print_group(
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
  auto& succ_list = group->immediate_succ_list();
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

END_NAMESPACE_DRUID
