
/// @file DtpgEngine.cc
/// @brief DtpgEngine の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2022, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgEngine.h"

#include "TpgNetwork.h"
#include "TpgFault.h"
#include "TpgDFF.h"
#include "GateEnc.h"
#include "NodeValList.h"
#include "TestVector.h"
#include "TpgNodeSet.h"

#include "ym/SatSolver.h"
#include "ym/SatStats.h"
#include "ym/Range.h"
#include "ym/Timer.h"

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

extern
NodeValList
extract_sufficient_condition(
  const TpgNode* root,
  const VidMap& gvar_map,
  const VidMap& fvar_map,
  const SatModel& model
);

// @brief コンストラクタ
DtpgEngine::DtpgEngine(
  const TpgNetwork& network,
  FaultType fault_type,
  const TpgNode* root,
  bool make_dchain,
  const SatSolverType& solver_type
) : mSolver{solver_type},
    mNetwork{network},
    mFaultType{fault_type},
    mRoot{root},
    mDchain{make_dchain},
    mHvarMap{network.node_num()},
    mGvarMap{network.node_num()},
    mFvarMap{network.node_num()},
    mDvarMap{network.node_num()}
{
}

// @brief CNF の生成を行う．
void
DtpgEngine::make_cnf()
{
  // 変数割り当て
  prepare_vars();

  // 正常回路の CNF を生成
  gen_good_cnf();

  // 故障回路の CNF を生成
  gen_faulty_cnf();

  //////////////////////////////////////////////////////////////////////
  // 故障の検出条件(正確には mRoot から外部出力までの故障の伝搬条件)
  //////////////////////////////////////////////////////////////////////
  if ( mDchain ) {
    vector<SatLiteral> odiff;
    odiff.reserve(output_list().size());
    for ( auto node: output_list() ) {
      auto dlit = dvar(node);
      odiff.push_back(dlit);
    }
    solver().add_clause(odiff);

    if ( !root_node()->is_ppo() ) {
      // root_node() の dlit が1でなければならない．
      auto dlit0 = dvar(root_node());
      solver().add_clause(dlit0);
    }
  }

  opt_make_cnf();
}

// @brief 対象の部分回路の関係を表す変数を用意する．
void
DtpgEngine::prepare_vars()
{
  if ( debug_dtpg ) {
    DEBUG_OUT << endl;
    DEBUG_OUT << "DtpgEngine::prepare_vars() begin" << endl;
    DEBUG_OUT << " Root = " << node_name(mRoot) << endl;
  }

  // root の TFO を mTfoList に入れる．
  mTfoList = TpgNodeSet::get_tfo_list(mNetwork.node_num(), mRoot,
				      [&](const TpgNode* node) {
					if ( node->is_ppo() ) {
					  mOutputList.push_back(node);
					}
				      });

  // TFO の TFI を mTfiList に入れる．
  // そのうちの DFF の出力に対応するDFFの入力を tmp_list に入れておく．
  mTfiList = TpgNodeSet::get_tfi_list(mNetwork.node_num(), mTfoList,
				      [&](const TpgNode* node) {
					if ( mFaultType == FaultType::TransitionDelay ) {
					  if ( node->is_dff_output() ) {
					    mDffInputList.push_back(node->alt_node());
					  }
					}
				      });

  if ( mFaultType == FaultType::TransitionDelay ) {
    auto tmp_list = mDffInputList;
    if ( mRoot->is_dff_output() ) {
      tmp_list.push_back(mRoot->alt_node());
    }
    // mRoot の1時刻前も必要なので tmp_list に入れておく．
    tmp_list.push_back(mRoot);
    // tmp_list の TFI を mTfi2List に入れる．
    mTfi2List = TpgNodeSet::get_tfi_list(mNetwork.node_num(), tmp_list);
  }

  // 正常回路の変数を作る．
  for ( auto node: mTfiList ) {
    auto gvar = mSolver.new_variable(true);

    mGvarMap.set_vid(node, gvar);
    mFvarMap.set_vid(node, gvar);

    if ( debug_dtpg ) {
      DEBUG_OUT << node_name(node)
		<< ": gvar|fvar = "
		<< gvar << endl;
    }
  }

  // 故障回路の変数を作る．
  for ( auto node: mTfoList ) {
    auto fvar = mSolver.new_variable(true);
    mFvarMap.set_vid(node, fvar);

    if ( debug_dtpg ) {
      DEBUG_OUT	<< node_name(node)
		<< ": fvar = " << fvar << endl;
    }

    if ( mDchain ) {
      auto dvar = mSolver.new_variable(true);
      mDvarMap.set_vid(node, dvar);
      if ( debug_dtpg ) {
	DEBUG_OUT << node_name(node) << ": dvar = " << dvar << endl;
      }
    }
  }

  // 1時刻前の正常回路の変数を作る．
  for ( auto node: mTfi2List ) {
    auto hvar = mSolver.new_variable(true);

    mHvarMap.set_vid(node, hvar);

    if ( debug_dtpg ) {
      DEBUG_OUT << node_name(node)
		<< ": hvar = " << hvar
		<< endl;
    }
  }

  if ( debug_dtpg ) {
    DEBUG_OUT << "DtpgEngine::prepare_vars() end" << endl;
  }
}

