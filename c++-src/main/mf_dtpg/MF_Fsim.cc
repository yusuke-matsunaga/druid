
/// @file MF_Fsim.cc
/// @brief MF_Fsim の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2019 Yusuke Matsunaga
/// All rights reserved.


#include "MF_Fsim.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス MF_Fsim
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] network 対象のネットワーク
// @param[in] fault_type 故障の種類
// @param[in] solver_type SATソルバの実装タイプ
MF_Fsim::MF_Fsim(const TpgNetwork& network,
		 FaultType fault_type,
		 const SatSolverType& solver_type) :
  mNetwork(network),
  mFaultType(fault_type),
  mSolver(solver_type),
  mFaultVarArray(mNetwork.max_fault_id())
{
  // ノード番号をキーにして変数番号を入れる配列
  vector<int> varmap(mNetwork.node_num(), -1);

  // 外部入力用の変数を作る．
  int nvar = 0;
  for ( int i: Range(mNetwork.input_num()) ) {
    auto node = mNetwork.input(i);
    int var = nvar;
    ++ nvar;
    varmap[node->id()] = var;
    mInputVarMap[i] = var;
  }

  // 故障用の変数を作る．
  for ( auto f: mNetwork.rep_fault_list() ) {
    int var = nvar;
    ++ nvar;
    SatVarId var = mSolver.new_variable();
    mFaultVarArray[f->id()] = var;
  }
  for ( auto
}

// @brief デストラクタ
MF_Fsim::~MF_Fsim()
{
}

// @brief テストベクタを追加する．
// @param[in] tv テストベクタ
void
MF_Fsim::add_testvector(const TestVector& tv)
{
  // TpgNetwork のノード番号をキーにして AigHandle を記憶するハッシュ表
  vector<AigHandle> aig_rray(mNetwork.node_num());

  int npi = mNetwork.input_num();
  ASSERT_COND( tv.vector_size() == npi );
  for ( int i: Range(npi) ) {
    auto node = mNetwork.input(i);
    int node_id = node->id();
    switch ( tv.val(i) ) {
    case Val3._0:
      aig_array[node_id] = AigMgr::make_zero();
      break;
    case Val3._1:
      aig_array[node_id] = AIgMgr::make_one();
      break;
    case Val3._X:
      ASSERT_NOT_REACHED;
    }
  }
}

// @brief 今までに追加されたテストベクタで検出されない故障を一つ取り出す．
vector<const TpgFault*>
MF_Fsim::get_undet_fault()
{
}

END_NAMESPACE_DRUID
