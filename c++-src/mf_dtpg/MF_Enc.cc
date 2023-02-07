
/// @file MF_Enc.cc
/// @brief MF_Enc の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2019 Yusuke Matsunaga
/// All rights reserved.

#include "MF_Enc.h"
#include "TpgNode.h"
#include "TpgFault.h"
#include "ym/Range.h"
#include "ym/SatSolver.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス MF_FaultEnd
//////////////////////////////////////////////////////////////////////

BEGIN_NONAMESPACE

void
gate_enc(
  SatSolver& solver,
  PrimType gate_type,
  const vector<SatLiteral>& ilit_list,
  SatLiteral olit
)
{
  SizeType ni = ilit_list.size();
  switch ( gate_type ) {
  case PrimType::None:
    // なにもしない．
    break;

  case PrimType::C0:
    solver.add_clause(~olit);
    break;

  case PrimType::C1:
    solver.add_clause( olit);
    break;


  case PrimType::Buff:
    solver.add_buffgate( ilit_list[0],  olit);
    break;

  case PrimType::Not:
    solver.add_notgate( ilit_list[0], olit);
    break;

  case PrimType::And:
    switch ( ni ) {
    case 2:
      {
	auto ilit0 = ilit_list[0];
	auto ilit1 = ilit_list[1];
	solver.add_andgate( olit, ilit0, ilit1);
      }
      break;

    case 3:
      {
	auto ilit0 = ilit_list[0];
	auto ilit1 = ilit_list[1];
	auto ilit2 = ilit_list[2];
	solver.add_andgate( olit, ilit0, ilit1, ilit2);
      }
      break;

    case 4:
      {
	auto ilit0 = ilit_list[0];
	auto ilit1 = ilit_list[1];
	auto ilit2 = ilit_list[2];
	auto ilit3 = ilit_list[3];
	solver.add_andgate( olit, ilit0, ilit1, ilit2, ilit3);
      }
      break;

    default:
      ASSERT_COND( ni > 4 );
      solver.add_andgate( olit, ilit_list);
      break;
    }
    break;

  case PrimType::Nand:
    switch ( ni ) {
    case 2:
      {
	auto ilit0 = ilit_list[0];
	auto ilit1 = ilit_list[1];
	solver.add_nandgate( olit, ilit0, ilit1);
      }
      break;

    case 3:
      {
	auto ilit0 = ilit_list[0];
	auto ilit1 = ilit_list[1];
	auto ilit2 = ilit_list[2];
	solver.add_nandgate( olit, ilit0, ilit1, ilit2);
      }
      break;

    case 4:
      {
	auto ilit0 = ilit_list[0];
	auto ilit1 = ilit_list[1];
	auto ilit2 = ilit_list[2];
	auto ilit3 = ilit_list[3];
	solver.add_nandgate( olit, ilit0, ilit1, ilit2, ilit3);
      }
      break;

    default:
      ASSERT_COND( ni > 4 );
      solver.add_nandgate( olit, ilit_list);
      break;
    }
    break;

  case PrimType::Or:
    switch ( ni ) {
    case 2:
      {
	auto ilit0 = ilit_list[0];
	auto ilit1 = ilit_list[1];
	solver.add_orgate( olit, ilit0, ilit1);
      }
      break;

    case 3:
      {
	auto ilit0 = ilit_list[0];
	auto ilit1 = ilit_list[1];
	auto ilit2 = ilit_list[2];
	solver.add_orgate( olit, ilit0, ilit1, ilit2);
      }
      break;

    case 4:
      {
	auto ilit0 = ilit_list[0];
	auto ilit1 = ilit_list[1];
	auto ilit2 = ilit_list[2];
	auto ilit3 = ilit_list[3];
	solver.add_orgate( olit, ilit0, ilit1, ilit2, ilit3);
      }
      break;

    default:
      ASSERT_COND( ni > 4 );
      solver.add_orgate( olit, ilit_list);
      break;
    }
    break;

  case PrimType::Nor:
    switch ( ni ) {
    case 2:
      {
	auto ilit0 = ilit_list[0];
	auto ilit1 = ilit_list[1];
	solver.add_norgate( olit, ilit0, ilit1);
      }
      break;

    case 3:
      {
	auto ilit0 = ilit_list[0];
	auto ilit1 = ilit_list[1];
	auto ilit2 = ilit_list[2];
	solver.add_norgate( olit, ilit0, ilit1, ilit2);
      }
      break;

    case 4:
      {
	auto ilit0 = ilit_list[0];
	auto ilit1 = ilit_list[1];
	auto ilit2 = ilit_list[2];
	auto ilit3 = ilit_list[3];
	solver.add_norgate( olit, ilit0, ilit1, ilit2, ilit3);
      }
      break;

    default:
      ASSERT_COND( ni > 4 );
      solver.add_norgate( olit, ilit_list);
      break;
    }
    break;

  case PrimType::Xor:
    ASSERT_COND( ni == 2 );
    {
      auto ilit0 = ilit_list[0];
      auto ilit1 = ilit_list[1];
      solver.add_xorgate( olit, ilit0, ilit1);
    }
    break;

  case PrimType::Xnor:
    ASSERT_COND( ni == 2 );
    {
      auto ilit0 = ilit_list[0];
      auto ilit1 = ilit_list[1];
      solver.add_xnorgate( olit, ilit0, ilit1);
    }
    break;

  default:
    ASSERT_NOT_REACHED;
    break;
  }
}

