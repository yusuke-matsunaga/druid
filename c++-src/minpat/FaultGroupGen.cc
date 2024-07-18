
/// @file FaultGroupGen.cc
/// @brief FaultGroupGen の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "FaultGroupGen.h"
#include "TpgNetwork.h"
#include "TpgFFR.h"
#include "TpgFault.h"
#include "LocalImp.h"
#include "TestVectorGen.h"
#include "TestVector.h"
#include "Fsim.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// リストのユニオンの要素数を数える．
SizeType
count_union(
  const vector<SizeType>& list1,
  const vector<SizeType>& list2
)
{
  auto rpos1 = list1.begin();
  auto rpos2 = list2.begin();
  auto epos1 = list1.end();
  auto epos2 = list2.end();
  SizeType count = 0;
  while ( rpos1 != epos1 && rpos2 != epos2 ) {
    SizeType v1 = *rpos1;
    SizeType v2 = *rpos2;
    if ( v1 < v2 ) {
      ++ count;
      ++ rpos1;
    }
    else if ( v1 > v2 ) {
      ++ count;
      ++ rpos2;
    }
    else { // v1 == v2
      ++ count;
      ++ rpos1;
      ++ rpos2;
    }
  }
  count += (epos1 - rpos1);
  count += (epos2 - rpos2);

  return count;
}

// リストの差分を求める．
void
diff(
  vector<SizeType>& list1,
  const vector<SizeType>& list2
)
{
  auto rpos1 = list1.begin();
  auto rpos2 = list2.begin();
  auto epos1 = list1.end();
  auto epos2 = list2.end();
  vector<SizeType> ans_list;
  ans_list.reserve(list1.size());
  while ( rpos1 != epos1 && rpos2 != epos2 ) {
    SizeType v1 = *rpos1;
    SizeType v2 = *rpos2;
    if ( v1 < v2 ) {
      ans_list.push_back(v1);
      ++ rpos1;
    }
    else if ( v1 > v2 ) {
      ++ rpos2;
    }
    else { // v1 == v2
      ++ rpos1;
      ++ rpos2;
    }
  }
  for ( ; rpos1 != epos1; ++ rpos1 ) {
    SizeType v1 = *rpos1;
    ans_list.push_back(v1);
  }
  std::swap(ans_list, list1);
}

// リストの差分の要素数を数える．
SizeType
count_diff(
  const vector<SizeType>& list1,
  const vector<SizeType>& list2
)
{
  auto rpos1 = list1.begin();
  auto rpos2 = list2.begin();
  auto epos1 = list1.end();
  auto epos2 = list2.end();
  SizeType count = 0;
  while ( rpos1 != epos1 && rpos2 != epos2 ) {
    SizeType v1 = *rpos1;
    SizeType v2 = *rpos2;
    if ( v1 < v2 ) {
      ++ count;
      ++ rpos1;
    }
    else if ( v1 > v2 ) {
      ++ rpos2;
    }
    else { // v1 == v2
      ++ rpos1;
      ++ rpos2;
    }
  }
  count += (epos1 - rpos1);

  return count;
}

// リストをマージする．
void
merge_list(
  vector<SizeType>& list1,
  const vector<SizeType>& list2
)
{
  vector<SizeType> new_list;
  auto rpos1 = list1.begin();
  auto rpos2 = list2.begin();
  auto epos1 = list1.end();
  auto epos2 = list2.end();
  new_list.reserve(list1.size() + list2.size());
  while ( rpos1 != epos1 && rpos2 != epos2 ) {
    SizeType v1 = *rpos1;
    SizeType v2 = *rpos2;
    if ( v1 <= v2 ) {
      new_list.push_back(v1);
      ++ rpos1;
      if ( v1 == v2 ) {
	++ rpos2;
      }
    }
    else { // v1 > v2
      new_list.push_back(v2);
      ++ rpos2;
    }
  }
  for ( ; rpos1 != epos1; ++ rpos1 ) {
    SizeType v1 = *rpos1;
    new_list.push_back(v1);
  }
  for ( ; rpos2 != epos2; ++ rpos2 ) {
    SizeType v2 = *rpos2;
    new_list.push_back(v2);
  }
  std::swap(new_list, list1);
}

