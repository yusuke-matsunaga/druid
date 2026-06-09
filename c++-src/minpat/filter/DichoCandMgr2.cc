
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

void
get_dom_subgroup(
  PackedVal dpat,
  DiGroup* group,
  std::vector<DiGroup*>& dom_list
)
{
  if ( group->has_subgroup(dpat) ) {
    auto subgroup = group->subgroup(dpat);
    dom_list.push_back(subgroup);
    return;
  }
  // 後続に再帰する．
  for ( auto dom_group: group->dominance_list() ) {
    get_dom_subgroup(dpat, dom_group, dom_list);
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

  auto ng = mCurGroupList.size();
  // グループごとに現れる dpat のリスト
  std::vector<std::vector<PackedVal>> dpat_list_array(ng);
  // グループの数を数える．
  // 同時にグループごとに現れた dpat のリストを作る．
  SizeType new_group_num = 0;
  for ( SizeType id = 0; id < ng; ++ id ) {
    auto& group = mCurGroupList[id];
    std::unordered_set<PackedVal> d_hash;
    auto& dpat_list = dpat_list_array[id];
    for ( auto fault: group->fault_list() ) {
      auto dpat = dpat_array[fault.id()];
      if ( d_hash.count(dpat) == 0 ) {
	d_hash.insert(dpat);
	dpat_list.push_back(dpat);
	++ new_group_num;
      }
    }
    // 深い意味はないけどデバッグ時に見やすいように．
    std::sort(dpat_list.begin(), dpat_list.end());
  }
  // パタンのリスト
  std::vector<PackedVal> all_dpat_list;
  {
    std::unordered_set<PackedVal> dpat_hash;
    for ( auto& dpat_list: dpat_list_array ) {
      for ( auto dpat: dpat_list ) {
	if ( dpat_hash.count(dpat) == 0 ) {
	  dpat_hash.insert(dpat);
	  all_dpat_list.push_back(dpat);
	}
      }
    }
  }

  // 細分化したグループを作る．
  std::vector<std::unique_ptr<DiGroup>> new_group_list;
  new_group_list.reserve(new_group_num);
  for ( SizeType id = 0; id < ng; ++ id ) {
    auto& src_group = mCurGroupList[id];
    auto& fault_list = src_group->fault_list();
    std::unordered_map<SizeType, TpgFaultList> fault_list_dict;
    auto& dpat_list = dpat_list_array[id];
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
      {
	std::cout << std::hex << dpat << std::dec
		  << ": Group#" << new_group->id() << std::endl;
      }
    }
  }

  // dominance list を作る．
  for ( SizeType id = 0; id < ng; ++ id ) {
    auto& src_group = mCurGroupList[id];
    auto& dpat_list = src_group->dpat_list();
    auto& src_dom_list = src_group->dominance_list();
    for ( auto dpat: dpat_list ) {
      auto group = src_group->subgroup(dpat);
      {
	std::cout << "Group#" << group->id() << std::endl;
      }
      std::vector<DiGroup*> dom_list;
      // まず自己ループに対する細分化
      // dpat_graph 上で直接の後続を求める．
      // ターゲットは src_group から細分化されたサブグループ
      // ブロックはなし．
      std::vector<PackedVal> target_pats;
      target_pats.reserve(dpat_list_array[id].size() - 1);
      for ( auto dpat1: dpat_list_array[id] ) {
	if ( dpat1 != dpat ) {
	  target_pats.push_back(dpat1);
	}
      }
      auto imm_succ_pats = dpat_graph.dom_list(dpat, target_pats, {});
      for ( auto pat1: imm_succ_pats ) {
	auto group1 = src_group->subgroup(pat1);
	dom_list.push_back(group1);
	{
	  std::cout << " [1]-> " << std::hex << pat1 << std::dec
		    << " Group#" << group1->id() << std::endl;
	}
      }

      // src_group の後続に対する細分化
      // 注意が必要なのは細分化した結果，対応するサブグループがない場合には
      // そのさらに後続を調べる必要がある．
      for ( auto src_dom_group: src_dom_list ) {
	get_dom_subgroup(dpat, src_dom_group, dom_list);
      }
      // dpat と imm_succ_pats の間にパタンがある場合，
      // それらのパタンに対してもサブグループを求める必要がある．
      std::vector<PackedVal> target_pats2;
      target_pats2.reserve(all_dpat_list.size() - 1);
      for ( auto pat: all_dpat_list ) {
	if ( pat != dpat ) {
	  target_pats2.push_back(pat);
	}
      }
      auto extra_pats = dpat_graph.dom_list(dpat, target_pats2, imm_succ_pats);
      for ( auto dpat1: extra_pats ) {
	// この dpat1 に関連した src_group のサブグループは存在しない．
	for ( auto src_dom_group: src_dom_list ) {
	  get_dom_subgroup(dpat1, src_dom_group, dom_list);
	}
      }
      group->set_dominance_list(std::move(dom_list));
    }
  }

  // 変化があったら更新する．
  if ( check(new_group_list) ) {
    std::swap(mCurGroupList, new_group_list);

    print(std::cout);
    return true;
  }

  return false;
}

// @brief 終了処理
EqDomCand
DichoCandMgr2::end()
{
  // fault_list() の先頭の故障番号の昇順でソートする．
  std::sort(mCurGroupList.begin(), mCurGroupList.end(),
	    [](const std::unique_ptr<DiGroup>& a,
	       const std::unique_ptr<DiGroup>& b) -> bool {
	      auto f1 = a->fault_list()[0];
	      auto f2 = b->fault_list()[0];
	      return f1.id() < f2.id();
	    });
  // グループ番号を付け直す．
  auto ng = group_num();
  std::vector<TpgFaultList> group_list;
  group_list.reserve(ng);
  std::unordered_map<SizeType, SizeType> id_map;
  for ( SizeType id = 0; id < ng; ++ id ) {
    auto& group = mCurGroupList[id];
    id_map.emplace(group->id(), id);
    group_list.push_back(group->fault_list());
  }
  std::vector<std::pair<SizeType, SizeType>> dom_list;
  for ( SizeType id1 = 0; id1 < ng; ++ id1 ) {
    auto group1 = mCurGroupList[id1].get();
    auto& domgroup_list = group1->dominance_list();
    for ( auto dom_group: domgroup_list ) {
      auto id2 = id_map.at(dom_group->id());
      if ( id2 != id1 ) {
	dom_list.push_back({id1, id2});
      }
    }
  }
  return EqDomCand(group_list, dom_list);
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

// @brief 内容を出力する．
void
DichoCandMgr2::print(
  std::ostream& s
) const
{
  s << "-------------------------------------------" << std::endl;
  for ( auto& group: mCurGroupList ) {
    s << "Group#" << group->id() << ":";
    for ( auto fault: group->fault_list() ) {
      s << " " << fault.str();
    }
    s << std::endl
      << "  ";
    auto& dom_list = group->dominance_list();
    const char* comma = "";
    for ( auto group1: dom_list ) {
      s << comma << "Group#" << group1->id();
      comma = ", ";
    }
    s << std::endl;
  }
  s << "-------------------------------------------" << std::endl;
}

END_NAMESPACE_DRUID
