
/// @file FaultGroupGen.cc
/// @brief FaultGroupGen の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "FaultGroupGen.h"
#include "TpgNetwork.h"
#include "TpgFault.h"


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
  // mCubeList を作る．
  mCubeList.clear();
  for ( auto& finfo: finfo_list ) {
    auto fault = finfo.fault();
    auto fid = fault->id();
    for ( auto& assign: finfo.sufficient_conditions() ) {
      mCubeList.push_back({assign, fid});
    }
  }

  // 故障集合を初期化する．
  // 初期値は空
  init();

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
  }

  return mFaultGroupList;
}

// @brief 故障集合を初期化する．
void
FaultGroupGen::init()
{
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
}

// @brief 極大集合を求める．
bool
FaultGroupGen::greedy_mcset()
{
  cout << "greedy_mcset()" << endl;
  // 各故障に対して現時点でカバーされている回数+1の逆数を重み
  // として，重みが最大(極大)となるような故障集合を求める．
  for ( ; ; ) {
    // 未選択の故障の拡張テストキューブのうち，
    // (自身の重み, 候補の重みの和)が
    // 辞書式順序で最大となるものを求める．
    auto cube_id = select_cube();

    // 追加できる故障がなくなったら終わる．
    if ( cube_id == mCubeList.size() ) {
      return !mCurFaultList.empty();
    }

    // 故障集合を更新する．
    auto& cube = mCubeList[cube_id];
    mCurFaultList.push_back(cube.mFaultId);
    mCurFaultSet[cube.mFaultId] = true;
    mCurCubeList.push_back(cube_id);
    mCurCubeSet[cube_id] = true;
    mCurAssignments.merge(cube.mAssignments);
  }
}

// @brief 最も価値の高いキューブを選ぶ．
SizeType
FaultGroupGen::select_cube()
{
  SizeType max_cube_id = mCubeList.size();
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
      cout << "  weight = " << weight << endl;
      if ( max_weight < weight ) {
	max_weight = weight;
	max_cube_id = cube_id;
      }
    }
    if ( max_weight > 0 ) {
      break;
    }
  }
  return max_cube_id;
}

// @brief 追加後の候補故障の重みを計算する．
double
FaultGroupGen::count_weight(
  const NodeTimeValList& assignments
)
{
  auto tmp_assign{mCurAssignments};
  tmp_assign.merge(assignments);
  double weight = 0.0;
  vector<bool> fault_set(mNetwork.max_fault_id(), false);
  for ( auto& cube: mCubeList ) {
    auto fid = cube.mFaultId;
    if ( mCurFaultSet[fid] ) {
      continue;
    }
    if ( fault_set[fid] ) {
      continue;
    }
    if ( is_compatible(cube.mAssignments, tmp_assign) ) {
      weight += (1 / (mCountArray[fid] + 1));
      fault_set[cube.mFaultId] = true;
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

  {
    SizeType nc = 0;
    for ( SizeType i = 0; i < mCubeListArray.size(); ++ i ) {
      for ( auto cube_id: mCubeListArray[i] ) {
	++ nc;
	auto& cube = mCubeList[cube_id];
	auto fid = cube.mFaultId;
	if ( mCountArray[fid] != i ) {
	  cout << mNetwork.fault(mCubeList[cube_id].mFaultId)->str() << endl;
	  abort();
	}
      }
    }
    if ( nc != mCubeList.size() ) {
      cout << "nc = " << nc
	   << ", mCubeList.size() = " << mCubeList.size() << endl;
    }
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