END_NONAMESPACE

// @brief コンストラクタ
FaultGroupGen::FaultGroupGen(
  const TpgNetwork& network,
  const JsonValue& option
) : mNetwork{network},
    mBaseEnc{network, option},
    mBlockListArray(network.node_num() * 4),
    mCountArray(network.max_fault_id(), 0)
{
  if ( option.is_object() &&
       option.has_key("debug") ) {
    mDebug = option.get("debug").get_bool();
  }
  auto& node_list = network.node_list();
  mBaseEnc.make_cnf(node_list, node_list);
}

// @brief デストラクタ
FaultGroupGen::~FaultGroupGen()
{
  for ( auto cube: mCubeList ) {
    delete cube;
  }
}

// @brief 両立故障グループを求める．
vector<NodeTimeValList>
FaultGroupGen::generate(
  const vector<const TpgFault*>& fault_list,
  const vector<TestCube>& cube_list
)
{
  // 初期化する．
  init(fault_list, cube_list);

  // ブロックリストを作る．
  gen_blocklist();

  // limit 分の故障集合を求める．
  vector<NodeTimeValList> ans_list;
  for ( ; ; ) {
    // 極大集合を求める．
    SizeType old_num = mFaultNum;
    auto assignments = greedy_mcset();
    if ( assignments.size() == 0 ) {
      break;
    }

    if ( mDebug ) {
      auto n = old_num - mFaultNum;
      cout << "#" << ans_list.size()
	   << ": " << n << " | " << mFaultNum
	   << endl;
    }

    ans_list.push_back(assignments);
  }

  { // verify
    for ( auto fault: fault_list ) {
      if ( mCountArray[fault->id()] == 0 ) {
	cout << fault->str() << " is not covered" << endl;
      }
    }
  }

  if ( mDebug ) {
    cout << "Total " << ans_list.size() << " groups" << endl;
  }

  return ans_list;
}

// @brief 故障集合を初期化する．
void
FaultGroupGen::init(
  const vector<const TpgFault*>& fault_list,
  const vector<TestCube>& cube_list
)
{
  Timer timer;
  timer.start();

  LocalImp imp{mNetwork};

  // mCubeList を作る．
  // その際に implication learning を行う．
  mCubeList.clear();
  SizeType c0 = 0;
  SizeType c1 = 0;
  SizeType c2 = 0;
  for ( auto& cube: cube_list ) {
    auto fault = cube.fault();
    auto fid = fault->id();
    mCountArray[fid] = 0;
    auto& assign = cube.assignments();
    auto new_assign = imp.run(assign);
#if 0
    auto new_assign2 = imply(new_assign);
#endif
    SizeType id = mCubeList.size();
    auto excube = new ExCube{id, new_assign, fid};
    mCubeList.push_back(excube);
  }
  mBlockListArray.clear();
  mCurBlockListArray.clear();
  mBlockListArray.resize(mCubeList.size());
  mCurBlockListArray.resize(mCubeList.size());
  mFaultNum = fault_list.size();

  timer.stop();
  if ( mDebug ) {
    cout << "Total # of cubes: " << mCubeList.size() << endl
	 << "CPU time:         " << timer.get_time() << endl;
  }
}

// @brief 各キューブごとのブロックリストを作る．
void
FaultGroupGen::gen_blocklist()
{
  Timer timer;
  timer.start();

  // 各インデックスごとのブロックリストを作る．
  vector<vector<SizeType>> block_list_array(mNetwork.node_num() * 4);
  for ( SizeType cube_id = 0; cube_id < mCubeList.size(); ++ cube_id ) {
    auto cube = mCubeList[cube_id];
    for ( auto nv: cube->mAssignments ) {
      auto node = nv.node();
      auto time = nv.time();
      auto val = nv.val();
      SizeType index = node->id() * 2 + time;
      SizeType offset = val ? 0 : 1;
      block_list_array[index * 2 + offset].push_back(cube_id);
    }
  }

  timer.stop();
  if ( mDebug ) {
    cout << "BlockList cnstruction(1)" << endl
	 << "CPU time:         " << timer.get_time() << endl;
  }

  timer.reset();
  timer.start();

  // 各キューブごとのブロックリストを作る．
  for ( SizeType cube_id = 0; cube_id < mCubeList.size(); ++ cube_id ) {
    auto cube = mCubeList[cube_id];
    auto& dst_list = mBlockListArray[cube->mId];
    for ( auto nv: cube->mAssignments ) {
      auto node = nv.node();
      auto time = nv.time();
      auto val = nv.val();
      SizeType index = node->id() * 2 + time;
      SizeType offset = val ? 1 : 0;
      merge_list(dst_list, block_list_array[index * 2 + offset]);
    }
  }

  timer.stop();
  if ( mDebug ) {
    cout << "BlockList construction(2)" << endl
	 << "CPU time:         " << timer.get_time() << endl;
  }
}

