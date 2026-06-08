
/// @file CandMgr.cc
/// @brief CandMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "CandMgr.h"
#include "NaiveCandMgr.h"
#include "DichoCandMgr.h"
#include "DichoCandMgr2.h"
#include "DiGroup.h"
#include "DPatGraph.h"
#include "DomGraph.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス CandMgr
//////////////////////////////////////////////////////////////////////

// @brief 新しいオブジェクトを作る．
std::unique_ptr<CandMgr>
CandMgr::new_obj(
  const TpgFaultList& fault_list,
  const ConfigParam& option
)
{
  auto str = option.get_string_elem("method", "naive");
  CandMgr* mgr = nullptr;
  if ( str == "naive" ) {
    mgr = new NaiveCandMgr(fault_list);
  }
  else if ( str == "dichotomy" ) {
    mgr = new DichoCandMgr(fault_list);
  }
  else if ( str == "dichotomy2" ) {
    mgr = new DichoCandMgr2(fault_list);
  }
  else {
    std::ostringstream buf;
    buf << str << ": unknown option for 'method'";
    throw std::invalid_argument{buf.str()};
  }
  return std::unique_ptr<CandMgr>{mgr};
}


//////////////////////////////////////////////////////////////////////
// クラス NaiveCandMgr
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
NaiveCandMgr::NaiveCandMgr(
  const TpgFaultList& fault_list
) : CandMgr(fault_list),
    mSize{max_fault_size()},
    mArray(mSize * mSize, false),
    mDomCandListArray(mSize)
{
}

// @brief デストラクタ
NaiveCandMgr::~NaiveCandMgr()
{
}

// @brief 更新処理
bool
NaiveCandMgr::update(
  const std::vector<PackedVal>& dpat_array
)
{
  if ( mInitialized ) {
    bool change = false;
    for ( auto fault1: fault_list() ) {
      auto pat1 = dpat_array[fault1.id()];
      auto& old_list = mDomCandListArray[fault1.id()];
      TpgFaultList new_list;
      new_list.reserve(old_list.size());
      for ( auto fault2: old_list ) {
	auto pat2 = dpat_array[fault2.id()];
	if ( (pat1 & ~pat2) != PV_ALL0 ) {
	  auto idx1 = _index(fault1, fault2);
	  mArray[idx1] = true;
	}
	else {
	  new_list.push_back(fault2);
	}
      }
      if ( new_list.size() != old_list.size() ) {
	std::swap(new_list, old_list);
	change = true;
      }
    }
    return change;
  }
  else {
    for ( auto fault1: fault_list() ) {
      auto pat1 = dpat_array[fault1.id()];
      auto& new_list = mDomCandListArray[fault1.id()];
      for ( auto fault2: fault_list() ) {
	if ( fault2 == fault1 ) {
	  continue;
	}
	auto pat2 = dpat_array[fault2.id()];
	if ( (pat1 & ~pat2) != PV_ALL0 ) {
	  auto idx1 = _index(fault1, fault2);
	  mArray[idx1] = true;
	}
	else {
	  new_list.push_back(fault2);
	}
      }
    }
    mInitialized = true;
    return true;
  }
}

// @brief 終了処理
EqDomCand
NaiveCandMgr::end()
{
  EqDomCand cand;
  cand.init(fault_list());
  {
    std::vector<bool> mark(mSize, false);
    for ( auto fault: fault_list() ) {
      if ( mark[fault.id()] ) {
	continue;
      }
      auto eq_list = eqcand_list(fault);
      eq_list.push_back(fault);
      eq_list.sort();
      cand.add_eqgroup(eq_list);
      for ( auto fault1: eq_list ) {
	mark[fault1.id()] = true;
      }
    }
  }
  for ( auto fault: fault_list() ) {
    auto dom_list = mDomCandListArray[fault.id()];
    cand.set_domcand(fault, dom_list);
  }
  cand.sort();
  return cand;
}

// @brief 等価な可能性のある故障のリストを返す．
TpgFaultList
NaiveCandMgr::eqcand_list(
  const TpgFault& fault
) const
{
  TpgFaultList ans_list;
  for ( auto fault2: mDomCandListArray[fault.id()] ) {
    auto idx2 = _index(fault2, fault);
    if ( !mArray[idx2] ) {
      ans_list.push_back(fault2);
    }
  }
  return ans_list;
}


//////////////////////////////////////////////////////////////////////
// クラス DichoCandMgr
//////////////////////////////////////////////////////////////////////

BEGIN_NONAMESPACE

// 分割されたグループの情報
struct SubGroup {
  PackedVal d_pat; // 検出パタン
  DiGroup* group;  // 分割されたグループ
};

struct SubGroupInfo {
  std::vector<SubGroup> sglist;
};

END_NONAMESPACE

