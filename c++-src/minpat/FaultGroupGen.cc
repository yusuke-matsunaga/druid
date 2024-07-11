
/// @file FaultGroupGen.cc
/// @brief FaultGroupGen の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "FaultGroupGen.h"
#include "TpgNetwork.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
FaultGroupGen::FaultGroupGen(
  const TpgNetwork& network,
  const JsonValue& option
) : mNetwork{network},
    mBaseEnc{network, option}
{
  auto& node_list = network.node_list();
  mBaseEnc.make_cnf(node_list, node_list);
}

// @brief デストラクタ
FaultGroupGen::~FaultGroupGen()
{
}

// @brief 両立故障グループを求める．
vector<vector<FaultInfo>>
FaultGroupGen::generate(
  const vector<FaultInfo>& finfo_list,
  SizeType limit
)
{
  // mCubeList を作る．
  mCubeList.clear();
  for ( auto& finfo: finfo_list ) {
    auto& fault = finfo.fault();
    auto fid = fault->id();
    for ( auto& assign: finfo.sufficient_conditions() ) {
      mCubeList.push_back({assign, fid});
    }
  }

  // 故障集合を初期化する．
  // 初期値は空
  init();

  // limit 分の故障集合を求める．
  for ( SizeType count; count < limit; ++ count ) {
    // 極大集合を求める．
    greedy_mcset(count);

    // 記録する．
    // 場合によっては重複チェックを行う．
    record();

    // signature から価値の低いキューブ取り除く
    auto cube_id = remove_cube();

    // cube_id をタブーリストに入れる．
    mTabuList[cube_id] = count + mTenure;
  }
}

// @brief 故障集合を初期化する．
void
FaultGroupGppppen::init()
{
  mFaultSet.clear();
  mFaultSet.resize(mNetwork.max_fault_id(), false);
  mCubeSet.clear();
  mCubeSet.resize(mCubeList.size(), false);
  mAssignments.clear();
  mTabuList.resize(mCubeList.size(), 0);
}

// @brief 極大集合を求める．
void
FaultGroupGen::greedy_mcset(
  SizeType count
)
{
  for ( ; ; ) {
    // 未選択の故障の拡張テストキューブのうち，
    // 新規に追加したときに候補の故障数が
    // 最大となるものを求める．
    auto cube_id = select_cube(count);

    // 追加できる故障がなくなったら終わる．
    if ( cube_id == mCubeList.size() ) {
      return;
    }

    // 故障集合を更新する．
    auto& cube = mCubeList[cube_id];
    mFaultSet[cube.mFaultId] = true;
    mCubeSet[cube_id] = true;
    mAssignments.merge(cube.mAssignments);
  }
}

// @brief 最も価値の高いキューブを選ぶ．
SizeType
FaultGroupGen::select_cube(
  SizeType count
)
{
  SizeType max_cube_id = mCubeList.size();
  SizeType max_num = 0;
  for ( SizeType cube_id = 0; cube_id < mCubeList.size(); ++ cube_id ) {
    if ( mTabuList[cube_id] >= count ) {
      // 禁止されている．
      continue;
    }
    auto& cube = mCubeList[cube_id];
    if ( mFaultSet[cube.mFaultId] ) {
      // 既に同じ故障のキューブが選ばれている．
      continue;
    }
    if ( !is_compatible(cube.mAssignments, mAssignments) ) {
      // 衝突していた．
      continue;
    }
    SizeType num = count_faults(cube.mAssignments);
    if ( max_num < num ) {
      max_num = num;
      max_cube_id = cube_id;
    }
  }
  return max_cube_id;
}

// @brief 追加後の候補故障の数を数える．
SizeType
FaultGroupGen::count_faults(
  const NodeTimeValList& assignments
)
{
  auto tmp_assign{mAssignments};
  tmp_assign.merge(assignments);
  SizeType num = 0;
  vector<bool> fault_set(mNetwork.max_fault_id(), false);
  for ( auto& cube: mCubeList ) {
    if ( mFaultSet[cube.mFaultId] ) {
      continue;
    }
    if ( fault_set[cube.mFaultId] ) {
      continue;
    }
    if ( is_compatible(cube.mAssignments, tmp_assign) ) {
      ++ num;
      fault_set[cube.mFaultId] = true;
    }
  }
  return num;
}

// @brief 現在の故障集合を記録する．
void
FaultGroupGen::record()
{
  SizeType nf = 0;
  for ( SizeType fid = 0; fid < mNetwork.max_fault_id(); ++ fid ) {
    if ( mFaultSet[fid] ) {
      ++ nf;
    }
  }
  cout << nf << " faults" << endl;
}

// @brief 最も価値の低いキューブを選んで取り除く
SizeType
FaultGroupGen::remove_cube()
{
}

// @brief 両立性のチェック
bool
FaultGroupGen::is_compatible(
  const ExCube& assignments1,
  const ExCube& assignments2
)
{
  auto lits1 = mBaseEnc.conv_to_literal_list(assignments1);
  auto lits2 = mBaseEnc.conv_to_literal_list(assignments2);
  lits1.insert(lits1.end(), lits2.begin(), lits2.end());
  return mBaseEnc.solver().solve(lits1) == SatBool3::True;
}

END_NAMESPACE_DRUID