// @brief 拡張テストキューブに対する含意を行う．
NodeTimeValList
FaultGroupGen::imply(
  const NodeTimeValList& assignments
)
{
  auto assumptions = mBaseEnc.conv_to_literal_list(assignments);
  assumptions.push_back(SatLiteral::X);
  std::unordered_set<NodeTimeVal> mark;
  for ( auto nv: assignments ) {
    mark.emplace(nv);
  }
  NodeTimeValList new_assign{assignments};
  for ( auto ffr: mNetwork.ffr_list() ) {
    auto node = ffr->root();
    auto nv0 = NodeTimeVal{node, 1, false};
    auto nv1 = NodeTimeVal{node, 1, true};
    if ( mark.count(nv0) > 0 || mark.count(nv1) > 0 ) {
      continue;
    }
    auto lit = mBaseEnc.conv_to_literal(nv1);
    assumptions.back() = lit;
    if ( mBaseEnc.solver().solve(assumptions) == SatBool3::False ) {
      // 肯定のリテラルを足したら UNSAT だった
      // -> 否定のリテラルが含意される．
      new_assign.add({node, 1, false});
    }
    assumptions.back() = ~lit;
    if ( mBaseEnc.solver().solve(assumptions) == SatBool3::False ) {
      // 否定のリテラルを足したら UNSAT だった
      // -> 肯定のリテラルが含意される．
      new_assign.add({node, 1, true});
    }
    if ( mNetwork.has_prev_state() ) {
      // 1時刻前の割当も試す．
      auto nv0 = NodeTimeVal{node, 0, false};
      auto nv1 = NodeTimeVal{node, 0, true};
      if ( mark.count(nv0) > 0 || mark.count(nv1) > 0 ) {
	continue;
      }
      auto lit = mBaseEnc.conv_to_literal(nv1);
      assumptions.back() = lit;
      if ( mBaseEnc.solver().solve(assumptions) == SatBool3::False ) {
	// 肯定のリテラルを足したら UNSAT だった
	// -> 否定のリテラルが含意される．
	new_assign.add({node, 0, false});
      }
      assumptions.back() = ~lit;
      if ( mBaseEnc.solver().solve(assumptions) == SatBool3::False ) {
	// 否定のリテラルを足したら UNSAT だった
	// -> 肯定のリテラルが含意される．
	new_assign.add({node, 0, true});
      }
    }
  }
  return new_assign;
}

// @brief 拡張テストキューブに対する含意を行う．
void
FaultGroupGen::check_imp(
  const NodeTimeValList& assignments0,
  const NodeTimeValList& assignments1
)
{
  auto tmp = assignments1 - assignments0;
  auto assumptions = mBaseEnc.conv_to_literal_list(assignments0);
  assumptions.push_back(SatLiteral::X);
  for ( auto nv: tmp ) {
    auto lit = mBaseEnc.conv_to_literal(nv);
    assumptions.back() = ~lit;
    if ( mBaseEnc.solver().solve(assumptions) != SatBool3::False ) {
      cout << "Error: assignments = " << assignments0 << endl
	   << "       nv = " << nv << endl;
    }
  }
}

