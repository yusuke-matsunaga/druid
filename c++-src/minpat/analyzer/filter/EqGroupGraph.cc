
/// @file EqGroupGraph.cc
/// @brief EqGroupGraph の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "EqGroupGraph.h"
#include "EqGroupMgr.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス EqGroupGraph
//////////////////////////////////////////////////////////////////////

// @brief EqGroupMgr の現在の内容を取り出すコンストラクタ
EqGroupGraph::EqGroupGraph(
  const EqGroupMgr& mgr
) : mGroupList(mgr.group_num())
{
  auto ng = mgr.group_num();

  // 故障番号をキーにして対応するグループ番号を格納する配列
  std::vector<SizeType> gid_map(mgr.max_fault_size());
  for ( SizeType gid = 0; gid < ng; ++ gid ) {
    for ( auto fault: mgr.fault_list(gid) ) {
      gid_map[fault.id()] = gid;
    }
  }

  // グループを先頭の故障番号の昇順にソートする．
  // 実際には先頭の故障から所属しているグループを調べる．
  // これなら O(N)

  // ソートされたグループ番号のリスト
  std::vector<SizeType> id_list;
  id_list.reserve(ng);

  // もとのグループ番号をキーにしてソートされたグループ番号を格納した配列
  std::vector<SizeType> id_map(ng, ng);
  for ( auto fault: mgr.fault_info().fault_list() ) {
    auto gid = gid_map[fault.id()];
    if ( id_map[gid] < ng ) {
      // 処理済み
      continue;
    }
    auto new_id = id_list.size();
    id_list.push_back(gid);
    id_map[gid] = new_id;
  }

  // 故障グループを作る．
  for ( SizeType gid = 0; gid < ng; ++ gid ) {
    auto orig_id = id_list[gid];
    auto& group = mGroupList[gid];
    group.mFaultList = mgr.fault_list(orig_id);
    auto orig_pred_list = mgr.pred_list(orig_id);
    auto& pred_list = group.mPredList;
    pred_list.reserve(orig_pred_list.size());
    for ( auto orig_id: orig_pred_list ) {
      auto id = id_map[orig_id];
      pred_list.push_back(id);
    }
    std::sort(pred_list.begin(), pred_list.end());
  }
}

// @brief 内容を出力する．
void
EqGroupGraph::print(
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
EqGroupGraph::operator==(
  const EqGroupGraph& right
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
EqGroupGraph::print_diff(
  std::ostream& s,
  const EqGroupGraph& left,
  const EqGroupGraph& right
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
