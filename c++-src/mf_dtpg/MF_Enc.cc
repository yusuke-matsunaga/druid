
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
#include "ym/HashSet.h"
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
	 const vector<SatLiteral>& ilit_list,
	 SatLiteral olit)
{
  int ni = ilit_list.size();
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
    solver.add_eq_rel( ilit_list[0],  olit);
    break;

  case GateType::Not:
    solver.add_neq_rel( ilit_list[0], olit);
    break;

  case GateType::And:
    switch ( ni ) {
    case 2:
      {
	SatLiteral ilit0{ilit_list[0]};
	SatLiteral ilit1{ilit_list[1]};
	solver.add_andgate_rel( olit, ilit0, ilit1);
      }
      break;

    case 3:
      {
	SatLiteral ilit0{ilit_list[0]};
	SatLiteral ilit1{ilit_list[1]};
	SatLiteral ilit2{ilit_list[2]};
	solver.add_andgate_rel( olit, ilit0, ilit1, ilit2);
      }
      break;

    case 4:
      {
	SatLiteral ilit0{ilit_list[0]};
	SatLiteral ilit1{ilit_list[1]};
	SatLiteral ilit2{ilit_list[2]};
	SatLiteral ilit3{ilit_list[3]};
	solver.add_andgate_rel( olit, ilit0, ilit1, ilit2, ilit3);
      }
      break;

    default:
      ASSERT_COND( ni > 4 );
      solver.add_andgate_rel( olit, ilit_list);
      break;
    }
    break;

  case GateType::Nand:
    switch ( ni ) {
    case 2:
      {
	SatLiteral ilit0{ilit_list[0]};
	SatLiteral ilit1{ilit_list[1]};
	solver.add_nandgate_rel( olit, ilit0, ilit1);
      }
      break;

    case 3:
      {
	SatLiteral ilit0{ilit_list[0]};
	SatLiteral ilit1{ilit_list[1]};
	SatLiteral ilit2{ilit_list[2]};
	solver.add_nandgate_rel( olit, ilit0, ilit1, ilit2);
      }
      break;

    case 4:
      {
	SatLiteral ilit0{ilit_list[0]};
	SatLiteral ilit1{ilit_list[1]};
	SatLiteral ilit2{ilit_list[2]};
	SatLiteral ilit3{ilit_list[3]};
	solver.add_nandgate_rel( olit, ilit0, ilit1, ilit2, ilit3);
      }
      break;

    default:
      ASSERT_COND( ni > 4 );
      solver.add_nandgate_rel( olit, ilit_list);
      break;
    }
    break;

  case GateType::Or:
    switch ( ni ) {
    case 2:
      {
	SatLiteral ilit0{ilit_list[0]};
	SatLiteral ilit1{ilit_list[1]};
	solver.add_orgate_rel( olit, ilit0, ilit1);
      }
      break;

    case 3:
      {
	SatLiteral ilit0{ilit_list[0]};
	SatLiteral ilit1{ilit_list[1]};
	SatLiteral ilit2{ilit_list[2]};
	solver.add_orgate_rel( olit, ilit0, ilit1, ilit2);
      }
      break;

    case 4:
      {
	SatLiteral ilit0{ilit_list[0]};
	SatLiteral ilit1{ilit_list[1]};
	SatLiteral ilit2{ilit_list[2]};
	SatLiteral ilit3{ilit_list[3]};
	solver.add_orgate_rel( olit, ilit0, ilit1, ilit2, ilit3);
      }
      break;

    default:
      ASSERT_COND( ni > 4 );
      solver.add_orgate_rel( olit, ilit_list);
      break;
    }
    break;

  case GateType::Nor:
    switch ( ni ) {
    case 2:
      {
	SatLiteral ilit0{ilit_list[0]};
	SatLiteral ilit1{ilit_list[1]};
	solver.add_norgate_rel( olit, ilit0, ilit1);
      }
      break;

    case 3:
      {
	SatLiteral ilit0{ilit_list[0]};
	SatLiteral ilit1{ilit_list[1]};
	SatLiteral ilit2{ilit_list[2]};
	solver.add_norgate_rel( olit, ilit0, ilit1, ilit2);
      }
      break;

    case 4:
      {
	SatLiteral ilit0{ilit_list[0]};
	SatLiteral ilit1{ilit_list[1]};
	SatLiteral ilit2{ilit_list[2]};
	SatLiteral ilit3{ilit_list[3]};
	solver.add_norgate_rel( olit, ilit0, ilit1, ilit2, ilit3);
      }
      break;

    default:
      ASSERT_COND( ni > 4 );
      solver.add_norgate_rel( olit, ilit_list);
      break;
    }
    break;

  case GateType::Xor:
    ASSERT_COND( ni == 2 );
    {
      SatLiteral ilit0{ilit_list[0]};
      SatLiteral ilit1{ilit_list[1]};
      solver.add_xorgate_rel( olit, ilit0, ilit1);
    }
    break;

  case GateType::Xnor:
    ASSERT_COND( ni == 2 );
    {
      SatLiteral ilit0{ilit_list[0]};
      SatLiteral ilit1{ilit_list[1]};
      solver.add_xnorgate_rel( olit, ilit0, ilit1);
    }
    break;

  default:
    ASSERT_NOT_REACHED;
    break;
  }
}

