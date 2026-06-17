
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

END_NONAMESPACE

// @brief 更新処理
bool
DichoCandMgr2::update(
  const std::vector<PackedVal>& dpat_array
)
{
  // パタン間の順序関係を表すグラフ
  DPatGraph dpat_graph(all_dpat_list);

  // 重複のないパタンのリスト
  auto all_dpat_list = dpat_graph.pat_list();

  // パタン数
  auto npat = all_dpat_list.size();

  // もとのグループ数
  auto ng = mCurGroupList.size();

  // サブグループの配列
  // キーは group->id() * npat + pat_id
  std::vector<SubGroup> subgroup_array(ng * npat);
  // サブグループ間の順序関係を作る．
  for ( auto& group: mCurGroupList ) {
    auto offset = group->id() * npat;
    auto succ_list = group.succ_list();
    for ( SizeType pid = 0; pid < npat; ++ pid ) {
      auto id = offset + pid;
      auto& subgroup = subgroup_array[id];
      subgroup.id = id;
      // 同じグループから細分化したサブグループ
      auto succ_id_list = dpat_graph.poset().imm_succ_list(pid);
      for ( auto pid1: succ_id_list ) {
	auto id1 = offset + pid1;
	auto& subgroup1 = subgroup_array[id1];
	subgroup.succ_list.push_back(&subgroup1);
      }
      // group の後続のサブグループで同じパタン番号を持つもの
      for ( auto group1: succ_list ) {
	auto offset1 = group1->id() * npat;
	auto id1 = offset1 + pid;
	auto& subgroup1 = subgroup_array[id1];
	subgroup.succ_list.push_back(&subgroup1);
      }
    }
  }

  // 故障をサブグループに割り当てる．
  for ( auto& group: mCurGroupList ) {
    auto offset = group->id() * npat;
    for ( auto fault: group->fault_list() ) {
      auto dpat = dpat_array[fault.id()];
      auto pid = pat_map.at(dpat);
      auto id = offset + pid;
      auto& subgroup = subgroup_array[id];
      subgroup.fault_list.push_back(fault);
    }
  }

  // 細分化したグループのリスト
  std::vector<std::unique_ptr<Group>> new_group_list;
  // Group::id() をキーにして SubGroup 番号を格納する配列
  std::vector<SubGroup*> subgroup_list;
  { // 要素数を数える．
    SizeType n = 0;
    for ( auto& group: mCurGroupList ) {
      auto offset = group->id() * npat;
      for ( SizeType pid = 0; pid < npat; ++ pid ) {
	auto id = offset + pid;
	auto& subgroup = subgroup_array[id];
	auto& fault_list = subgroup.fault_list;
	if ( !fault_list.empty() ) {
	  ++ n;
	}
      }
    }
    new_group_list.reserve(n);
    subgroup_list.reserve(n);
  }

  // 故障を持つサブグループにグループを結びつける．
  for ( auto& group: mCurGroupList ) {
    auto offset = group->id() * npat;
    for ( SizeType pid = 0; pid < npat; ++ pid ) {
      auto id = offset + pid;
      auto& subgroup = subgroup_array[id];
      auto& fault_list = subgroup.fault_list;
      if ( fault_list.empty() ) {
	subgroup.group = nullptr;
      }
      else {
	auto gid = new_group_list.size();
	auto new_group = new Group(gid, fault_list);
	new_group_list.push_back(std::unique_ptr<Group>{new_group});
	subgroup.group = new_group;
	subgroup_list.push_back(&subgroup);
      }
    }
  }
  auto nng = new_group_list.size();

  // 変化があったことを示すフラグ
  bool changed = nng != ng;

  // グループ番号をキーにして後続グループのリストを持つ配列
  std::vector<std::vector<Group*>> succ_list_array(nng);
  // グループ番号をキーにして先行グループのリストを持つ配列
  std::vector<std::vector<Group*>> pred_list_array(nng);
  // グループ間の順序関係を作る．
  for ( auto& group: new_group_list ) {
    auto subgroup = subgroup_list[group->id()];
    std::unordered_set<SizeType> mark;
    dfs(subgroup, group, mark, succ_list_array, pred_list_array);
  }
  // グループのランクを計算する．
  std::vector<SizeType> rank_array(nng);
  set_rank(succ_list_array, pred_list_array, rank_array);
  std::vector<SizeType> id_list(nng);
  for ( SizeType i = 0; i < nng; ++ i ) {
    id_list[i] = i;
  }
  // id_list をランクの昇順にソートする．
  std::sort(id_list.begin(), id_list.end(),
	    [&](SizeType a, SizeType b) -> bool {
	      return rank_array[a] < rank_array[b];
	    });
  // 推移簡約を行う．
  for ( auto id: id_list ) {
    auto& group = new_group_list[id];
    auto succ_list = tr_red(group.get(), succ_list_array, id_list);
    group->set_succ_list(succ_list);
  }

  // 変化があったら更新する．
  if ( check(new_group_list) ) {
    std::swap(mCurGroupList, new_group_list );
    return true;
  }
  return false;
}

// @brief subgroup から到達可能な Group のリストを求める．
DichoCandMgr2::dfs(
  const SubGroup* subgroup,
  Group* from,
  std::unordered_set<SizeType>& mark,
  std::vector<std::vector<Group*>>& succ_list_array,
  std::vector<std::vector<Group*>>& pred_list_array
)
{
  if ( mark.count(subgroup.id) > 0 ) {
    return;
  }
  mark.insert(subgroup.id);
  auto group = subgroup.group;
  if ( group != nullptr ) {
    succ_list_array[from->id()].push_back(group);
    pred_list_array[group->id()].push_back(from);
  }
  else {
    for ( auto subgroup1: subgroup->succ_list ) {
      dfs(subgroup1, from, mark, succ_list_array, pred_list_array);
    }
  }
}

// @brief 推移的簡約を行う．
std::vector<Group*>
DichoCandMgr2::tr_red(
  Group* from,
  const std::vector<std::vector<Group*>>& succ_list_array
)
{
  // from に隣接したグループのうち，直接のリンク以外で経路が
  // 存在するものは推移的なリンク
  std::vector<Group*> ans_list;
  for ( auto to: succ_list_array[from->id()] ) {
    if ( rank_array[to] == ref_rank ) {
      // ランクが1違いなら確実に必要なリンク
      ans_list.push_back(to);
      continue;
    }
  }
  return ans_list;
}

// @brief 終了処理
std::unique_ptr<EqDomCand>
DichoCandMgr2::end(
  bool reduce
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
  std::unordered_map<SizeType, SizeType> id_map;
  auto ng = tmp_list.size();
  for ( SizeType id = 0; id < ng; ++ id ) {
    auto group = tmp_list[id];
    id_map.emplace(group->id(), id);
  }

  // グループ番号を付け直す．
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

// @brief パタンを文字列にする．
std::string
DichoCandMgr2::pat_str(
  PackedVal pat
)
{
  std::ostringstream buf;
  buf << "[" << std::hex << pat << std::dec << "]";
  return buf.str();
}

// @brief パタンのリストを文字列にする．
std::string
DichoCandMgr2::pat_list_str(
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
