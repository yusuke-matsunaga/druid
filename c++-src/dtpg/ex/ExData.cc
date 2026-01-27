
/// @file ExData.cc
/// @brief ExData の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "ExData.h"
#include "types/TpgNodeList.h"
#include "types/AssignList.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

class Queue {
public:

  void
  put(
    const TpgNode& node
  )
  {
    if ( mMark.count(node.id()) == 0 ) {
      mMark.emplace(node.id());
      mQueue.push_back(node);
    }
  }

  TpgNode
  get()
  {
    auto node = mQueue.front();
    mQueue.pop_front();
    return node;
  }

  bool
  empty() const
  {
    return mQueue.empty();
  }


private:
  std::deque<TpgNode> mQueue;
  std::unordered_set<SizeType> mMark;
};

END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// クラス ExData
//////////////////////////////////////////////////////////////////////

ExData::ExData(
  const TpgNode& root,
  const VidMap& gvar_map,
  const VidMap& fvar_map,
  const SatModel& model
) : mRoot{root},
    mGvarMap{gvar_map},
    mFvarMap{fvar_map},
    mSatModel{model}
{
  // root の TFO (fault cone) に印をつける．
  // 同時に故障差の伝搬している外部出力のリストを作る．
  std::deque<TpgNode> tmp_queue;
  mFconeMark.emplace(root.id());
  tmp_queue.push_back(root);
  while ( !tmp_queue.empty() ) {
    auto node = tmp_queue.front();
    tmp_queue.pop_front();
    if ( node.is_ppo() ) {
      if ( gval(node) != fval(node) ) {
	mSensitizedOutputList.push_back(node);
      }
    }
    for ( auto onode: node.fanout_list() ) {
      if ( mFconeMark.count(onode.id()) == 0 ) {
	mFconeMark.emplace(onode.id());
	tmp_queue.push_back(onode);
      }
    }
  }
}

// @brief ノードの出力に故障の影響を伝搬させる side input を求める．
std::vector<TpgNode>
ExData::backtrace(
  const TpgNode& node,
  std::vector<std::vector<TpgNode>>& cgroup_list
)
{
  std::vector<std::vector<TpgNode>> tmp_cgroup_list;
  auto tmp_node_list = backtrace1(node, tmp_cgroup_list);

  // step1: node_list 中の重複を取り除く

  // IDの昇順にソートする．
  sort(tmp_node_list.begin(), tmp_node_list.end(),
       [](const TpgNode& a,
	  const TpgNode& b) {
	 return a.id() < b.id();
       });
  // 重複を取り除く．
  std::vector<TpgNode> node_list;
  node_list.reserve(tmp_node_list.size());
  TpgNode prev;
  for ( auto& node: tmp_node_list ) {
    if ( node != prev ) {
      node_list.push_back(node);
      prev = node;
    }
  }

  // step2: cgroup_list 中に node_list が現れていたら
  // そのグループを削除する（常に成り立っているので）
  std::unordered_set<SizeType> node_set;
  for ( auto& node: node_list ) {
    node_set.emplace(node.id());
  }
  cgroup_list.clear();
  cgroup_list.reserve(tmp_cgroup_list.size());
  for ( auto& cgroup: tmp_cgroup_list ) {
    for ( auto& node: cgroup ) {
      if ( node_set.count(node.id()) > 0 ) {
	continue;
      }
      // cgroup をソートしておく
      sort(cgroup.begin(), cgroup.end(),
	   [](const TpgNode& a,
	      const TpgNode& b) {
	     return a.id() < b.id();
	   });
      cgroup_list.push_back(cgroup);
    }
  }
  return node_list;
}

// @brief ノードの出力に故障の影響を伝搬させる side input を求める．
std::vector<TpgNode>
ExData::backtrace1(
  const TpgNode& node,
  std::vector<std::vector<TpgNode>>& cgroup_list
)
{
  std::vector<TpgNode> ncnode_list;
  Queue queue;
  queue.put(node);
  while ( !queue.empty() ) {
    auto node = queue.get();
    for ( auto inode: node.fanin_list() ) {
      switch ( type(inode) ) {
      case 1: // inode に故障の影響が伝搬している場合．
	if ( inode != mRoot ) {
	  queue.put(inode);
	}
	break;

      case 2: // inode に故障の影響が伝搬していない場合．
	backtrace2(inode, ncnode_list, cgroup_list);
	break;

      case 3:
	// inode が fcone 外のノードの場合
	// node が XOR/XNOR 以外なら非制御値になっているはず．
	if ( node.gate_type() != PrimType::Xor &&
	     node.gate_type() != PrimType::Xnor ) {
	  ncnode_list.push_back(inode);
	}
      }
    }
  }
  return ncnode_list;
}

// @brief ノードの出力の値を確定させる side input の割り当てを求める．
void
ExData::backtrace2(
  const TpgNode& node,
  std::vector<TpgNode>& ncnode_list,
  std::vector<std::vector<TpgNode>>& cgroup_list
)
{
  Queue queue;
  queue.put(node);
  while ( !queue.empty() ) {
    auto node = queue.get();
    // node の出力を確定させる条件は以下の2通りがある．
    // case A: 制御値を持つ side input がある．
    //         制御値を持つノードのうちどれか1つを選ぶ．
    // case B: 制御値を持つ side input がない．
    //         すべてのファンインの値を確定させる．
    std::vector<TpgNode> cnode_list;
    cnode_list.reserve(node.fanin_num());
    for ( auto inode: node.fanin_list() ) {
      if ( type(inode) == 3 ) {
	if ( gval(inode) == node.cval() ) {
	  cnode_list.push_back(inode);
	}
      }
    }
    if ( cnode_list.empty() ) {
      // すべてのファンインの値を確定させる．
      for ( auto inode: node.fanin_list() ) {
	if ( type(inode) == 3 ) {
	  ncnode_list.push_back(inode);
	}
	else {
	  queue.put(inode);
	}
      }
    }
    else {
      // 制御値を持つノードを選ぶ．
      if ( cnode_list.size() == 1 ) {
	// 選択の余地なくこのノードを選ぶ．
	auto& cnode = cnode_list.front();
	ncnode_list.push_back(cnode);
      }
      else {
	std::vector<TpgNode> cgroup;
	for ( auto& cnode: cnode_list ) {
	  cgroup.push_back(cnode);
	}
	cgroup_list.push_back(cgroup);
      }
    }
  }
}

END_NAMESPACE_DRUID