void
good_cnf_dfs(
  SatSolver& solver,
  const TpgNode* node,
  SatLiteral olit,
  const unordered_map<SizeType, SatLiteral>& input_varmap,
  unordered_set<SizeType>& dfs_mark
)
{
  if ( dfs_mark.count(node->id()) > 0 ) {
    return;
  }
  dfs_mark.emplace(node->id());

  // ファンインに再帰して入力側のCNF式を作っておく．
  SizeType ni = node->fanin_num();
  vector<SatLiteral> ilit_list(ni);
  for ( SizeType i: Range(ni) ) {
    auto inode = node->fanin(i);
    if ( input_varmap.count(inode->id()) == 0 ) {
      auto ilit = solver.new_variable();
      good_cnf_dfs(solver, inode, ilit, input_varmap, dfs_mark);
    }
    auto ilit = input_varmap.at(inode->id());
    ilit_list[i] = ilit;
  }

  // 実際のゲートの入出力関係を表すCNFを作る．
  gate_enc(solver, node->gate_type(), ilit_list, olit);
}

void
faulty_cnf_dfs(
  SatSolver& solver,
  const TpgNetwork& network,
  const TpgNode* node,
  SatLiteral olit,
  const unordered_map<SizeType, SatLiteral>& input_varmap,
  const unordered_map<SizeType, SatLiteral>& fault_varmap,
  unordered_set<SizeType>& dfs_mark
)
{
  if ( dfs_mark.count(node->id()) > 0 ) {
    return;
  }
  dfs_mark.emplace(node->id());

  // ファンインに再帰して入力側のCNF式を作っておく．
  SizeType ni = node->fanin_num();
  vector<SatLiteral> ilit_list(ni);
  for ( SizeType i: Range(ni) ) {
    auto inode = node->fanin(i);
    if ( input_varmap.count(inode->id()) == 0 ) {
      auto ilit = solver.new_variable();
      faulty_cnf_dfs(solver, network, inode, ilit, input_varmap, fault_varmap, dfs_mark);
    }
    SatLiteral ilit = input_varmap.at(inode->id());
    ilit_list[i] = ilit;
  }

  // 故障挿入回路を作る．
  SizeType nf = network.node_rep_fault_num(node->id());
  for ( auto i: Range(nf) ) {
    auto f = network.node_rep_fault(node->id(), i);
    ASSERT_COND( fault_varmap.count(f->id()) > 0 );
    auto flit = fault_varmap.at(f->id());
    if ( f->is_branch_fault() ) {
      // ブランチの故障
      SizeType pos = f->tpg_pos();
      auto olit = solver.new_variable();
      auto ilit = ilit_list[pos];
      switch ( f->val() ) {
      case Fval2::zero:
	// 0縮退故障の挿入回路を追加する．
	solver.add_andgate(olit, ilit, ~flit);
	break;
      case Fval2::one:
	// 1縮退故障の挿入回路を追加する．
	solver.add_orgate(olit, ilit, flit);
	break;
      }
      // ovar を ilit_list[pos] に置き換える．
      ilit_list[pos] = olit;
    }
    else {
      // ステムの故障
      auto tmp_lit = solver.new_variable();
      switch ( f->val() ) {
      case Fval2::zero:
	// 0縮退故障の挿入回路を追加する．
	solver.add_andgate(olit, tmp_lit, ~flit);
	break;
      case Fval2::one:
	// 1縮退故障の挿入回路を追加する．
	solver.add_orgate(olit, tmp_lit, flit);
	break;
      }
      olit = tmp_lit;
    }
  }

  // 実際のゲートの入出力関係を表すCNFを作る．
  gate_enc(solver, node->gate_type(), ilit_list, olit);
}