void
good_cnf_dfs(SatSolver& solver,
	     const TpgNode* node,
	     SatLiteral olit,
	     const HashMap<int, SatLiteral>& input_varmap,
	     HashSet<int>& dfs_mark)
{
  if ( dfs_mark.check(node->id()) ) {
    return;
  }
  dfs_mark.add(node->id());

  // ファンインに再帰して入力側のCNF式を作っておく．
  int ni = node->fanin_num();
  vector<SatLiteral> ilit_list(ni);
  for ( int i: Range(ni) ) {
    auto inode = node->fanin(i);
    SatLiteral ilit;
    bool stat = input_varmap.find(inode->id(), ilit);
    if ( !stat ) {
      auto ivar = solver.new_variable();
      ilit = SatLiteral{ivar};
      good_cnf_dfs(solver, inode, ilit, input_varmap, dfs_mark);
    }
    ilit_list[i] = ilit;
  }

  // 実際のゲートの入出力関係を表すCNFを作る．
  gate_enc(solver, node->gate_type(), ilit_list, olit);
}

void
faulty_cnf_dfs(SatSolver& solver,
	       const TpgNetwork& network,
	       const TpgNode* node,
	       SatLiteral olit,
	       const HashMap<int, SatLiteral>& input_varmap,
	       const HashMap<int, SatLiteral>& fault_varmap,
	       HashSet<int>& dfs_mark)
{
  if ( dfs_mark.check(node->id()) ) {
    return;
  }
  dfs_mark.add(node->id());

  // ファンインに再帰して入力側のCNF式を作っておく．
  int ni = node->fanin_num();
  vector<SatLiteral> ilit_list(ni);
  for ( int i: Range(ni) ) {
    auto inode = node->fanin(i);
    SatLiteral ilit;
    bool stat = input_varmap.find(inode->id(), ilit);
    if ( !stat ) {
      auto ivar = solver.new_variable();
      ilit = SatLiteral{ivar};
      faulty_cnf_dfs(solver, network, inode, ilit, input_varmap, fault_varmap, dfs_mark);
    }
    ilit_list[i] = ilit;
  }

  // 故障挿入回路を作る．
  int nf = network.node_rep_fault_num(node->id());
  for ( auto i: Range(nf) ) {
    auto f = network.node_rep_fault(node->id(), i);
    SatLiteral flit;
    bool stat = fault_varmap.find(f->id(), flit);
    ASSERT_COND( stat );
    if ( f->is_branch_fault() ) {
      // ブランチの故障
      int pos = f->tpg_pos();
      SatVarId ovar = solver.new_variable();
      SatLiteral olit{ovar};
      SatLiteral ilit{ilit_list[pos]};
          if ( f->val() == 0 ) {
	// 0縮退故障の挿入回路を追加する．
	solver.add_andgate_rel(olit, ilit, ~flit);
      }
      else {
	// 1縮退故障の挿入回路を追加する．
	solver.add_orgate_rel(olit, ilit, flit);
      }
      // ovar を ilit_list[pos] に置き換える．
      ilit_list[pos] = SatLiteral{ovar};
    }
    else {
      // ステムの故障
      SatVarId tmp_var = solver.new_variable();
      SatLiteral tmp_lit(tmp_var);
      if ( f->val() == 0 ) {
	// 0縮退故障の挿入回路を追加する．
	solver.add_andgate_rel(olit, tmp_lit, ~flit);
      }
      else {
	// 1縮退故障の挿入回路を追加する．
	solver.add_orgate_rel(olit, tmp_lit, flit);
      }
      olit = tmp_lit;
    }
  }

  // 実際のゲートの入出力関係を表すCNFを作る．
  gate_enc(solver, node->gate_type(), ilit_list, olit);
}

