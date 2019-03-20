
/// @file MF_Enc.cc
/// @brief MF_Enc の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2019 Yusuke Matsunaga
/// All rights reserved.


#include "MF_Enc.h"
#include "TpgNode.h"
#include "TpgFault.h"
#include "GateType.h"
#include "ym/HashMap.h"
#include "ym/Range.h"
#include "ym/SatSolver.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス MF_FaultEnd
//////////////////////////////////////////////////////////////////////

BEGIN_NONAMESPACE

void
gate_enc(SatSolver& solver,
	 GateType gate_type,
	 const vector<SatVarId>& ivar_list,
	 SatVarId ovar)
{
  int ni = ivar_list.size();
  SatLiteral olit(ovar);
  switch ( gate_type ) {
  case GateType::Const0:
    solver.add_clause(~olit);
    break;

  case GateType::Const1:
    solver.add_clause( olit);
    break;

  case GateType::Input:
    // なにもしない．
    break;

  case GateType::Buff:
    {
      SatLiteral ilit(ivar_list[0]);
      solver.add_eq_rel( ilit,  olit);
    }
    break;

  case GateType::Not:
    {
      SatLiteral ilit(ivar_list[0]);
      solver.add_neq_rel( ilit, olit);
    }
    break;

  case GateType::And:
    switch ( ni ) {
    case 2:
      {
	SatLiteral ilit0(ivar_list[0]);
	SatLiteral ilit1(ivar_list[1]);
	solver.add_andgate_rel( olit, ilit0, ilit1);
      }
      break;

    case 3:
      {
	SatLiteral ilit0(ivar_list[0]);
	SatLiteral ilit1(ivar_list[1]);
	SatLiteral ilit2(ivar_list[2]);
	solver.add_andgate_rel( olit, ilit0, ilit1, ilit2);
      }
      break;

    case 4:
      {
	SatLiteral ilit0(ivar_list[0]);
	SatLiteral ilit1(ivar_list[1]);
	SatLiteral ilit2(ivar_list[2]);
	SatLiteral ilit3(ivar_list[3]);
	solver.add_andgate_rel( olit, ilit0, ilit1, ilit2, ilit3);
      }
      break;

    default:
      ASSERT_COND( ni > 4 );
      {
	vector<SatLiteral> ilits(ni);
	for (int i = 0; i < ni; ++ i) {
	  ilits[i] = SatLiteral(ivar_list[i]);
	}
	solver.add_andgate_rel( olit, ilits);
      }
      break;
    }
    break;

  case GateType::Nand:
    switch ( ni ) {
    case 2:
      {
	SatLiteral ilit0(ivar_list[0]);
	SatLiteral ilit1(ivar_list[1]);
	solver.add_nandgate_rel( olit, ilit0, ilit1);
      }
      break;

    case 3:
      {
	SatLiteral ilit0(ivar_list[0]);
	SatLiteral ilit1(ivar_list[1]);
	SatLiteral ilit2(ivar_list[2]);
	solver.add_nandgate_rel( olit, ilit0, ilit1, ilit2);
      }
      break;

    case 4:
      {
	SatLiteral ilit0(ivar_list[0]);
	SatLiteral ilit1(ivar_list[1]);
	SatLiteral ilit2(ivar_list[2]);
	SatLiteral ilit3(ivar_list[3]);
	solver.add_nandgate_rel( olit, ilit0, ilit1, ilit2, ilit3);
      }
      break;

    default:
      ASSERT_COND( ni > 4 );
      {
	vector<SatLiteral> ilits(ni);
	for (int i = 0; i < ni; ++ i) {
	  ilits[i] = SatLiteral(ivar_list[i]);
	}
	solver.add_nandgate_rel( olit, ilits);
      }
      break;
    }
    break;

  case GateType::Or:
    switch ( ni ) {
    case 2:
      {
	SatLiteral ilit0(ivar_list[0]);
	SatLiteral ilit1(ivar_list[1]);
	solver.add_orgate_rel( olit, ilit0, ilit1);
      }
      break;

    case 3:
      {
	SatLiteral ilit0(ivar_list[0]);
	SatLiteral ilit1(ivar_list[1]);
	SatLiteral ilit2(ivar_list[2]);
	solver.add_orgate_rel( olit, ilit0, ilit1, ilit2);
      }
      break;

    case 4:
      {
	SatLiteral ilit0(ivar_list[0]);
	SatLiteral ilit1(ivar_list[1]);
	SatLiteral ilit2(ivar_list[2]);
	SatLiteral ilit3(ivar_list[3]);
	solver.add_orgate_rel( olit, ilit0, ilit1, ilit2, ilit3);
      }
      break;

    default:
      ASSERT_COND( ni > 4 );
      {
	vector<SatLiteral> ilits(ni);
	for (int i = 0; i < ni; ++ i) {
	  ilits[i] = SatLiteral(ivar_list[i]);
	}
	solver.add_orgate_rel( olit, ilits);
      }
      break;
    }
    break;

  case GateType::Nor:
    switch ( ni ) {
    case 2:
      {
	SatLiteral ilit0(ivar_list[0]);
	SatLiteral ilit1(ivar_list[1]);
	solver.add_norgate_rel( olit, ilit0, ilit1);
      }
      break;

    case 3:
      {
	SatLiteral ilit0(ivar_list[0]);
	SatLiteral ilit1(ivar_list[1]);
	SatLiteral ilit2(ivar_list[2]);
	solver.add_norgate_rel( olit, ilit0, ilit1, ilit2);
      }
      break;

    case 4:
      {
	SatLiteral ilit0(ivar_list[0]);
	SatLiteral ilit1(ivar_list[1]);
	SatLiteral ilit2(ivar_list[2]);
	SatLiteral ilit3(ivar_list[3]);
	solver.add_norgate_rel( olit, ilit0, ilit1, ilit2, ilit3);
      }
      break;

    default:
      ASSERT_COND( ni > 4 );
      {
	vector<SatLiteral> ilits(ni);
	for (int i = 0; i < ni; ++ i) {
	  ilits[i] = SatLiteral(ivar_list[i]);
	}
	solver.add_norgate_rel( olit, ilits);
      }
      break;
    }
    break;

  case GateType::Xor:
    ASSERT_COND( ni == 2 );
    {
      SatLiteral ilit0(ivar_list[0]);
      SatLiteral ilit1(ivar_list[1]);
      solver.add_xorgate_rel( olit, ilit0, ilit1);
    }
    break;

  case GateType::Xnor:
    ASSERT_COND( ni == 2 );
    {
      SatLiteral ilit0(ivar_list[0]);
      SatLiteral ilit1(ivar_list[1]);
      solver.add_xnorgate_rel( olit, ilit0, ilit1);
    }
    break;

  default:
    ASSERT_NOT_REACHED;
    break;
  }
}