// @brief 対象の部分回路の正常値の関係を表す CNF 式を作る．
void
DtpgEngine::gen_good_cnf()
{
  //////////////////////////////////////////////////////////////////////
  // 正常回路の CNF を生成
  //////////////////////////////////////////////////////////////////////
  GateEnc gval_enc{mSolver, mGvarMap};
  for ( auto node: mTfiList ) {
    {
      auto olit = gvar(node);
      if ( olit == SatLiteral::X ) {
	cout << node_name(node) << ": gvar = X" << endl;
	abort();
      }
      for ( auto inode: node->fanin_list() ) {
	auto ilit = gvar(inode);
	if ( ilit == SatLiteral::X ) {
	  cout << node_name(inode) << ": gvar = X" << endl;
	  abort();
	}
      }
    }
    gval_enc.make_cnf(node);
  }

  GateEnc hval_enc{mSolver, mHvarMap};
  for ( auto node: mTfi2List ) {
    if ( hvar(node) == SatLiteral::X ) {
      cout << node_name(node) << ": hvar = X" << endl;
      abort();
    }
    hval_enc.make_cnf(node);
  }

  // DFF の入力と出力の値を一致させる．
  for ( auto node: mDffInputList ) {
    auto onode = node->alt_node();
    auto ilit = hvar(node);
    auto olit = gvar(onode);
    mSolver.add_buffgate(olit, ilit);
  }
}

// @brief 対象の部分回路の故障値の関係を表す CNF 式を作る．
void
DtpgEngine::gen_faulty_cnf()
{
  //////////////////////////////////////////////////////////////////////
  // 故障回路の CNF を生成
  //////////////////////////////////////////////////////////////////////
  GateEnc fval_enc{mSolver, mFvarMap};
  for ( auto node: mTfoList ) {
    if ( node != mRoot ) {
      fval_enc.make_cnf(node);
    }
    if ( mDchain ) {
      make_dchain_cnf(node);
    }
  }
}

// @brief make_cnf() の追加処理
void
DtpgEngine::opt_make_cnf()
{
}

// @brief gen_pattern() で用いる検出条件を作る．
vector<SatLiteral>
DtpgEngine::gen_assumptions(
  const TpgFault*
)
{
  return {};
}

