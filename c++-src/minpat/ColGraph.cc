
/// @file ColGraph.cc
/// @brief ColGraph の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "ColGraph.h"
#include "TpgNetwork.h"
#include "TpgNode.h"
#include "TpgFault.h"
#include "TestCover.h"
#include "ym/Range.h"
#include "ym/Timer.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
ColGraph::ColGraph(
  const TpgNetwork& network,
  const vector<TestCover>& cover_list,
  const JsonValue& option
) : mNetwork{network},
    mBaseEnc{network, option},
    mCubeListArray(network.max_fault_id())
{
  {
    auto& node_list = network.node_list();
    mBaseEnc.make_cnf(node_list, node_list);
  }
  SizeType node_num = 0;
  for ( auto& cover: cover_list ) {
    node_num += cover.cube_list().size();
  }
  mNodeList.reserve(node_num);
  mFaultNum = cover_list.size();
  for ( auto& cover: cover_list ) {
    auto fault = cover.fault();
    auto fid = fault->id();
    for ( auto& cube: cover.cube_list() ) {
      auto id = mNodeList.size();
      mNodeList.push_back({fault, cube, 0});
      mCubeListArray[fid].push_back(id);
    }
  }

  Timer timer;
  timer.start();
  cout << "building blocking matrix" << endl;
  for ( SizeType id1 = 0; id1 < node_num - 1; ++ id1 ) {
    auto& cube1 = cube(id1);
    for ( SizeType id2 = id1 + 1; id2 < node_num; ++ id2 ) {
      auto& cube2 = cube(id2);
      if ( is_conflict(cube1, cube2) ) {
	mNodeList[id1].mConflictList.push_back(id2);
	mNodeList[id2].mConflictList.push_back(id1);
      }
    }
  }
  for ( SizeType id1 = 0; id1 < node_num; ++ id1 ) {
    auto& list = mNodeList[id1].mConflictList;
    sort(list.begin(), list.end());
  }
  timer.stop();
  cout << "end" << endl
       << "CPU Time: " << timer.get_time() << endl;
}

// @brief デストラクタ
ColGraph::~ColGraph()
{
}

// @brief ノードの saturation degree を返す．
SizeType
ColGraph::saturation_degree(
  SizeType id
)
{
  SizeType sat = 0;
  vector<bool> color_set(color_num() + 1, false);
  for ( auto col1: mNodeList[id].mConflictColList ) {
    if ( !color_set[col1] ) {
      color_set[col1] = true;
      ++ sat;
    }
  }
  // 残りの色も衝突している可能性がある．
  for ( SizeType col = 1; col <= color_num(); ++ col ) {
    if ( color_set[col] ) {
      // 既に考慮済み
      continue;
    }
    auto& cube1 = cube(id);
    auto assumptions = mBaseEnc.conv_to_literal_list(cube1);
    auto assign1 = mGroupList[col - 1].mAssignments;
    auto assumptions1 = mBaseEnc.conv_to_literal_list(assign1);
    assumptions.insert(assumptions.end(),
		       assumptions1.begin(),
		       assumptions1.end());
    if ( mBaseEnc.solver().solve(assumptions) == SatBool3::False ) {
      mNodeList[id].mConflictColList.push_back(col);
      ++ sat;
    }
  }
  return sat;
}

// @brief ノードの adjacent degree を返す．
SizeType
ColGraph::adjacent_degree(
  SizeType id
)
{
  SizeType adj = 0;
  vector<bool> fault_set(mNetwork.max_fault_id(), false);
  for ( auto id1: mNodeList[id].mConflictList ) {
    if ( color(id1) > 0 ) {
      // 彩色済みのノードはスキップ
      continue;
    }
    auto fid = fault(id1)->id();
    if ( !fault_set[fid] ) {
      fault_set[fid] = true;
      ++ adj;
    }
  }
  return adj;
}

// @brief ノードを色をつける．
void
ColGraph::set_color(
  SizeType id,
  SizeType color
)
{
  ASSERT_COND( 0 <= id && id < node_num() );
  ASSERT_COND( 1 <= color && color <= color_num() );

  auto& group = mGroupList[color - 1];
  group.mNodeList.push_back(id);
  group.mAssignments.merge(cube(id));

  auto fid = fault(id)->id();
  for ( auto id: mCubeListArray[fid] ) {
    mNodeList[id].mColor = color;
  }
}

// @brief color_map を作る．
SizeType
ColGraph::get_color_map(
  vector<SizeType>& color_map
) const
{
  color_map.clear();
  SizeType n = node_num();
  color_map.resize(n);
  for ( SizeType i = 0; i < n; ++ i ) {
    auto& node = mNodeList[i];
    color_map[i] = node.mColor;
  }
  return color_num();
}

// @brief cube1 と cube2 が衝突する時 true を返す．
bool
ColGraph::is_conflict(
  const NodeTimeValList& assign1,
  const NodeTimeValList& assign2
)
{
  if ( compare(assign1, assign2) == -1 ) {
    // 割り当てが矛盾している．
    return true;
  }
  auto assumptions = mBaseEnc.conv_to_literal_list(assign1);
  auto assumptions2 = mBaseEnc.conv_to_literal_list(assign2);
  assumptions.insert(assumptions.end(),
		     assumptions2.begin(),
		     assumptions2.end());
  return mBaseEnc.solver().solve(assumptions) == SatBool3::False;
}

END_NAMESPACE_DRUID