void
good_ffr_dfs(SatSolver& solver,
	     const TpgNode* node,
	     const HashMap<int, SatVarId>& input_varmap,
	     SatVarId output_var)
{
  // ファンインに再帰して入力側のCNF式を作っておく．
  int ni = node->fanin_num();
  vector<SatVarId> ivar_list(ni);
  for ( int i: Range(ni) ) {
    auto inode = node->fanin(i);
    SatVarId ivar;
    bool stat = input_varmap.find(inode->id(), ivar);
    if ( stat ) {
      ivar_list[i] = ivar;
    }
    else {
      ivar = solver.new_variable();
      good_ffr_dfs(solver, inode, input_varmap, ivar);
    }
    ivar_list[i] = ivar;
  }

  // 実際のゲートの入出力関係を表すCNFを作る．
  gate_enc(solver, node->gate_type(), ivar_list, output_var);
}

void
faulty_ffr_dfs(SatSolver& solver,
	       const TpgNetwork& network,
	       const TpgNode* node,
	       const HashMap<int, SatVarId>& input_varmap,
	       SatVarId output_var,
	       const HashMap<int, SatVarId>& fault_varmap)
{
  // ファンインに再帰して入力側のCNF式を作っておく．
  int ni = node->fanin_num();
  vector<SatVarId> ivar_list(ni);
  for ( int i: Range(ni) ) {
    auto inode = node->fanin(i);
    SatVarId ivar;
    bool stat = input_varmap.find(inode->id(), ivar);
    if ( stat ) {
      ivar_list[i] = ivar;
    }
    else {
      ivar = solver.new_variable();
      faulty_ffr_dfs(solver, network, inode, input_varmap, ivar, fault_varmap);
    }
    ivar_list[i] = ivar;
  }

  // 故障挿入回路を作る．
  int nf = network.node_rep_fault_num(node->id());
  for ( auto i: Range(nf) ) {
    auto f = network.node_rep_fault(node->id(), i);
    SatVarId fvar;
    bool stat = fault_varmap.find(f->id(), fvar);
    ASSERT_COND( stat );
    if ( f->is_branch_fault() ) {
      // ブランチの故障
      int pos = f->tpg_pos();
      SatVarId ovar = solver.new_variable();
      SatLiteral olit{ovar};
      SatLiteral ilit{ivar_list[pos]};
      SatLiteral flit(fvar);
      if ( f->val() == 0 ) {
	// 0縮退故障の挿入回路を追加する．
	solver.add_andgate_rel(olit, ilit, ~flit);
      }
      else {
	// 1縮退故障の挿入回路を追加する．
	solver.add_orgate_rel(olit, ilit, flit);
      }
      // ovar を ivar_list[pos] に置き換える．
      ivar_list[pos] = ovar;
    }
    else {
      // ステムの故障
      SatVarId tmp_var = solver.new_variable();
      SatLiteral olit(output_var);
      SatLiteral ilit(tmp_var);
      SatLiteral flit(fvar);
      if ( f->val() == 0 ) {
	// 0縮退故障の挿入回路を追加する．
	solver.add_andgate_rel(olit, ilit, ~flit);
      }
      else {
	// 1縮退故障の挿入回路を追加する．
	solver.add_orgate_rel(olit, ilit, flit);
      }
      output_var = tmp_var;
    }
  }

  // 実際のゲートの入出力関係を表すCNFを作る．
  gate_enc(solver, node->gate_type(), ivar_list, output_var);
}