// @brief 極大集合を求める．
NodeTimeValList
FaultGroupGen::greedy_mcset()
{
  {
    for ( auto cube: mCubeList ) {
      auto& assign = cube->mAssignments;
      SizeType dummy = 0;
      for ( auto nv: assign ) {
	auto node = nv.node();
	auto id = node->id();
	dummy += id;
      }
    }
  }

  // 現在選択されているキューブ番号を表すリスト
  vector<SizeType> selected_list;
  // 現在選択されている故障集合を表すビットマップ
  vector<bool> selected_map(mNetwork.max_fault_id(), false);
  // 現在選択されているキューブの割り当て
  NodeTimeValList cur_assignments;
  // 現在選択されている故障のリスト(デバッグ，検証用)
  vector<const TpgFault*> cur_fault_list;

  // 現在選択されているキューブ集合でブロックされている
  // キューブ番号のリスト
  vector<SizeType> cur_block_list;
  // 現在選択されているキューブ集合でブロックされている
  // キューブ集合を表すビットマップ
  vector<bool> blocked_map(mCubeList.size(), false);

  for ( auto cube: mCubeList ) {
    if ( mCountArray[cube->mId] > 0 ) {
      mCurBlockListArray[cube->mId].clear();
    }
    else {
      mCurBlockListArray[cube->mId] = mBlockListArray[cube->mId];
    }
  }

  // 未選択のキューブのうち，新規に追加した時にブロックリストの増加が
  // 最小となるものを求める．
  for ( ; ; ) {
    vector<pair<const ExCube*, double>> cube_weight_list;
    for ( auto cube: mCubeList ) {
      if ( mCountArray[cube->mFaultId] > 0 ) {
	continue;
      }
      if ( selected_map[cube->mFaultId] ) {
	// すでに選択済み
	continue;
      }
      if ( blocked_map[cube->mId] ) {
	// ブロックされているキューブ
	continue;
      }

      // このキューブを選択すると新たにブロックされるキューブ
#if 0
      double weight = - (1.0 / (mCountArray[cube->mFaultId] + 1));
      for ( auto id: mCurBlockListArray[cube->mId] ) {
	auto cube1 = mCubeList[id];
	auto fid = cube1->mFaultId;
	weight += 1.0 / (mCountArray[fid] + 1);
      }
#else
      double weight = -1.0;
      for ( auto id: mCurBlockListArray[cube->mId] ) {
	auto cube1 = mCubeList[id];
	auto fid = cube1->mFaultId;
	if ( mCountArray[fid] == 0 ) {
	  weight += 1.0;
	}
      }
#endif
      cube_weight_list.push_back({cube, weight});
    }
    if ( cube_weight_list.empty() ) {
      break;
    }
    sort(cube_weight_list.begin(), cube_weight_list.end(),
	 [](const pair<const ExCube*, double>& a,
	    const pair<const ExCube*, double>& b)
	 {
	   return a.second < b.second;
	 });
    bool found = false;
    for ( auto& p: cube_weight_list ) {
      auto cube = p.first;
      if ( is_compatible(cur_assignments, cube->mAssignments) ) {
	// このキューブを選択する．
	selected_list.push_back(cube->mId);
	selected_map[cube->mFaultId] = true;
	cur_assignments.merge(cube->mAssignments);
	++ mCountArray[cube->mFaultId];
	-- mFaultNum;
	cur_fault_list.push_back(mNetwork.fault(cube->mFaultId));
	auto& blist = mBlockListArray[cube->mId];
	for ( auto cube1: mCubeList ) {
	  if ( mCountArray[cube1->mId] > 0 ) {
	    continue;
	  }
	  diff(mCurBlockListArray[cube1->mId], blist);
	}
	for ( auto i: mBlockListArray[cube->mId] ) {
	  blocked_map[i] = true;
	}
	found = true;
	break;
      }
    }
    if ( !found ) {
      break;
    }
  }
  return cur_assignments;
}

// @brief 両立性のチェック
bool
FaultGroupGen::is_compatible(
  const NodeTimeValList& assignments1,
  const NodeTimeValList& assignments2
)
{
  auto lits1 = mBaseEnc.conv_to_literal_list(assignments1);
  auto lits2 = mBaseEnc.conv_to_literal_list(assignments2);
  lits1.insert(lits1.end(), lits2.begin(), lits2.end());
  return mBaseEnc.solver().solve(lits1) == SatBool3::True;
}

END_NAMESPACE_DRUID
