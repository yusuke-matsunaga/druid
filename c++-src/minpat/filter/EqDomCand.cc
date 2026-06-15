
/// @file EqDomCand.cc
/// @brief EqDomCand の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "EqDomCand.h"
#include "POSet.h"

#define DEBUG 0


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
#if DEBUG
  {
    auto ng = mGroupList.size();
    for ( SizeType id = 0; id < ng; ++ id ) {
      std::cout << "Group#" << id << ":";
      for ( auto f: group_list[id] ) {
	std::cout << " " << f.str();
      }
      std::cout << std::endl;
    }
    for ( auto& p: dom_list ) {
      auto g1 = group_list[p.first];
      auto g2 = group_list[p.second];
      std::cout << g1[0].str() << " -> " << g2[0].str()
		<< " (" << p.first << ", " << p.second << ")" << std::endl;
    }
  }
#endif
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
    {
      std::cout << builder.size() << " -> " << total_num() << std::endl;
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
    const char* spc = "";
    for ( auto fault: group ) {
      s << spc << fault.str();
      spc = " ";
    }
    s << std::endl
      << "  ==> ";
    auto& dom_list = mDomListArray[id];
    const char* comma = "";
    for ( auto id1: dom_list ) {
      auto& group1 = mGroupList[id1];
      s << comma << group1[0].str();
      comma = ", ";
    }
    s << std::endl;
  }
}

inline
int
compare(
  const TpgFaultList& group1,
  const TpgFaultList& group2
)
{
  auto n1 = group1.size();
  auto n2 = group2.size();
  SizeType i1 = 0;
  SizeType i2 = 0;
  while ( i1 < n1 && i2 < n2 ) {
    auto f1 = group1[i1];
    auto f2 = group2[i2];
    if ( f1.id() < f2.id() ) {
      return -1;
    }
    if ( f1.id() > f2.id() ) {
      return 1;
    }
    ++ i1;
    ++ i2;
  }
  if ( i1 < n1 ) {
    return 1;
  }
  if ( i2 < n2 ) {
    return -1;
  }
  return 0;
}

void
EqDomCand::check(
  const EqDomCand& right
) const
{
  if ( mGroupList != right.mGroupList ) {
    std::cout << "mGroupList mismatch" << std::endl;
    std::vector<TpgFaultList> group_list1;
    std::vector<TpgFaultList> group_list2;
    SizeType i1 = 0;
    SizeType i2 = 0;
    while ( i1 < mGroupList.size() && i2 < right.mGroupList.size() ) {
      auto group1 = mGroupList[i1];
      auto group2 = right.mGroupList[i2];
      auto r = compare(group1, group2);
      if ( r == 0 ) {
	++ i1;
	++ i2;
      }
      else if ( r < 0 ) {
	++ i1;
	group_list1.push_back(group1);
      }
      else {
	++ i2;
	group_list2.push_back(group2);
      }
    }
    for ( ; i1 < mGroupList.size(); ++ i1 ) {
      group_list1.push_back(mGroupList[i1]);
    }
    for ( ; i2 < right.mGroupList.size(); ++ i2 ) {
      group_list2.push_back(right.mGroupList[i2]);
    }
    std::cout << "Only in the left" << std::endl;
    for ( auto& group: group_list1 ) {
      for ( auto f: group ) {
	std::cout << " " << f.str();
      }
      std::cout << std::endl;
    }
    std::cout << "Only in the right" << std::endl;
    for ( auto& group: group_list2 ) {
      for ( auto f: group ) {
	std::cout << " " << f.str();
      }
      std::cout << std::endl;
    }
    return;
  }
  auto ng = mGroupList.size();
  for ( SizeType g = 0; g < ng; ++ g ) {
    auto& dom_list1 = mDomListArray[g];
    auto& dom_list2 = right.mDomListArray[g];
    SizeType n1 = dom_list1.size();
    SizeType n2 = dom_list2.size();
    SizeType i1 = 0;
    SizeType i2 = 0;
    while ( i1 < n1 && i2 < n2 ) {
      auto g1 = dom_list1[i1];
      auto g2 = dom_list2[i2];
      if ( g1 < g2 ) {
	++ i1;
	auto f1 = mGroupList[g][0];
	auto f2 = mGroupList[g1][0];
	std::cout << "Only in the left" << std::endl;
	std::cout << f1.str() << " ==> " << f2.str() << std::endl;
      }
      else if ( g1 > g2 ) {
	++ i2;
	auto f1 = mGroupList[g][0];
	auto f2 = mGroupList[g2][0];
	std::cout << "Only in the right" << std::endl;
	std::cout << f1.str() << " ==> " << f2.str() << std::endl;
      }
      else {
	++ i1;
	++ i2;
      }
    }
    for ( ; i1 < n1; ++ i1 ) {
      auto g1 = dom_list1[i1];
      auto f1 = mGroupList[g][0];
      auto f2 = mGroupList[g1][0];
      std::cout << "Only in the left" << std::endl;
      std::cout << f1.str() << " ==> " << f2.str() << std::endl;
    }
    for ( ; i2 < n2; ++ i2 ) {
      auto g2 = dom_list2[i2];
      auto f1 = mGroupList[g][0];
      auto f2 = mGroupList[g2][0];
      std::cout << "Only in the right" << std::endl;
      std::cout << f1.str() << " ==> " << f2.str() << std::endl;
    }
  }
}

END_NAMESPACE_DRUID