// @brief コンストラクタ
DichoCandMgr::DichoCandMgr(
  const TpgFaultList& fault_list
) : CandMgr(fault_list)
{
  // 最初は１つのグループ
  auto group = new DiGroup(0, fault_list);
  group->set_dominance_list({group});
  mCurGroupList.push_back(std::unique_ptr<DiGroup>{group});
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
  // 同時に現れた d_pat のリストを作る．
  auto ng = mCurGroupList.size();
  std::vector<std::vector<PackedVal>> d_list_array(ng);
  SizeType new_group_num = 0;
  for ( auto& src_group: mCurGroupList ) {
    std::unordered_set<PackedVal> d_hash;
    auto& d_list = d_list_array[src_group->id()];
    for ( auto fault: src_group->fault_list() ) {
      auto d = dpat_array[fault.id()];
      if ( d_hash.count(d) == 0 ) {
	d_hash.insert(d);
	d_list.push_back(d);
	++ new_group_num;
      }
    }
    std::sort(d_list.begin(), d_list.end(), std::greater<>());
    src_group->init();
  }

  // 細分化したグループを作る．
  std::vector<std::unique_ptr<DiGroup>> new_group_list;
  new_group_list.reserve(new_group_num);
  for ( SizeType id = 0; id < ng; ++ id ) {
    auto& src_group = mCurGroupList[id];
    auto& fault_list = src_group->fault_list();
    std::unordered_map<SizeType, TpgFaultList> fault_list_dict;
    auto& d_list = d_list_array[id];
    for ( auto d: d_list ) {
      fault_list_dict.emplace(d, TpgFaultList());
    }
    for ( auto fault: fault_list ) {
      auto d = dpat_array[fault.id()];
      fault_list_dict.at(d).push_back(fault);
    }
    for ( auto d: d_list ) {
      auto& fault_list = fault_list_dict.at(d);
      if ( fault_list.empty() ) {
	continue;
      }
      auto id = new_group_list.size();
      auto new_group = new DiGroup(id, fault_list);
      new_group_list.push_back(std::unique_ptr<DiGroup>{new_group});
      src_group->add_subgroup(d, new_group);
    }
  }

  // dominance list を作る．
  for ( SizeType id = 0; id < ng; ++ id ) {
    auto& src_group = mCurGroupList[id];
    std::vector<DiGroup*> dom_list;
    auto& src_dom_list = src_group->dominance_list();
    auto& dpat_list = src_group->dpat_list();
    for ( auto dpat: dpat_list ) {
      auto group = src_group->subgroup(dpat);
      for ( auto src_dom_group: src_dom_list ) {
	auto& dpat1_list = src_dom_group->dpat_list();
	for ( auto dpat1: dpat1_list ) {
	  if ( (dpat & dpat1) == dpat ) {
	    auto src_dom_subgroup = src_dom_group->subgroup(dpat1);
	    dom_list.push_back(src_dom_subgroup);
	  }
	}
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

// @brief 終了処理
EqDomCand
DichoCandMgr::end()
{
  DomGraph dg(*this);
  EqDomCand cand;
  cand.init(fault_list());

  for ( auto& group: mCurGroupList ) {
    cand.add_eqgroup(group->fault_list());
    auto rank = dg.rank(group->id());
    for ( auto fault: group->fault_list() ) {
      TpgFaultList dom_list;
      for ( auto dom_group: group->dominance_list() ) {
	auto rank1 = dg.rank(dom_group->id());
	if ( rank1 == rank + 1 ) {
	  for ( auto fault1: dom_group->fault_list() ) {
	    if ( fault1 != fault ) {
	      dom_list.push_back(fault1);
	    }
	  }
	}
      }
      cand.set_domcand(fault, dom_list);
    }
  }
  cand.sort();
  return cand;
}

// @brief 変化があったか調べる．
bool
DichoCandMgr::check(
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
#if 0
std::vector<DiGroup*>
get_dom_subgroup(
  PackedVal dpat0,
  const std::vector<PackedVal>& block_pats,
  DiGroup* subgroup,
  const DPatGraph& dpat_graph
)
{
  auto pats = dpat_graph.dom_list(dpat0, target_pats, block_pats);
  if ( !pats.empty() ) {
    auto& sginfo1 = sginfo_array[src_dom_group->id()];
    std::vector<DiGroup*> ans_list;
    ans_list.reserve(pats.size());
    for ( auto pat1: pats ) {
      auto src_dom_subgroup = sginfo1.at(pat1);
      ans_list.push_back(src_dom_subgroup);
    }
    return ans_list;
  }
}
#endif
END_NONAMESPACE

// @brief 更新処理
bool
DichoCandMgr2::update(
  const std::vector<PackedVal>& dpat_array
)
{
  using SubGroupInfo = std::unordered_map<PackedVal, DiGroup*>;

  // dapt_array に含まれるパタンの包含関係を表すグラフを作る．
  DPatGraph dpat_graph(dpat_array);

  // グループの数を数える．
  // 同時にグループごとに現れた d_pat のリストを作る．
  auto ng = mCurGroupList.size();
  std::vector<std::vector<PackedVal>> dpat_list_array(ng);
  SizeType new_group_num = 0;
  for ( auto& src_group: mCurGroupList ) {
    auto& dpat_list = dpat_list_array[src_group->id()];
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
    //auto& sginfo = sginfo_array[id];
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
      }
#if 0
      // src_group の後続に対する細分化
      // 注意が必要なのは細分化した結果，対応するサブグループがない場合には
      // そのさらに後続を調べる必要がある．
      get_dom_subgroup(dpat, src_dom_list, imm_succ_pats, dom_list);
      for ( auto src_dom_group: src_dom_list ) {
	for ( ; ; ) {
	  auto pats = dpat_graph.dom_list(dpat, target_pats, imm_succ_pats);
	  if ( pats.empty() ) {
	  }
	  else {
	    auto& sginfo1 = sginfo_array[src_dom_group->id()];
	    for ( auto pat1: pats ) {
	      auto src_dom_subgroup = sginfo1.at(pat1);
	      dom_list.push_back(src_dom_subgroup);
	    }
	    break;
	  }
	}
      }
#endif
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
  cand.init(fault_list());

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
  cand.sort();
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
