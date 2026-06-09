
/// @file DichoCandMgr2.cc
/// @brief DichoCandMgr2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "CandMgr.h"
#include "DichoCandMgr2.h"
#include "DiGroup.h"
#include "DPatGraph.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス DichoCandMgr2
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
DichoCandMgr2::DichoCandMgr2(
  const TpgFaultList& fault_list
) : CandMgr(fault_list)
{
  // 最初は１つのグループ
  auto group = new DiGroup(0, fault_list);
  mCurGroupList.push_back(std::unique_ptr<DiGroup>{group});
  // 自己ループは追加しない．
}

// @brief デストラクタ
DichoCandMgr2::~DichoCandMgr2()
{
}

BEGIN_NONAMESPACE

struct SearchMgr {
  std::vector<std::vector<PackedVal>> dpat_list_array;
  DPatGraph dpat_graph;
  std::vector<DiGroup*> dom_list;

  SearchMgr(
    const std::vector<PackedVal>& dpat_array,
    const std::vector<std::unique_ptr<DiGroup>>& group_list
  ) : dpat_graph(dpat_array),
      dpat_list_array(group_list.size())
  {
    auto ng = group_list.size();
    for ( SizeType id = 0; id < ng; ++ id ) {
      auto& group = group_list[id];
      std::unordered_set<PackedVal> d_hash;
      auto& dpat_list = dpat_list_array[group->id()];
      for ( auto fault: group->fault_list() ) {
	auto dpat = dpat_array[fault.id()];
	if ( d_hash.count(dpat) == 0 ) {
	  d_hash.insert(dpat);
	  dpat_list.push_back(dpat);
	}
      }
      std::sort(dpat_list.begin(), dpat_list.end(), std::greater<>());
    }
  }

  void
  get_dom_subgroup(
    PackedVal dpat,
    DiGroup* group,
    const std::vector<PackedVal>& block_pats
  );

};


void
SearchMgr::get_dom_subgroup(
  PackedVal dpat,
  DiGroup* group,
  const std::vector<PackedVal>& block_pats
)
{
  auto& target_pats = dpat_list_array[group->id()];
  auto pats = dpat_graph.dom_list(dpat, target_pats, block_pats);
  auto block_pats1 = block_pats;
  bool stopped = false;
  for ( auto pat1: pats ) {
    if ( pat1 == dpat ) {
      stopped = true;
    }
    auto subgroup = group->subgroup(pat1);
    dom_list.push_back(subgroup);
    block_pats1.push_back(pat1);
  }
  if ( stopped ) {
    return;
  }
  for ( auto dom_group: group->dominance_list() ) {
    get_dom_subgroup(dpat, dom_group, block_pats1);
  }
}

END_NONAMESPACE

// @brief 更新処理
bool
DichoCandMgr2::update(
  const std::vector<PackedVal>& dpat_array
)
{
  SearchMgr mgr(dpat_array, mCurGroupList);

  // グループの数を数える．
  // 同時にグループごとに現れた dpat のリストを作る．
  SizeType new_group_num = 0;
  for ( auto& src_group: mCurGroupList ) {
    auto& dpat_list = mgr.dpat_list_array[src_group->id()];
    std::unordered_set<PackedVal> d_hash;
    for ( auto fault: src_group->fault_list() ) {
      auto d = dpat_array[fault.id()];
      if ( d_hash.count(d) == 0 ) {
	d_hash.insert(d);
	dpat_list.push_back(d);
	++ new_group_num;
      }
    }
  }

  // 細分化したグループを作る．
  std::vector<std::unique_ptr<DiGroup>> new_group_list;
  new_group_list.reserve(new_group_num);
  auto ng = mCurGroupList.size();
  for ( SizeType id = 0; id < ng; ++ id ) {
    auto& src_group = mCurGroupList[id];
    auto& fault_list = src_group->fault_list();
    std::unordered_map<SizeType, TpgFaultList> fault_list_dict;
    auto& dpat_list = mgr.dpat_list_array[id];
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
  for ( SizeType id = 0; id < ng; ++ id ) {
    auto& src_group = mCurGroupList[id];
    auto& dpat_list = src_group->dpat_list();
    auto& src_dom_list = src_group->dominance_list();
    for ( auto dpat: dpat_list ) {
      auto group = src_group->subgroup(dpat);
      std::vector<DiGroup*> dom_list;
      // まず自己ループに対する細分化
      // dpat_graph 上で直接の後続を求める．
      // ターゲットは src_group から細分化されたサブグループ
      // ブロックはなし．
      std::vector<PackedVal> target_pats;
      target_pats.reserve(mgr.dpat_list_array[id].size() - 1);
      for ( auto dpat1: mgr.dpat_list_array[id] ) {
	if ( dpat1 != dpat ) {
	  target_pats.push_back(dpat1);
	}
      }
      auto imm_succ_pats = mgr.dpat_graph.dom_list(dpat, target_pats, {});
      for ( auto pat1: imm_succ_pats ) {
	auto group1 = src_group->subgroup(pat1);
	dom_list.push_back(group1);
      }

      // src_group の後続に対する細分化
      // 注意が必要なのは細分化した結果，対応するサブグループがない場合には
      // そのさらに後続を調べる必要がある．
      for ( auto src_dom_group: src_dom_list ) {
	mgr.get_dom_subgroup(dpat, src_dom_group, imm_succ_pats);
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

BEGIN_NONAMESPACE

void
transitive_dominance_list(
  DiGroup* group,
  std::unordered_set<SizeType>& mark,
  std::vector<DiGroup*>& ans_list
)
{
  if ( mark.count(group->id()) > 0 ) {
    return;
  }
  mark.emplace(group->id());
  ans_list.push_back(group);
  for ( auto group1: group->dominance_list() ) {
    transitive_dominance_list(group1, mark, ans_list);
  }
}

END_NONAMESPACE

// @brief 終了処理
EqDomCand
DichoCandMgr2::end()
{
  EqDomCand cand;
#if 0
  for ( auto& group: mCurGroupList ) {
    cand.add_eqgroup(group->fault_list());
    std::vector<DiGroup*> dom_group_list;
    std::unordered_set<SizeType> mark;
    transitive_dominance_list(group.get(), mark, dom_group_list);
    for ( auto fault: group->fault_list() ) {
      TpgFaultList dom_list;
      for ( auto dom_group: dom_group_list ) {
	for ( auto fault1: dom_group->fault_list() ) {
	  if ( fault1 != fault ) {
	    dom_list.push_back(fault1);
	  }
	}
      }
      cand.set_domcand(fault, dom_list);
    }
  }
#endif
  return cand;
}

// @brief 変化があったか調べる．
bool
DichoCandMgr2::check(
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
