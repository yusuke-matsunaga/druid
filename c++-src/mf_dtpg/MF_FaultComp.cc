
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
add_constraint(SatSolver& solver,
	       const TpgNetwork& network,
	       const TpgNode* root,
	       const vector<const TpgNode*>& input_list,
	       const vector<const TpgFault*>& fault_list,
	       const vector<SatVarId>& fault_vars,
	       const vector<bool>& fvec)
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

  int ni = input_list.size();
  vector<SatVarId> input_vars(ni);
  for ( int i: Range(ni) ) {
    auto var = solver.new_variable();
    input_vars[i] = var;
  }

  int nf = fault_list.size();

#if 0
  // 正常回路
  SatVarId gvar = solver.new_variable();
  MF_Enc::make_good_FFR(solver, root, input_list, input_vars, gvar);
#endif

  // fvec で指定された参照用故障回路
  vector<SatVarId> fault_vars1(nf);
  for ( int i: Range(nf) ) {
    auto var = solver.new_variable();
    fault_vars1[i] = var;
    SatLiteral lit(var);
    if ( fvec[i] ) {
      solver.add_clause(lit);
    }
    else {
      solver.add_clause(~lit);
    }
  }

  SatVarId fvar = solver.new_variable();
  MF_Enc::make_faulty_FFR(solver, network, root, input_list, input_vars, fvar,
			  fault_list, fault_vars1);

  // 故障回路
  SatVarId ovar = solver.new_variable();
  MF_Enc::make_faulty_FFR(solver, network, root, input_list, input_vars, ovar,
			  fault_list, fault_vars);

  //  SatLiteral glit(gvar);
  SatLiteral flit(fvar);
  SatLiteral olit(ovar);


#if 0
  // fvar が検出される条件
  solver.add_neq_rel(flit, glit);

  // ovar が検出されない条件
  solver.add_eq_rel(olit, glit);
#else
  solver.add_neq_rel(olit, flit);
#endif
}

void
add_faults(vector<vector<const TpgFault*>>& ans_list,
	   const vector<const TpgFault*>& fault_list,
	   const vector<bool>& fvec)
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
			      const vector<const TpgFault*>& fault_list,
			      SatSolverType solver_type)
{
  int nf = fault_list.size();

  SatSolver solver(solver_type);

  vector<SatVarId> fault_vars(nf);
  vector<SatLiteral> tmp_lits(nf);
  for ( int i: Range(nf) ) {
    auto var = solver.new_variable();
    solver.freeze_literal(SatLiteral{var});
    fault_vars[i] = var;
    tmp_lits[i] = SatLiteral{var};
  }
  // 最低1つは故障を含むという制約
  solver.add_clause(tmp_lits);

  vector<vector<bool>> fvec_list;

  // singleton を加えておく．
  for ( int i: Range(nf) ) {
    vector<bool> fvec(nf, false);
    fvec[i] = true;
    add_constraint(solver, network, root, input_list, fault_list, fault_vars, fvec);
    fvec_list.push_back(fvec);
  }

  for ( ; ; ) {
    vector<SatBool3> model;
    SatBool3 ans = solver.solve(model);
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
    vector<bool> fvec(nf);
    for ( int i: Range(nf) ) {
      SatVarId fvar = fault_vars[i];
      SatBool3 val = model[fvar.val()];
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

    add_constraint(solver, network, root, input_list, fault_list, fault_vars, fvec);
    fvec_list.push_back(fvec);
  }

  vector<vector<const TpgFault*> > ans_list;
  {
    SatSolver solver(solver_type);
    for ( int i: Range(nf) ) {
      auto var = solver.new_variable();
      solver.freeze_literal(SatLiteral{var});
      fault_vars[i] = var;
      tmp_lits[i] = SatLiteral{var};
    }
    // 最低1つは故障を含むという制約
    solver.add_clause(tmp_lits);

    int nv = fvec_list.size();
    for ( int i = nv - 1; i >= 0; -- i ) {
      const auto& fvec = fvec_list[i];
      vector<SatLiteral> assumptions(nf);
      for ( int i: Range(nf) ) {
	SatVarId fvar = fault_vars[i];
	SatLiteral flit{fvar};
	if ( fvec[i] ) {
	  assumptions[i] = flit;
	}
	else {
	  assumptions[i] = ~flit;
	}
      }
      vector<SatBool3> model;
      SatBool3 stat = solver.solve(assumptions, model);
      if ( stat == SatBool3::True ) {
	add_constraint(solver, network, root, input_list, fault_list, fault_vars, fvec);
	add_faults(ans_list, fault_list, fvec);

      }
    }
  }

  return ans_list;
}

END_NAMESPACE_DRUID
