
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
    mBaseEnc{network, option}
{
  {
    auto& node_list = network.node_list();
    mBaseEnc.make_cnf(node_list, node_list);
  }
  mNodeList.reserve(cover_list.size());
  for ( auto& cover: cover_list ) {
    auto fault = cover.fault();
    auto fid = fault->id();
    auto cvar = mBaseEnc.solver().new_variable(true);
    vector<SatLiteral> tmp_lits;
    tmp_lits.reserve(cover.cube_list().size());
    tmp_lits.push_back(~cvar);
    for ( auto& cube: cover.cube_list() ) {
      auto var = mBaseEnc.solver().new_variable(false);
      for ( auto nv: cube ) {
	auto lit = mBaseEnc.conv_to_literal(nv);
	mBaseEnc.solver().add_clause(~var, lit);
      }
      tmp_lits.push_back(var);
    }
    mBaseEnc.solver().add_clause(tmp_lits);
    mNodeList.push_back({fault, 0, {}, {}, cvar});
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
    cout << "Node#" << id1 << ": " << mNodeList[id1].mConflictList.size() << endl;
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
ColGraph::adjacent_degree(
  SizeType id
)
{
  SizeType adj = 0;
  for ( auto id1: mNodeList[id].mConflictList ) {
    if ( color(id1) > 0 ) {
      // 彩色済みのノードはスキップ
      continue;
    }
    ++ adj;
  }
  return adj;
}

// @brief 指定された色のテストベクタを返す．
TestVector
ColGraph::testvector(
  SizeType color
)
{
  auto& group = mGroupList[color - 1];
  vector<SatLiteral> assumptions;
  assumptions.reserve(group.mNodeList.size());
  for ( auto id: group.mNodeList ) {
    auto& node = mNodeList[id];
    assumptions.push_back(node.mControlVar);
  }
  auto res = mBaseEnc.solver().solve(assumptions);
  if ( res != SatBool3::True ) {
    throw std::invalid_argument{"wrong assignments"};
  }

  auto pi_assign = mBaseEnc.get_pi_assign();
  return TestVector{mNetwork, pi_assign};
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

  mNodeList[id].mColor = color;
  auto& group = mGroupList[color - 1];
  group.mNodeList.push_back(id);
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
