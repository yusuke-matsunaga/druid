
/// @file EqDomCand.cc
/// @brief EqDomCand の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "EqDomCand.h"
#include "POSet.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス EqDomCand
//////////////////////////////////////////////////////////////////////

// @brief 内容を指定したコンストラクタ
EqDomCand::EqDomCand(
  const std::vector<TpgFaultList>& group_list,
  const DomPairList& dom_list,
  bool prune
) : mGroupList{group_list},
    mDomListArray(mGroupList.size())
{
  auto ng = mGroupList.size();
  for ( SizeType id = 0; id < ng; ++ id ) {
    auto& fault_list = mGroupList[id];
    fault_list.sort();
    for ( auto fault: fault_list ) {
      mIdMap.emplace(fault.id(), id);
    }
  }
  if ( prune ) {
    POSet::Builder builder;
    for ( auto& p: dom_list ) {
      auto id1 = p.first;
      if ( id1 >= ng ) {
	throw std::out_of_range{"id1 is out of range"};
      }
      auto id2 = p.second;
      if ( id2 >= ng ) {
	throw std::out_of_range{"id2 is out of range"};
      }
      builder.add(id1, id2);
    }
    POSet poset(builder);
    for ( SizeType id1 = 0; id1 < ng; ++ id1 ) {
      auto id2_list = poset.imm_succ_list(id1);
      for ( auto id2: id2_list ) {
	mDomListArray[id1].push_back(id2);
      }
    }
  }
  else {
    for ( auto& p: dom_list ) {
      auto id1 = p.first;
      if ( id1 >= ng ) {
	throw std::out_of_range{"id1 is out of range"};
      }
      auto id2 = p.second;
      if ( id2 >= ng ) {
	throw std::out_of_range{"id2 is out of range"};
      }
      mDomListArray[id1].push_back(id2);
    }
  }
  for ( SizeType id = 0; id < ng; ++ id ) {
    auto& dom_list = mDomListArray[id];
    std::sort(dom_list.begin(), dom_list.end());
  }
}

// @brief 支配故障の候補数を返す．
SizeType
EqDomCand::total_num() const
{
  SizeType count = 0;
  for ( auto& tmp: mDomListArray ) {
    count += tmp.size();
  }
  return count;
}

// @brief 内容を出力する．
void
EqDomCand::print(
  std::ostream& s
) const
{
  for ( SizeType id = 0; id < mGroupList.size(); ++ id ) {
    auto& group = mGroupList[id];
    s << "Group#" << id << ":";
    for ( auto fault: group ) {
      s << " " << fault.str();
    }
    s << std::endl
      << "  ";
    auto& dom_list = mDomListArray[id];
    const char* comma = "";
    for ( auto id1: dom_list ) {
      s << comma << "Group#" << id1;
      comma = ", ";
    }
    s << std::endl;
  }
}

END_NAMESPACE_DRUID