END_NONAMESPACE

// @brief 部分回路に対する正常回路を作る．
// @param[in] solver SATソルバ
// @param[in] input_list 入力のノードと対応するSATのリテラルのペアのリスト
// @param[in] output_list 出力のノードと対応するSATのリテラルのペアのリスト
void
MF_Enc::make_good_cnf(SatSolver& solver,
		      const vector<pair<const TpgNode*, SatLiteral>>& input_list,
		      const vector<pair<const TpgNode*, SatLiteral>>& output_list)
{
  // ノード番号をキーにして対応するリテラルを保持するハッシュ表
  HashMap<int, SatLiteral> input_varmap;
  for ( const auto& p: input_list ) {
    auto node = p.first;
    auto lit = p.second;
    input_varmap.add(node->id(), lit);
  }

  HashSet<int> dfs_mark;
  for ( const auto& p: output_list ) {
    auto node = p.first;
    auto lit = p.second;
    good_cnf_dfs(solver, node, lit, input_varmap, dfs_mark);
  }
}

// @brief FFR に対する正常回路を作る．
// @param[in] solver SATソルバ
// @param[in] input_list 入力のノードと対応するSATのリテラルのペアのリスト
// @param[in] onode 出力のノード
// @param[in] olit 出力のノードに対応するSATのリテラル
//
// * input_vars の順番は ffr.input_list() の順番と同じ
void
MF_Enc::make_good_FFR(SatSolver& solver,
		      const vector<pair<const TpgNode*, SatLiteral>>& input_list,
		      const TpgNode* onode,
		      SatLiteral olit)
{
  make_good_cnf(solver, input_list, vector<pair<const TpgNode*, SatLiteral>>{make_pair(onode, olit)});
}

// @brief 部分回路に対する故障回路を作る．
// @param[in] solver SATソルバ
// @param[in] network 対象のネットワーク
// @param[in] input_list 入力のノードと対応するSATのリテラルのペアのリスト
// @param[in] output_list 出力のノードと対応するSATのリテラルのペアのリスト
// @param[in] fault_list 代表故障と対応するSATのリテラルのペアのリスト
void
MF_Enc::make_faulty_cnf(SatSolver& solver,
			const TpgNetwork& network,
			const vector<pair<const TpgNode*, SatLiteral>>& input_list,
			const vector<pair<const TpgNode*, SatLiteral>>& output_list,
			const vector<pair<const TpgFault*, SatLiteral>>& fault_list)
{
  // ノード番号をキーにして対応するリテラルを保持するハッシュ表
  HashMap<int, SatLiteral> input_varmap;
  for ( const auto& p: input_list ) {
    auto node = p.first;
    auto lit = p.second;
    input_varmap.add(node->id(), lit);
  }

  // 故障番号をキーにして対応するリテラルを保持するハッシュ表
  HashMap<int, SatLiteral> fault_varmap;
  for ( const auto& p: fault_list ) {
    auto fault = p.first;
    auto lit = p.second;
    fault_varmap.add(fault->id(), lit);
  }

  HashSet<int> dfs_mark;
  for ( const auto& p: output_list ) {
    auto node = p.first;
    auto lit = p.second;
    faulty_cnf_dfs(solver, network, node, lit, input_varmap, fault_varmap, dfs_mark);
  }
}

// @brief FFR に対する故障回路を作る．
// @param[in] solver SATソルバ
// @param[in] network 対象のネットワーク
// @param[in] input_list 入力のノードと対応するSATのリテラルのペアのリスト
// @param[in] onode 出力のノード
// @param[in] olit 出力のノードに対応するSATのリテラル
// @param[in] fault_list 代表故障と対応するSATのリテラルのペアのリスト
void
MF_Enc::make_faulty_FFR(SatSolver& solver,
			const TpgNetwork& network,
			const vector<pair<const TpgNode*, SatLiteral>>& input_list,
			const TpgNode* onode,
			SatLiteral olit,
			const vector<pair<const TpgFault*, SatLiteral>>& fault_list)
{
  make_faulty_cnf(solver, network, input_list,
		  vector<pair<const TpgNode*, SatLiteral>>{make_pair(onode, olit)},
		  fault_list);
}

END_NAMESPACE_DRUID