END_NONAMESPACE

// @brief 部分回路に対する正常回路を作る．
void
MF_Enc::make_good_cnf(
  SatSolver& solver,
  const vector<pair<const TpgNode*, SatLiteral>>& input_list,
  const vector<pair<const TpgNode*, SatLiteral>>& output_list
)
{
  // ノード番号をキーにして対応するリテラルを保持するハッシュ表
  unordered_map<SizeType, SatLiteral> input_varmap;
  for ( const auto& p: input_list ) {
    auto node = p.first;
    auto lit = p.second;
    input_varmap.emplace(node->id(), lit);
  }

  unordered_set<SizeType> dfs_mark;
  for ( const auto& p: output_list ) {
    auto node = p.first;
    auto lit = p.second;
    good_cnf_dfs(solver, node, lit, input_varmap, dfs_mark);
  }
}

// @brief FFR に対する正常回路を作る．
void
MF_Enc::make_good_FFR(
  SatSolver& solver,
  const vector<pair<const TpgNode*, SatLiteral>>& input_list,
  const TpgNode* onode,
  SatLiteral olit
)
{
  make_good_cnf(solver, input_list,
		vector<pair<const TpgNode*, SatLiteral>>{{onode, olit}});
}

// @brief 部分回路に対する故障回路を作る．
void
MF_Enc::make_faulty_cnf(
  SatSolver& solver,
  const TpgNetwork& network,
  const vector<pair<const TpgNode*, SatLiteral>>& input_list,
  const vector<pair<const TpgNode*, SatLiteral>>& output_list,
  const vector<pair<const TpgFault*, SatLiteral>>& fault_list
)
{
  // ノード番号をキーにして対応するリテラルを保持するハッシュ表
  unordered_map<SizeType, SatLiteral> input_varmap;
  for ( const auto& p: input_list ) {
    auto node = p.first;
    auto lit = p.second;
    input_varmap.emplace(node->id(), lit);
  }

  // 故障番号をキーにして対応するリテラルを保持するハッシュ表
  unordered_map<SizeType, SatLiteral> fault_varmap;
  for ( const auto& p: fault_list ) {
    auto fault = p.first;
    auto lit = p.second;
    fault_varmap.emplace(fault->id(), lit);
  }

  unordered_set<SizeType> dfs_mark;
  for ( const auto& p: output_list ) {
    auto node = p.first;
    auto lit = p.second;
    faulty_cnf_dfs(solver, network, node, lit, input_varmap, fault_varmap, dfs_mark);
  }
}

// @brief FFR に対する故障回路を作る．
void
MF_Enc::make_faulty_FFR(
  SatSolver& solver,
  const TpgNetwork& network,
  const vector<pair<const TpgNode*, SatLiteral>>& input_list,
  const TpgNode* onode,
  SatLiteral olit,
  const vector<pair<const TpgFault*, SatLiteral>>& fault_list
)
{
  make_faulty_cnf(solver, network, input_list,
		  vector<pair<const TpgNode*, SatLiteral>>{make_pair(onode, olit)},
		  fault_list);
}

END_NAMESPACE_DRUID
