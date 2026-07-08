
/// @file EqGroupState.cc
/// @brief EqGroupState の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "EqGroupState.h"
#include "EqGroupMgr.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス EqGroupState
//////////////////////////////////////////////////////////////////////

// @brief EqGroupMgr の現在の内容を取り出すコンストラクタ
EqGroupState::EqGroupState(
  const EqGroupMgr& mgr
) : mGroupList(mgr.group_num())
{
  auto ng = mgr.group_num();

  // 故障グループのソート用の構造体
  struct GroupInfo {
    SizeType key;
    SizeType orig_id;
  };

  // 故障グループを最初の故障番号の昇順にソートする．
  // 実際にはグループ番号のマッピングのみを求める．
  std::vector<GroupInfo> tmp_list;
  tmp_list.reserve(ng);
  for ( SizeType gid = 0; gid < ng; ++ gid ) {
    auto fault_list = mgr.fault_list(gid);
    auto fault1 = fault_list[0];
    tmp_list.push_back({fault1.id(), gid});
  }
  std::sort(tmp_list.begin(), tmp_list.end(),
	    [](const GroupInfo& a, const GroupInfo& b) -> bool {
	      return a.key < b.key;
	    });

  // 元のグループ番号からソートされたグループ番号へのマッピングを作る．
  std::vector<SizeType> id_map(ng);
  for ( SizeType gid = 0; gid < ng; ++ gid ) {
    auto orig_id = tmp_list[gid].orig_id;
    id_map[orig_id] = gid;
  }

  // 故障グループを作る．
  for ( SizeType gid = 0; gid < ng; ++ gid ) {
    auto orig_id = tmp_list[gid].orig_id;
    auto& group = mGroupList[orig_id];
    group.mFaultList = mgr.fault_list(gid);
    auto orig_pred_list = mgr.pred_list(orig_id);
    group.mPredList.reserve(orig_pred_list.size());
    for ( auto orig_id: orig_pred_list ) {
      auto id = id_map[orig_id];
      group.mPredList.push_back(id);
    }
  }
}

// @brief 内容を出力する．
void
EqGroupState::print(
  std::ostream& s
) const
{
  auto ng = group_num();
  for ( SizeType gid = 0; gid < ng; ++ gid ) {
    s << "Group#" << gid << ":";
    for ( auto fault: fault_list(gid) ) {
      s << " " << fault.str();
    }
    s << std::endl;
    for ( auto pred_id: pred_list(gid) ) {
      s << "  <-- Group#" << pred_id << std::endl;
    }
  }
}

// @brief 等価比較演算子
bool
EqGroupState::operator==(
  const EqGroupState& right
) const
{
  auto ng = group_num();
  if ( right.group_num() != ng ) {
    return false;
  }
  for ( SizeType gid = 0; gid < ng; ++ gid ) {
    if ( fault_list(gid) != right.fault_list(gid) ) {
      return false;
    }
    if ( pred_list(gid) != right.pred_list(gid) ) {
      return false;
    }
  }
  return true;
}

BEGIN_NONAMESPACE

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

END_NONAMESPACE

// @brief 詳細な比較を行う．
void
EqGroupState::print_diff(
  std::ostream& s,
  const EqGroupState& left,
  const EqGroupState& right
)
{
  auto ng1 = left.group_num();
  auto ng2 = right.group_num();
  std::vector<TpgFaultList> group_list1;
  std::vector<TpgFaultList> group_list2;
  SizeType i1 = 0;
  SizeType i2 = 0;
  while ( i1 < ng1 && i2 < ng2 ) {
    auto group1 = left.fault_list(i1);
    auto group2 = right.fault_list(i2);
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
  for ( ; i1 < ng1; ++ i1 ) {
    group_list1.push_back(left.fault_list(i1));
  }
  for ( ; i2 < ng2; ++ i2 ) {
    group_list2.push_back(right.fault_list(i2));
  }
  bool error = false;
  if ( !group_list1.empty() ) {
    error = true;
    std::cout << "Only in the left" << std::endl;
    for ( auto& group: group_list1 ) {
      for ( auto f: group ) {
	std::cout << " " << f.str();
      }
      std::cout << std::endl;
    }
  }
  if ( !group_list2.empty() ) {
    error = true;
    std::cout << "Only in the right" << std::endl;
    for ( auto& group: group_list2 ) {
      for ( auto f: group ) {
	std::cout << " " << f.str();
      }
      std::cout << std::endl;
    }
  }
  if ( error ) {
    return;
  }

  // この時点で ng1 == ng2 のはず
  for ( SizeType gid = 0; gid < ng1; ++ gid ) {
    auto& pred_list1 = left.pred_list(gid);
    auto& pred_list2 = right.pred_list(gid);
    SizeType n1 = pred_list1.size();
    SizeType n2 = pred_list2.size();
    SizeType i1 = 0;
    SizeType i2 = 0;
    while ( i1 < n1 && i2 < n2 ) {
      auto g1 = pred_list1[i1];
      auto g2 = pred_list2[i2];
      if ( g1 < g2 ) {
	++ i1;
	std::cout << "Only in the left" << std::endl;
	std::cout << "  Group#" << gid << " <-- Group#" << g1 << std::endl;
      }
      else if ( g1 > g2 ) {
	++ i2;
	std::cout << "Only in the right" << std::endl;
	std::cout << "  Group#" << gid << " <-- Group#" << g2 << std::endl;
      }
      else {
	++ i1;
	++ i2;
      }
    }
    for ( ; i1 < n1; ++ i1 ) {
      auto g1 = pred_list1[i1];
      std::cout << "Only in the left" << std::endl;
      std::cout << "  Group#" << gid << " <-- Group#" << g1 << std::endl;
    }
    for ( ; i2 < n2; ++ i2 ) {
      auto g2 = pred_list2[i2];
      std::cout << "Only in the right" << std::endl;
      std::cout << "  Group#" << gid << " <-- Group#" << g2 << std::endl;
    }
  }
}

END_NAMESPACE_DRUID
