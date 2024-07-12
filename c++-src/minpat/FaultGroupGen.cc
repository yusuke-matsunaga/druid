
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


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
FaultGroupGen::FaultGroupGen(
  const TpgNetwork& network,
  const JsonValue& option
) : mNetwork{network},
    mBaseEnc{network, option}
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
}

// @brief 両立故障グループを求める．
vector<vector<SizeType>>
FaultGroupGen::generate(
  const vector<FaultInfo>& finfo_list,
  SizeType limit
)
{
  // 初期化する．
  init(finfo_list);

  // limit 分の故障集合を求める．
  for ( SizeType count = 0; count < limit; ++ count ) {
    // 極大集合を求める．
    if ( !greedy_mcset() ) {
      break;
    }

    // 更新する．
    // 場合によっては重複チェックを行う．
    update();
  }

  if ( mDebug ) {
    cout << "Total " << mFaultGroupList.size() << " groups" << endl;
    for ( auto& finfo: finfo_list ) {
      auto fault = finfo.fault();
      if ( mCountArray[fault->id()] == 0 ) {
	cout << fault->str() << " is not covered" << endl;
      }
    }
  }

  return mFaultGroupList;
}

// @brief 故障集合を初期化する．
void
FaultGroupGen::init(
  const vector<FaultInfo>& finfo_list
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
  for ( auto& finfo: finfo_list ) {
    auto fault = finfo.fault();
    auto fid = fault->id();
    if ( mDebug ) {
      cout << fault->str();
      cout.flush();
      cout << endl;
    }
    for ( auto& assign: finfo.sufficient_conditions() ) {
      auto new_assign = imp.run(assign);
      auto new_assign2 = imply(new_assign);
      cout << assign.size() << " -> " << new_assign.size()
	   << " -> " << new_assign2.size() << endl;
      c0 += assign.size();
      c1 += new_assign.size();
      c2 += new_assign2.size();
      SizeType id = mCubeList.size();
      mCubeList.push_back({id, new_assign2, fid});
    }
    if ( mDebug ) {
      cout << "... done" << endl;
    }
  }
  cout << "Total assign size:  " << c0 << endl
       << "Total assign1 size: " << c1 << endl
       << "Total assign2 size: " << c2 << endl;

  mCountArray.clear();
  mCountArray.resize(mNetwork.max_fault_id(), 0);
  mCurFaultList.clear();
  mCurFaultSet.clear();
  mCurFaultSet.resize(mNetwork.max_fault_id(), false);
  mCurCubeList.clear();
  mCurCubeSet.clear();
  mCurCubeSet.resize(mCubeList.size(), false);
  mCubeListArray.clear();
  mCubeListArray.push_back({});
  auto& dst_list = mCubeListArray[0];
  dst_list.reserve(mCubeList.size());
  for ( SizeType i = 0; i < mCubeList.size(); ++ i ) {
    dst_list.push_back(i);
  }

  timer.stop();
  if ( mDebug ) {
    cout << "Total # of cubes: " << mCubeList.size() << endl
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
bool
FaultGroupGen::greedy_mcset()
{
  mCurCandList.clear();
  for ( auto& cube: mCubeList ) {
    mCurCandList.push_back(&cube);
  }

  // 各故障に対して現時点でカバーされている回数+1の逆数を重み
  // として，重みが最大(極大)となるような故障集合を求める．
  for ( ; ; ) {
    // 未選択の故障の拡張テストキューブのうち，
    // 候補の重みの和)が最大となるものを求める．
    auto cube = select_cube();

    // 追加できる故障がなくなったら終わる．
    if ( cube == nullptr ) {
      return !mCurFaultList.empty();
    }

    // 故障集合を更新する．
    mCurFaultList.push_back(cube->mFaultId);
    mCurFaultSet[cube->mFaultId] = true;
    mCurCubeList.push_back(cube->mId);
    mCurCubeSet[cube->mId] = true;
    mCurAssignments.merge(cube->mAssignments);
    vector<ExCube*> new_list;
    for ( auto cube: mCurCandList ) {
      if ( is_compatible(mCurAssignments, cube->mAssignments) ) {
	new_list.push_back(cube);
      }
    }
    std::swap(mCurCandList, new_list);
  }
}

// @brief 最も価値の高いキューブを選ぶ．
FaultGroupGen::ExCube*
FaultGroupGen::select_cube()
{
  ExCube* max_cube = nullptr;
#if 0
  for ( auto& cube_list: mCubeListArray ) {
    double max_weight = 0.0;
    for ( auto cube_id: cube_list ) {
      cout << "cube_id = " << cube_id << endl;
      auto& cube = mCubeList[cube_id];
      if ( mCurFaultSet[cube.mFaultId] ) {
	// 既に同じ故障のキューブが選ばれている．
	cout << "Fault#" << cube.mFaultId << " is already covered" << endl;
	continue;
      }
      if ( !is_compatible(cube.mAssignments, mCurAssignments) ) {
	// 衝突していた．
	cout << " is not compatible" << endl;
	continue;
      }
      double weight = count_weight(cube.mAssignments);
      if ( max_weight < weight ) {
	max_weight = weight;
	max_cube_id = cube_id;
      }
    }
    if ( max_weight > 0 ) {
      break;
    }
  }
#else
  double max_weight = 0.0;
  for ( auto cube: mCurCandList ) {
    if ( mCurFaultSet[cube->mFaultId] ) {
      continue;
    }
    double weight = count_weight(cube);
    if ( max_weight < weight ) {
      max_weight = weight;
      max_cube = cube;
    }
  }
#endif
  return max_cube;
}

// @brief 追加後の候補故障の重みを計算する．
double
FaultGroupGen::count_weight(
  const ExCube* cube
)
{
  auto tmp_assign{mCurAssignments};
  tmp_assign.merge(cube->mAssignments);
  double weight = (1 / (mCountArray[cube->mFaultId] + 1));
  vector<bool> fault_set(mNetwork.max_fault_id(), false);
  fault_set[cube->mFaultId] = true;
  for ( auto cube: mCurCandList ) {
    auto fid = cube->mFaultId;
    if ( mCurFaultSet[fid] ) {
      continue;
    }
    if ( fault_set[fid] ) {
      continue;
    }
    if ( is_compatible(cube->mAssignments, tmp_assign) ) {
      weight += (1 / (mCountArray[fid] + 1));
      fault_set[cube->mFaultId] = true;
    }
  }
  return weight;
}

// @brief 更新する．
void
FaultGroupGen::update()
{
  if ( mDebug ) {
    for ( auto fid: mCurFaultList ) {
      auto fault = mNetwork.fault(fid);
      cout << " " << fault->str()
	   << " [" << mCountArray[fault->id()] << "]";
    }
    cout << endl;
  }

  mFaultGroupList.push_back({});
  for ( auto fid: mCurFaultList ) {
    ++ mCountArray[fid];
    mCurFaultSet[fid] = false;
    mFaultGroupList.back().push_back(fid);
  }
  mCurFaultList.clear();
  for ( auto cube_id: mCurCubeList ) {
    mCurCubeSet[cube_id] = false;
  }
  mCurCubeList.clear();

  mCurAssignments.clear();

  // とりあえずナイーブなやり方で実装する．
  SizeType nc = mCubeListArray.size();
  for ( SizeType c = 0; c < nc; ++ c ) {
    auto& cube_list = mCubeListArray[c];
#if 0
    auto rpos = cube_list.begin();
    auto wpos = rpos;
    auto epos = cube_list.end();
    for ( ; rpos != epos; ++ rpos ) {
      auto cube_id = *rpos;
      auto& cube = mCubeList[cube_id];
      auto fid = cube.mFaultId;
      auto new_c = mCountArray[fid];
      if ( new_c > c ) {
	if ( new_c == nc ) {
	  mCubeListArray.push_back({});
	}
	mCubeListArray[new_c].push_back(cube_id);
      }
      else {
	if ( wpos != rpos ) {
	  *wpos = cube_id;
	}
	++ wpos;
      }
    }
    if ( wpos != epos ) {
      cube_list.erase(wpos);
    }
#else
    vector<SizeType> dst_list;
    for ( auto cube_id: mCubeListArray[c] ) {
      auto& cube = mCubeList[cube_id];
      auto fid = cube.mFaultId;
      auto new_c = mCountArray[fid];
      if ( new_c > c ) {
	if ( new_c == nc ) {
	  mCubeListArray.push_back({});
	}
	mCubeListArray[new_c].push_back(cube_id);
      }
      else {
	dst_list.push_back(cube_id);
      }
    }
    mCubeListArray[c] = dst_list;
#endif
  }
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
