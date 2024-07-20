
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
#include "Sim.h"
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
    mNodeList.push_back({fault, cover.cube_list(), 0, {}, {}, cvar});
  }

  SizeType limit = 10;
  if ( option.is_object() ) {
    if ( option.has_key("debug") ) {
      mDebug = option.get("debug").get_bool();
    }
    if ( option.has_key("colgraph_looplimit") ) {
      limit = option.get("colgraph_looplimit").get_int();
    }
  }
  make_conflict_list(limit);

  for ( auto& node: mNodeList ) {
    node.mAdjDegree = node.mConflictList.size();
  }
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
  return mNodeList[id].mConflictColList.size();
}

// @brief ノードの adjacent degree を返す．
SizeType
ColGraph::adjacent_degree(
  SizeType id
)
{
  return mNodeList[id].mAdjDegree;
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

// @brief 新しい色を割り当てる．
SizeType
ColGraph::new_color()
{
  SizeType color = color_num() + 1;
  // 最初は空なので全てのノードと両立している．
  // ただし既に彩色済みのノードは除く
  SizeType nn = mNodeList.size();
  vector<SizeType> tmp_list;
  tmp_list.reserve(nn);
  for ( SizeType id = 0; id < nn; ++ id ) {
    auto& node = mNodeList[id];
    if ( node.mColor > 0 ) {
      continue;
    }
    tmp_list.push_back(id);
  }
  mGroupList.push_back({color, {}, tmp_list});
  return color;
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

  auto& node = mNodeList[id];
  node.mColor = color;
  auto& group = mGroupList[color - 1];
  group.mNodeList.push_back(id);

  // node に隣接しているノードの指標を更新する．
  std::unordered_set<SizeType> mark;
  for ( auto id1: node.mConflictList ) {
    auto& node1 = mNodeList[id1];
    // 無条件に mAdjDegree は減らす
    -- node1.mAdjDegree;
    // node1 に隣接しているノードに既に color のノードが
    // あれば mCoflictColList は変わらない．
    // node1 と衝突している色に color がある場合も
    // mConflictColList は変わらない．
    bool has_same_color = false;
    for ( auto id2: node1.mConflictList ) {
      if ( id2 == id ) {
	continue;
      }
      auto& node2 = mNodeList[id2];
      if ( node2.mColor == color ) {
	has_same_color = true;
	break;
      }
    }
    if ( !has_same_color ) {
      for ( auto col1: node1.mConflictColList ) {
	if ( col1 == color ) {
	  has_same_color = true;
	  break;
	}
      }
    }
    if ( !has_same_color ) {
      node1.mConflictColList.push_back(color);
      mark.emplace(id1);
    }
  }

  // color の条件が変わったので今まで無関係だったノードが衝突している
  // 可能性がある．
  vector<SizeType> new_list;
  for ( auto id1: group.mCompatList ) {
    if ( mark.count(id1) > 0 ) {
      // 上で処理済み
      continue;
    }
    auto& node1 = mNodeList[id1];
    if ( node1.mColor > 0 ) {
      continue;
    }
    if ( is_conflict(id1, group.mNodeList) ) {
      node1.mConflictColList.push_back(color);
      continue;
    }
    new_list.push_back(id1);
  }
  std::swap(group.mCompatList, new_list);
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

// @brief 衝突リストを作る．
void
ColGraph::make_conflict_list(
  SizeType limit
)
{
  if ( mDebug ) {
    cout << "building conflict list" << endl;
  }

  Timer timer;
  timer.start();

  SizeType node_num = mNodeList.size();

  // 割り当てが衝突しているペアに印をつける．
  for ( SizeType id1 = 0; id1 < node_num - 1; ++ id1 ) {
    for ( SizeType id2 = id1 + 1; id2 < node_num; ++ id2 ) {
      if ( is_trivial_conflict(id1, id2) ) {
	auto key = id1 * node_num + id2;
	mConflictMark.emplace(key);
      }
    }
  }
  if ( mDebug ) {
    cout << "Total trivial conflict pairs: " << mConflictMark.size() << endl;
  }

  // シミュレーションを用いて両立ペアに印をつける．
  make_compat_mark(limit);
  if ( mDebug ) {
    cout << "simulation end" << endl
	 << "Total compat pairs: " << mCompatMark.size() << endl;
  }

  // SATソルバを用いて衝突ペアを求める．
  for ( SizeType id1 = 0; id1 < node_num - 1; ++ id1 ) {
    for ( SizeType id2 = id1 + 1; id2 < node_num; ++ id2 ) {
      SizeType key = id1 * node_num + id2;
      if ( mConflictMark.count(key) == 0 ) {
	if ( mCompatMark.count(key) > 0 ) {
	  // このペアは両立している．
	  continue;
	}
	if ( !is_conflict(id1, id2) ) {
	  continue;
	}
      }
      mNodeList[id1].mConflictList.push_back(id2);
      mNodeList[id2].mConflictList.push_back(id1);
    }
    //cout << "Node#" << id1 << ": "
    //<< mNodeList[id1].mConflictList.size() << endl;
  }
  SizeType n = 0;
  for ( SizeType id = 0; id < node_num; ++ id ) {
    auto& list = mNodeList[id].mConflictList;
    sort(list.begin(), list.end());
    n += list.size();
  }
  timer.stop();
  if ( mDebug ) {
    cout << "end" << endl
	 << "Total conflict pairs: " << n << endl
	 << "CPU Time: " << timer.get_time() << endl;
  }
}

// @brief 故障シミュレーションを用いて衝突ペアの候補を作る．
void
ColGraph::make_compat_mark(
  SizeType limit
)
{
  Sim sim{mNetwork};
  for ( SizeType no_change = 0; no_change < limit; ) {
    // 乱数を用いたシミュレーションを行う．
    sim.sim_random();
    // 各故障の検出条件を調べる．
    SizeType nn = mNodeList.size();
    vector<PackedVal> dbits_array(nn, PV_ALL0);
    for ( SizeType id = 0; id < nn; ++ id ) {
      auto& node = mNodeList[id];
      PackedVal dbits = PV_ALL0;
      for ( auto& cube: node.mCubeList ) {
	auto dbits1 = sim.check(cube);
	dbits |= dbits1;
	if ( dbits == PV_ALL1 ) {
	  break;
	}
      }
      dbits_array[id] = dbits;
    }
    // 同時に検出している故障ペアを求める．
    bool changed = false;
    for ( SizeType id1 = 0; id1 < nn - 1; ++ id1 ) {
      auto dbits1 = dbits_array[id1];
      for ( SizeType id2 = id1 + 1; id2 < nn; ++ id2 ) {
	auto dbits2 = dbits_array[id2];
	auto dbits3 = dbits1 & dbits2;
	if ( dbits3 != PV_ALL0 ) {
	  SizeType key = id1 * nn + id2;
	  if ( mCompatMark.count(key) == 0 ) {
	    mCompatMark.emplace(key);
	    changed = true;
	  }
	}
      }
    }
    if ( changed ) {
      no_change = 0;
    }
    else {
      ++ no_change;
    }
  }
}

// @brief node1 と node2 が衝突する時 true を返す(簡易版)．
bool
ColGraph::is_trivial_conflict(
  SizeType id1,
  SizeType id2
)
{
  auto& node1 = mNodeList[id1];
  auto& node2 = mNodeList[id2];
  for ( auto& assign1: node1.mCubeList ) {
    for ( auto& assign2: node2.mCubeList ) {
      auto res = compare(assign1, assign2);
      if ( res != -1 ) {
	return false;
      }
    }
  }
  return true;
}

// @brief cube1 と cube2 が衝突する時 true を返す．
bool
ColGraph::is_conflict(
  SizeType id1,
  SizeType id2
)
{
  auto& node1 = mNodeList[id1];
  auto& node2 = mNodeList[id2];
  auto clit1 = node1.mControlVar;
  auto clit2 = node2.mControlVar;
  vector<SatLiteral> assumptions = {clit1, clit2};
  return mBaseEnc.solver().solve(assumptions) == SatBool3::False;
}

// @brief ノードとノード集合が衝突するとき true を返す．
bool
ColGraph::is_conflict(
  SizeType id1,
  const vector<SizeType>& id_list
)
{
  auto& node1 = mNodeList[id1];
  auto clit1 = node1.mControlVar;
  vector<SatLiteral> assumptions;
  assumptions.reserve(id_list.size() + 1);
  assumptions.push_back(clit1);
  for ( auto id2: id_list ) {
    auto& node2 = mNodeList[id2];
    auto clit2 = node2.mControlVar;
    assumptions.push_back(clit2);
  }
  return mBaseEnc.solver().solve(assumptions) == SatBool3::False;
}

END_NAMESPACE_DRUID
