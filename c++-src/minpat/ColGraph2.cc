
/// @file ColGraph2.cc
/// @brief ColGraph2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "ColGraph2.h"
#include "TpgNetwork.h"
#include "TpgNode.h"
#include "TpgFault.h"
#include "TestCube.h"
#include "ym/Range.h"
#include "ym/Timer.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
ColGraph2::ColGraph2(
  const TpgNetwork& network,
  const vector<TestCube>& cube_list,
  const JsonValue& option
) : mNetwork{network},
    mBaseEnc{network, option}
{
  {
    auto& node_list = network.node_list();
    mBaseEnc.make_cnf(node_list, node_list);
  }

  vector<vector<SizeType>> cube_list_array(mNetwork.max_fault_id());
  vector<SizeType> fault_list;
  SizeType cube_num = cube_list.size();
  for ( SizeType id = 0; id < cube_num; ++ id ) {
    auto& cube = cube_list[id];
    auto fault = cube.fault();
    auto fid = fault->id();
    if ( cube_list_array[fid].empty() ) {
      fault_list.push_back(fid);
    }
    cube_list_array[fid].push_back(id);
  }
  mNodeList.reserve(fault_list.size());
  for ( auto fid: fault_list ) {
    auto fault = mNetwork.fault(fid);
    auto cvar = mBaseEnc.solver().new_variable(true);
    mNodeList.push_back({fault, 0, {}, {}, cvar});
    auto& id_list = cube_list_array[fid];
    vector<SatLiteral> tmp_lits;
    tmp_lits.reserve(id_list.size() + 1);
    tmp_lits.push_back(~cvar);
    for ( auto id: id_list ) {
      auto& cube = cube_list[id];
      auto assign = cube.assignments();
      auto lit_list = mBaseEnc.conv_to_literal_list(assign);
      auto var = mBaseEnc.solver().new_variable(false);
      lit_list.push_back(~var);
      mBaseEnc.solver().add_clause(lit_list);
      tmp_lits.push_back(var);
    }
  }

  Timer timer;
  timer.start();
  cout << "building blocking matrix" << endl;
  SizeType node_num = mNodeList.size();
  for ( SizeType id1 = 0; id1 < node_num - 1; ++ id1 ) {
    for ( SizeType id2 = id1 + 1; id2 < node_num; ++ id2 ) {
      if ( is_conflict(id1, id2) ) {
	mNodeList[id1].mConflictList.push_back(id2);
	mNodeList[id2].mConflictList.push_back(id1);
      }
    }
  }
  for ( SizeType id = 0; id < node_num; ++ id ) {
    auto& list = mNodeList[id].mConflictList;
    sort(list.begin(), list.end());
  }
  timer.stop();
  cout << "end" << endl
       << "CPU Time: " << timer.get_time() << endl;
}

// @brief デストラクタ
ColGraph2::~ColGraph2()
{
}

// @brief ノードの saturation degree を返す．
SizeType
ColGraph2::saturation_degree(
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
    vector<SatLiteral> assumptions;
    auto& group = mGroupList[col - 1];
    assumptions.reserve(group.mNodeList.size() + 1);
    auto& node1 = mNodeList[id];
    assumptions.push_back(node1.mControlVar);
    for ( auto id: group.mNodeList ) {
      auto& node = mNodeList[id];
      assumptions.push_back(node.mControlVar);
    }
    if ( mBaseEnc.solver().solve(assumptions) == SatBool3::False ) {
      mNodeList[id].mConflictColList.push_back(col);
      ++ sat;
    }
  }
  return sat;
}

// @brief ノードの adjacent degree を返す．
SizeType
ColGraph2::adjacent_degree(
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
    ++ adj;
  }
  return adj;
}

// @brief ノードを色をつける．
void
ColGraph2::set_color(
  SizeType id,
  SizeType color
)
{
  ASSERT_COND( 0 <= id && id < node_num() );
  ASSERT_COND( 1 <= color && color <= color_num() );

  mNodeList[id].mColor = color;
  auto& group = mGroupList[color - 1];
  group.mNodeList.push_back(id);
}

// @brief color_map を作る．
SizeType
ColGraph2::get_color_map(
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
ColGraph2::is_conflict(
  SizeType id1,
  SizeType id2
)
{
  auto clit1 = mNodeList[id1].mControlVar;
  auto clit2 = mNodeList[id2].mControlVar;
  vector<SatLiteral> assumptions = {clit1, clit2};
  return mBaseEnc.solver().solve(assumptions) == SatBool3::False;
}

END_NAMESPACE_DRUID
