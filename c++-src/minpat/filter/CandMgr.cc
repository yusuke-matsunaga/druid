
/// @file CandMgr.cc
/// @brief CandMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "CandMgr.h"
#include "NaiveCandMgr.h"
#include "DichoCandMgr.h"
#include "DiGroup.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス CandMgr
//////////////////////////////////////////////////////////////////////

// @brief 新しいオブジェクトを作る．
std::unique_ptr<CandMgr>
CandMgr::new_obj(
  const TpgFaultList& fault_list,
  const std::string& opt
)
{
  CandMgr* mgr = nullptr;
  if ( opt == "naive" ) {
    mgr = new NaiveCandMgr(fault_list);
  }
  else if ( opt == "dichotomy" ) {
    mgr = new DichoCandMgr(fault_list);
  }
  else {
    std::ostringstream buf;
    buf << opt << ": unknown option";
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
  }

  // 細分化したグループを作る．
  std::vector<std::unique_ptr<DiGroup>> new_group_list;
  new_group_list.reserve(new_group_num);
  std::vector<SubGroupInfo> sginfo_array(ng);
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
    auto& sginfo = sginfo_array[id];
    for ( auto d: d_list ) {
      auto& fault_list = fault_list_dict.at(d);
      if ( fault_list.empty() ) {
	continue;
      }
      auto id = new_group_list.size();
      auto new_group = new DiGroup(id, fault_list);
      new_group_list.push_back(std::unique_ptr<DiGroup>{new_group});
      sginfo.sglist.push_back({d, new_group});
    }
  }

  // dominance list を作る．
  for ( SizeType id = 0; id < ng; ++ id ) {
    auto& src_group = mCurGroupList[id];
    auto& sginfo = sginfo_array[id];
    std::vector<DiGroup*> dom_list;
    auto& src_dom_list = src_group->dominance_list();
    for ( auto& sg: sginfo.sglist ) {
      auto d = sg.d_pat;
      auto group = sg.group;
      for ( auto src_dom_group: src_dom_list ) {
	auto& src_dom_sginfo = sginfo_array[src_dom_group->id()];
	for ( auto& sg1: src_dom_sginfo.sglist ) {
	  auto d1 = sg1.d_pat;
	  if ( (d & d1) == d ) {
	    auto src_dom_subgroup = sg1.group;
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
  EqDomCand cand;
  cand.init(fault_list());

  for ( auto& group: mCurGroupList ) {
    cand.add_eqgroup(group->fault_list());
    for ( auto fault: group->fault_list() ) {
      TpgFaultList dom_list;
      for ( auto dom_group: group->dominance_list() ) {
	for ( auto fault1: dom_group->fault_list() ) {
	  if ( fault1 != fault ) {
	    dom_list.push_back(fault1);
	  }
	}
      }
      cand.set_domcand(fault, dom_list);
    }
  }
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

END_NAMESPACE_DRUID