END_NONAMESPACE

// @brief FFR に対する正常回路を作る．
// @param[in] solver SATソルバ
// @param[in] root FFR の根のノード
// @param[in] input_list FFR の葉のノードのリスト
// @param[in] input_vars FFR の入力に対応する変数のリスト
// @param[in] output_var FFR の出力に対応する変数
//
// * input_vars の順番は ffr.input_list() の順番と同じ
void
MF_Enc::make_good_FFR(SatSolver& solver,
		      const TpgNode* root,
		      const vector<const TpgNode*>& input_list,
		      const vector<SatVarId>& input_vars,
		      SatVarId output_var)
{
  // ノード番号をキーにして対応する変数を保持するハッシュ表
  HashMap<int, SatVarId> input_varmap;

  int ni = input_list.size();
  ASSERT_COND( input_vars.size() == ni );
  for ( auto i: Range(ni) ) {
    auto node = input_list[i];
    auto var = input_vars[i];
    input_varmap.add(node->id(), var);
  }

  good_ffr_dfs(solver, root, input_varmap, output_var);
}

// @brief FFR に対する故障回路を作る．
// @param[in] solver SATソルバ
// @param[in] network 対象のネットワーク
// @param[in] root FFR の根のノード
// @param[in] input_list FFR の葉のノードのリスト
// @param[in] input_vars FFR の入力に対応する変数のリスト
// @param[in] output_var FFR の出力に対応する変数
// @param[in] fault_list FFR内の代表故障のリスト
// @param[in] fault_vars FFR内の代表故障に対応する変数のリスト
//
// * input_vars の順番は input_list の順番と同じ
// * fault_vars の順番は fault_list の順番と同じ
void
MF_Enc::make_faulty_FFR(SatSolver& solver,
			const TpgNetwork& network,
			const TpgNode* root,
			const vector<const TpgNode*>& input_list,
			const vector<SatVarId>& input_vars,
			SatVarId output_var,
			const vector<const TpgFault*>& fault_list,
			const vector<SatVarId>& fault_vars)
{
  // ノード番号をキーにして対応する変数を保持するハッシュ表
  HashMap<int, SatVarId> input_varmap;

  int ni = input_list.size();
  ASSERT_COND( input_vars.size() == ni );
  for ( auto i: Range(ni) ) {
    auto node = input_list[i];
    auto var = input_vars[i];
    input_varmap.add(node->id(), var);
  }

  // 故障番号をキーにして対応する変数を保持するハッシュ表
  HashMap<int, SatVarId> fault_varmap;
  int nf = fault_list.size();
  ASSERT_COND( fault_vars.size() == nf );
  for ( int i: Range(nf) ) {
    auto fault = fault_list[i];
    auto var = fault_vars[i];
    fault_varmap.add(fault->id(), var);
  }

  faulty_ffr_dfs(solver, network, root, input_varmap, output_var, fault_varmap);
}

END_NAMESPACE_DRUID
