
/// @file ConeEnc.cc
/// @brief ConeEnc の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "ConeEnc.h"
#include "ConeInfo.h"
#include "TpgNetwork.h"
#include "VidMap.h"
#include "GateEnc.h"

//#define DEBUG_DTPG

#define DEBUG_OUT cout

BEGIN_NONAMESPACE
#ifdef DEBUG_DTPG
const int debug_dtpg = 1;
#else
const int debug_dtpg = 0;
#endif
END_NONAMESPACE


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// @brief 対象の部分回路の正常値の関係を表す CNF 式を作る．
void
gen_good_cnf(
  SatSolver& solver,
  const ConeInfo& info,
  VidMap& gvar_map,
  VidMap& fvar_map
)
{
  //////////////////////////////////////////////////////////////////////
  // 正常回路の変数を作る．
  //////////////////////////////////////////////////////////////////////
  for ( auto node: info.tfi_list() ) {
    auto gvar = solver.new_variable(true);

    gvar_map.set_vid(node, gvar);
    fvar_map.set_vid(node, gvar);

    if ( debug_dtpg ) {
      DEBUG_OUT << node->str()
		<< ": gvar|fvar = "
		<< gvar << endl;
    }
  }

  //////////////////////////////////////////////////////////////////////
  // 正常回路の CNF を生成
  //////////////////////////////////////////////////////////////////////
  GateEnc gval_enc{solver, gvar_map};
  for ( auto node: info.tfi_list() ) {
    gval_enc.make_cnf(node);
  }
}

// @brief 対象の部分回路の故障値の関係を表す CNF 式を作る．
void
gen_faulty_cnf(
  SatSolver& solver,
  const ConeInfo& info,
  VidMap& fvar_map
)
{
  //////////////////////////////////////////////////////////////////////
  // 故障回路の変数を作る．
  //////////////////////////////////////////////////////////////////////
  for ( auto node: info.tfo_list() ) {
    auto fvar = solver.new_variable(true);
    fvar_map.set_vid(node, fvar);

    if ( debug_dtpg ) {
      DEBUG_OUT	<< node->str()
		<< ": fvar = " << fvar << endl;
    }
  }

  //////////////////////////////////////////////////////////////////////
  // 故障回路の CNF を生成
  //////////////////////////////////////////////////////////////////////
  GateEnc fval_enc{solver, fvar_map};
  for ( auto node: info.tfo_list() ) {
    if ( node != info.root() ) {
      fval_enc.make_cnf(node);
    }
  }
}

// @brief 故障伝搬条件を表すCNF式を生成する．
void
make_dchain_cnf(
  SatSolver& solver,
  const ConeInfo& info,
  VidMap& gvar_map,
  VidMap& fvar_map
)
{
  //////////////////////////////////////////////////////////////////////
  // 伝搬条件用の変数を作る．
  //////////////////////////////////////////////////////////////////////
  VidMap dvar_map{info.network().node_num()};
  for ( auto node: info.tfo_list() ) {
    auto dvar = solver.new_variable(true);
    dvar_map.set_vid(node, dvar);
    if ( debug_dtpg ) {
      DEBUG_OUT << node->str() << ": dvar = " << dvar << endl;
    }
  }

  //////////////////////////////////////////////////////////////////////
  // 伝搬条件の CNF を生成
  //////////////////////////////////////////////////////////////////////
  for ( auto node: info.tfo_list() ) {
    auto glit = gvar_map(node);
    auto flit = fvar_map(node);
    auto dlit = dvar_map(node);

    // dlit -> XOR(glit, flit) を追加する．
    // 要するに正常回路と故障回路で異なっているとき dlit が 1 となる．
    solver.add_clause(~glit, ~flit, ~dlit);
    solver.add_clause( glit,  flit, ~dlit);

    if ( debug_dtpg ) {
      DEBUG_OUT << node->str() << ": dvar(" << dlit << ") -> "
		<< glit << " != " << flit << endl;
    }

    if ( node->is_ppo() ) {
      solver.add_clause(~glit,  flit,  dlit);
      solver.add_clause( glit, ~flit,  dlit);

      if ( debug_dtpg ) {
	DEBUG_OUT << node->str() << ": !dvar(" << dlit << ") -> "
		  << glit << " == " << flit << endl;
      }
    }
    else {
      // dlit -> ファンアウト先のノードの dlit の一つが 1

      if ( debug_dtpg ) {
	DEBUG_OUT << node->str() << "dvar(" << dlit << ") -> ";
      }
      auto nfo = node->fanout_num();
      if ( nfo == 1 ) {
	auto odlit = dvar_map(node->fanout(0));
	solver.add_clause(~dlit, odlit);

	if ( debug_dtpg ) {
	  DEBUG_OUT << odlit << endl;
	}
      }
      else {
	vector<SatLiteral> tmp_lits;
	tmp_lits.reserve(nfo + 1);
	for ( auto onode: node->fanout_list() ) {
	  tmp_lits.push_back(SatLiteral(dvar_map(onode)));

	  if ( debug_dtpg ) {
	    DEBUG_OUT << " " << dvar_map(onode);
	  }
	}

	if ( debug_dtpg ) {
	  DEBUG_OUT << endl;
	}
	tmp_lits.push_back(~dlit);
	solver.add_clause(tmp_lits);

	auto imm_dom = node->imm_dom();
	if ( imm_dom != nullptr ) {
	  auto odlit = dvar_map(imm_dom);
	  solver.add_clause(~dlit, odlit);

	  if ( debug_dtpg ) {
	    DEBUG_OUT << node->str() << "dvar(" << dlit << ") -> "
		      << odlit << endl;
	  }
	}
      }
    }
  }

  //////////////////////////////////////////////////////////////////////
  // 故障の検出条件(正確には root から外部出力までの故障の伝搬条件)
  //////////////////////////////////////////////////////////////////////
  vector<SatLiteral> odiff;
  odiff.reserve(info.ppo_list().size());
  for ( auto node: info.ppo_list() ) {
    auto dlit = dvar_map(node);
    odiff.push_back(dlit);
  }
  solver.add_clause(odiff);

  // root の dlit が1でなければならない．
  auto root = info.root();
  if ( !root->is_ppo() ) {
    auto dlit0 = dvar_map(root);
    solver.add_clause(dlit0);
  }
}

END_NONAMESPACE


// @brief 条件を表す CNF の生成を行う．
void
ConeEnc::encode(
  SatSolver& solver,
  const ConeInfo& info,
  VidMap& gvar_map,
  VidMap& fvar_map
)
{
  // 正常回路の CNF を生成
  gen_good_cnf(solver, info, gvar_map, fvar_map);

  // 故障回路の CNF を生成
  gen_faulty_cnf(solver, info, fvar_map);

  // 故障伝搬条件の CNF を生成
  make_dchain_cnf(solver, info, gvar_map, fvar_map);
}

END_NAMESPACE_DRUID