// @brief 故障伝搬条件を表すCNF式を生成する．
void
DtpgEngine::make_dchain_cnf(
  const TpgNode* node
)
{
  auto glit = mGvarMap(node);
  auto flit = mFvarMap(node);
  auto dlit = mDvarMap(node);

  // dlit -> XOR(glit, flit) を追加する．
  // 要するに正常回路と故障回路で異なっているとき dlit が 1 となる．
  mSolver.add_clause(~glit, ~flit, ~dlit);
  mSolver.add_clause( glit,  flit, ~dlit);

  if ( debug_dtpg ) {
    DEBUG_OUT << node_name(node) << ": dvar(" << dlit << ") -> "
	      << glit << " != " << flit << endl;
  }

  if ( node->is_ppo() ) {
    mSolver.add_clause(~glit,  flit,  dlit);
    mSolver.add_clause( glit, ~flit,  dlit);

    if ( debug_dtpg ) {
      DEBUG_OUT << node_name(node) << ": !dvar(" << dlit << ") -> "
		<< glit << " == " << flit << endl;
    }
  }
  else {
    // dlit -> ファンアウト先のノードの dlit の一つが 1

    if ( debug_dtpg ) {
      DEBUG_OUT << node_name(node) << "dvar(" << dlit << ") -> ";
    }
    int nfo = node->fanout_num();
    if ( nfo == 1 ) {
      auto odlit = mDvarMap(node->fanout_list()[0]);
      mSolver.add_clause(~dlit, odlit);

      if ( debug_dtpg ) {
	DEBUG_OUT << odlit << endl;
      }
    }
    else {
      vector<SatLiteral> tmp_lits;
      tmp_lits.reserve(nfo + 1);
      for ( auto onode: node->fanout_list() ) {
	tmp_lits.push_back(SatLiteral(mDvarMap(onode)));

	if ( debug_dtpg ) {
	  DEBUG_OUT << " " << mDvarMap(onode);
	}
      }

      if ( debug_dtpg ) {
	DEBUG_OUT << endl;
      }
      tmp_lits.push_back(~dlit);
      mSolver.add_clause(tmp_lits);

      auto imm_dom = node->imm_dom();
      if ( imm_dom != nullptr ) {
	auto odlit = mDvarMap(imm_dom);
	mSolver.add_clause(~dlit, odlit);

	if ( debug_dtpg ) {
	  DEBUG_OUT << node_name(node) << "dvar(" << dlit << ") -> "
		    << odlit << endl;
	}
      }
    }
  }
}

// @brief 値割り当てをリテラルに変換する．
SatLiteral
DtpgEngine::conv_to_literal(
  NodeVal node_val
)
{
  auto node = node_val.node();
  bool inv = !node_val.val(); // 0 の時が inv = true
  auto vid = (node_val.time() == 0) ? hvar(node) : gvar(node);
  return vid * inv;
}

// @brief 値割り当てをリテラルのリストに変換する．
void
DtpgEngine::add_to_literal_list(
  const NodeValList& assign_list,
  vector<SatLiteral>& lit_list
)
{
  SizeType n0 = lit_list.size();
  SizeType n = assign_list.size();
  lit_list.reserve(n + n0);
  for ( auto nv: assign_list ) {
    auto lit = conv_to_literal(nv);
    lit_list.push_back(lit);
  }
}

// @brief テストパタン生成を行う．
SatBool3
DtpgEngine::solve(
  const TpgFault* fault
)
{
  // FFR 内の伝搬条件
  auto ffr_cond = fault->ffr_propagate_condition(fault_type());
  // fault の活性化条件を求める．
  auto assumptions = gen_assumptions(fault);
  add_to_literal_list(ffr_cond, assumptions);
  return mSolver.solve(assumptions);
}

// @brief 十分条件を取り出す．
// @return 十分条件を表す割当リストを返す．
NodeValList
DtpgEngine::get_sufficient_condition(
  const TpgFault* fault
)
{
  // FFR 内の伝搬条件
  auto ffr_cond = fault->ffr_propagate_condition(fault_type());
  // FFR の根の先の伝搬条件
  auto ffr_root = fault->tpg_onode()->ffr_root();
  const auto& model = mSolver.model();
  auto suf_cond = extract_sufficient_condition(ffr_root, mGvarMap, mFvarMap, model);
  suf_cond.merge(ffr_cond);
  return suf_cond;
}

// @brief ノード名を返す．
string
DtpgEngine::node_name(
  const TpgNode* node
)
{
  return mNetwork.node_name(node->id());
}

END_NAMESPACE_DRUID
