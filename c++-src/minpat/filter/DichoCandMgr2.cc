
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

// @brief 更新処理
bool
DichoCandMgr2::update(
  const std::vector<PackedVal>& dpat_array
)
{
  // パタン間の順序関係を表すグラフ
  DPatGraph dpat_graph(dpat_array);

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
    auto succ_list = group->succ_list();
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
      auto pid = dpat_graph.id(dpat);
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

  // グループ間の順序関係を求める．
  auto builder = POSet::Builder();
  for ( auto& group: new_group_list ) {
    auto subgroup = subgroup_list[group->id()];
    std::unordered_set<SizeType> mark;
    dfs(subgroup, group.get(), mark, builder);
  }
  auto poset = POSet(builder);
  // 推移簡約を行った結果をセットする．
  SizeType total_count = 0;
  for ( auto& group: new_group_list ) {
    auto succ_id_list = poset.imm_succ_list(group->id());
    std::vector<Group*> succ_list;
    succ_list.reserve(succ_id_list.size());
    for ( auto id1: succ_id_list ) {
      auto& group1 = new_group_list[id1];
      succ_list.push_back(group1.get());
    }
    total_count += succ_list.size();
    group->set_succ_list(std::move(succ_list));
  }

  // 変化があったら更新する．
  if ( nng != ng ) {
    SizeType total_count0 = 0;
    for ( auto& group: mCurGroupList ) {
      total_count0 += group->succ_list().size();
    }
    if ( total_count != total_count0 ) {
      std::swap(mCurGroupList, new_group_list );
      return true;
    }
  }
  return false;
}

// @brief subgroup から到達可能な Group のリストを求める．
void
DichoCandMgr2::dfs(
  const SubGroup* subgroup,
  Group* from,
  std::unordered_set<SizeType>& mark,
  POSet::Builder& builder
)
{
  if ( mark.count(subgroup->id) > 0 ) {
    return;
  }
  mark.insert(subgroup->id);
  auto group = subgroup->group;
  if ( group != nullptr ) {
    builder.add(from->id(), group->id());
  }
  else {
    for ( auto subgroup1: subgroup->succ_list ) {
      dfs(subgroup1, from, mark, builder);
    }
  }
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
    auto& succ_list = group1->succ_list();
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
    auto succ_list = group->succ_list();
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
