
/// @file MF_FaultComp.cc
/// @brief MF_FaultComp の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2019 Yusuke Matsunaga
/// All rights reserved.


#include "MF_FaultComp.h"
#include "MF_Enc.h"
#include "TpgNetwork.h"
#include "TpgNode.h"
#include "TpgFault.h"
#include "ym/Range.h"
#include "ym/SatSolver.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス MF_FaultComp
//////////////////////////////////////////////////////////////////////

BEGIN_NONAMESPACE

void
add_constraint(
  SatSolver& solver,
  const TpgNetwork& network,
  const TpgNode* root,
  const vector<const TpgNode*>& inode_list,
  const vector<pair<const TpgFault*, SatLiteral>>& fault_list,
  const vector<bool>& fvec
)
{
  if ( 0 ) {
    int nf = fault_list.size();
    cout << "fvec: ";
    for ( int i: Range(nf) ) {
      if ( fvec[i] ) {
	cout << "1";
      }
      else {
	cout << "0";
      }
    }
    cout << endl;
  }

  vector<pair<const TpgNode*, SatLiteral>> input_list;
  input_list.reserve(inode_list.size());
  for ( auto node: inode_list ) {
    auto var = solver.new_variable();
    SatLiteral lit{var};
    input_list.push_back(make_pair(node, lit));
  }

  int nf = fault_list.size();

  // fvec で指定された参照用故障回路
  vector<pair<const TpgFault*, SatLiteral>> fault_list1;
  fault_list1.reserve(nf);
  for ( int i: Range(nf) ) {
    auto fault = fault_list[i].first;
    auto var = solver.new_variable();
    SatLiteral lit{var};
    fault_list1.push_back(make_pair(fault, lit));
    if ( fvec[i] ) {
      solver.add_clause(lit);
    }
    else {
      solver.add_clause(~lit);
    }
  }

  auto flit = solver.new_variable();
  MF_Enc::make_faulty_FFR(solver, network, input_list, root, flit, fault_list1);

  // 故障回路
  auto olit = solver.new_variable();
  MF_Enc::make_faulty_FFR(solver, network, input_list, root, olit, fault_list);

  solver.add_notgate(olit, flit);
}

void
add_faults(
  vector<vector<const TpgFault*>>& ans_list,
  const vector<const TpgFault*>& fault_list,
  const vector<bool>& fvec
)
{
  int nf = fault_list.size();
  vector<const TpgFault*> tmp_list;
  tmp_list.reserve(nf);
  for ( int i: Range(nf) ) {
    if ( fvec[i] ) {
      tmp_list.push_back(fault_list[i]);
    }
  }
  ans_list.push_back(std::move(tmp_list));
}

END_NONAMESPACE

// @brief FFR 内の多重故障リストを求める．
// @param[in] network 対象のネットワーク
// @param[in] root FFR の根のノード
// @param[in] input_list FFR の葉のノードのリスト
// @param[in] fault_list FFR内の代表故障のリスト
// @param[in] solver_type SATソルバの実装タイプ
vector<vector<const TpgFault*>>
MF_FaultComp::get_faults_list(const TpgNetwork& network,
			      const TpgNode* root,
			      const vector<const TpgNode*>& input_list,
			      const vector<const TpgFault*>& f_list,
			      SatSolverType solver_type)
{

  SatSolver solver{solver_type};

  vector<pair<const TpgFault*, SatLiteral>> fault_list;
  vector<SatLiteral> tmp_lits;
  SizeType nf = f_list.size();
  fault_list.reserve(nf);
  tmp_lits.reserve(nf);
  for ( auto fault: f_list ) {
    auto var = solver.new_variable(true);
    fault_list.push_back( make_pair(fault, var) );
    tmp_lits.push_back(var);
  }
  // 最低1つは故障を含むという制約
  solver.add_clause(tmp_lits);

  vector<vector<bool>> fvec_list;

  // singleton を加えておく．
  for ( int i: Range(nf) ) {
    vector<bool> fvec(nf, false);
    fvec[i] = true;
    add_constraint(solver, network, root, input_list, fault_list, fvec);
    fvec_list.push_back(fvec);
  }

  cout << "nf = " << nf << endl;
  for ( ; ; ) {
    SatBool3 ans = solver.solve();
    if ( ans == SatBool3::False ) {
      // 新たな代表故障はなかった．
      break;
    }
    if ( ans == SatBool3::X ) {
      // アボート
      cout << "SAT: Abort";
      break;
    }
    // fvec を作る．
    const auto& model = solver.model();
    vector<bool> fvec(nf);
    for ( int i: Range(nf) ) {
      auto fvar = fault_list[i].second;
      SatBool3 val = model[fvar];
      if ( val == SatBool3::True ) {
	fvec[i] = true;
      }
      else if ( val == SatBool3::False ) {
	fvec[i] = false;
      }
      else {
	ASSERT_NOT_REACHED;
      }
    }

    add_constraint(solver, network, root, input_list, fault_list, fvec);
    fvec_list.push_back(fvec);
    cout << ".";
    cout.flush();
  }
  cout << endl;
  cout << " " << fvec_list.size();

  vector<vector<const TpgFault*>> ans_list;
  for ( const auto& fvec: fvec_list ) {
    add_faults(ans_list, f_list, fvec);
  }

  return ans_list;
}

END_NAMESPACE_DRUID
