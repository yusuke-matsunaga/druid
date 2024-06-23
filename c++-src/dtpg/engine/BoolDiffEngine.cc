
/// @file BoolDiffEngine.cc
/// @brief BoolDiffEngine の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "BoolDiffEngine.h"

#include "TpgNetwork.h"
#include "TpgFault.h"
#include "FaultType.h"
#include "GateEnc.h"
#include "NodeTimeValList.h"
#include "TpgNodeSet.h"
#include "extract.h"

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

BEGIN_NONAMESPACE

SatInitParam
get_sat_param(
  const JsonValue& option
)
{
  if ( option.is_object() && option.has_key("sat_param") ) {
    return SatInitParam{option.get("sat_param")};
  }
  return SatInitParam{};
}

JsonValue
get_option(
  const JsonValue& option,
  const char* keyword
)
{
  if ( option.is_object() && option.has_key(keyword) ) {
    return option.get(keyword);
  }
  return JsonValue{};
}

END_NONAMESPACE

// @brief コンストラクタ
BoolDiffEngine::BoolDiffEngine(
  const TpgNetwork& network,
  const TpgNode* root,
  const JsonValue& option
) : mSolver{get_sat_param(option)},
    mNetwork{network},
    mRoot{root},
    mHvarMap{network.node_num()},
    mGvarMap{network.node_num()},
    mFvarMap{network.node_num()},
    mDvarMap{network.node_num()},
    mExOption{get_option(option, "extractor")},
    mJustifier{network, get_option(option, "justifier")}
{
  make_cnf();
}

// @brief CNF の生成を行う．
void
BoolDiffEngine::make_cnf()
{
  Timer timer;
  timer.start();

  // 変数割り当て
  prepare_vars();

  // 正常回路の CNF を生成
  gen_good_cnf();

  // 故障回路の CNF を生成
  gen_faulty_cnf();

  //////////////////////////////////////////////////////////////////////
  // mRoot から外部出力までの故障の伝搬条件を表す変数を作る．
  //////////////////////////////////////////////////////////////////////
  mPropVar = new_variable(true);
  {
    vector<SatLiteral> tmp_lits;
    tmp_lits.reserve(output_list().size() + 1);
    for ( auto node: output_list() ) {
      auto dlit = dvar(node);
      tmp_lits.push_back(dlit);
    }
    solver().add_orgate(mPropVar, tmp_lits);
  }

  // root_node() の dlit が1でなければならない．
  auto dlit0 = dvar(root_node());
  solver().add_clause(dlit0);

  timer.stop();
  mCnfTime = timer.get_time();
}

// @brief 対象の部分回路の関係を表す変数を用意する．
void
BoolDiffEngine::prepare_vars()
{
  if ( debug_dtpg ) {
    DEBUG_OUT << endl;
    DEBUG_OUT << "BoolDiffEngine::prepare_vars() begin" << endl;
    DEBUG_OUT << " Root = " << mRoot->str() << endl;
  }

  // root の TFO を mTfoList に入れる．
  mTfoList = TpgNodeSet::get_tfo_list(mNetwork.node_num(), mRoot,
				      [&](const TpgNode* node) {
					if ( node->is_ppo() ) {
					  mOutputList.push_back(node);
					}
				      });

  bool has_prev_state = mNetwork.fault_type() == FaultType::TransitionDelay;
  // TFO の TFI を mTfiList に入れる．
  // そのうちの DFF の出力に対応するDFFの入力を tmp_list に入れておく．
  mTfiList = TpgNodeSet::get_tfi_list(mNetwork.node_num(), mTfoList,
				      [&](const TpgNode* node) {
					if ( has_prev_state &&
					     node->is_dff_output() ) {
					  mDffInputList.push_back(node->alt_node());
					}
				      });

  if ( has_prev_state ) {
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
    auto gvar = new_variable();
    mGvarMap.set_vid(node, gvar);
    mFvarMap.set_vid(node, gvar);

    if ( debug_dtpg ) {
      DEBUG_OUT << node->str()
		<< ": gvar|fvar = " << gvar << endl;
    }
  }

  // 故障回路の変数を作る．
  for ( auto node: mTfoList ) {
    auto fvar = new_variable();
    auto dvar = new_variable(false);
    mFvarMap.set_vid(node, fvar);
    mDvarMap.set_vid(node, dvar);

    if ( debug_dtpg ) {
      DEBUG_OUT	<< node->id()
		<< ": fvar = " << fvar
		<< ", dvar = " << dvar << endl;
    }
  }

  // 1時刻前の正常回路の変数を作る．
  for ( auto node: mTfi2List ) {
    auto hvar = new_variable();
    mHvarMap.set_vid(node, hvar);

    if ( debug_dtpg ) {
      DEBUG_OUT << node->str()
		<< ": hvar = " << hvar << endl;
    }
  }

  if ( debug_dtpg ) {
    DEBUG_OUT << "BoolDiffEngine::prepare_vars() end" << endl;
  }
}

// @brief 対象の部分回路の正常値の関係を表す CNF 式を作る．
void
BoolDiffEngine::gen_good_cnf()
{
  //////////////////////////////////////////////////////////////////////
  // 正常回路の CNF を生成
  //////////////////////////////////////////////////////////////////////
  GateEnc gval_enc{mSolver, mGvarMap};
  for ( auto node: mTfiList ) {
#if 0
    auto olit = gvar(node);
    if ( olit == SatLiteral::X ) {
      cout << node->str() << ": gvar = X" << endl;
      abort();
    }
    for ( auto inode: node->fanin_list() ) {
      auto ilit = gvar(inode);
      if ( ilit == SatLiteral::X ) {
	cout << inode->str() << ": gvar = X" << endl;
	abort();
      }
    }
#endif
    gval_enc.make_cnf(node);
  }

  GateEnc hval_enc{mSolver, mHvarMap};
  for ( auto node: mTfi2List ) {
#if 0
    if ( hvar(node) == SatLiteral::X ) {
      cout << node->str() << ": hvar = X" << endl;
      abort();
    }
#endif
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
BoolDiffEngine::gen_faulty_cnf()
{
  //////////////////////////////////////////////////////////////////////
  // 故障回路の CNF を生成
  //////////////////////////////////////////////////////////////////////
  GateEnc fval_enc{mSolver, mFvarMap};
  for ( auto node: mTfoList ) {
    if ( node != mRoot ) {
      fval_enc.make_cnf(node);
    }
    make_dchain_cnf(node);
  }
}

// @brief 故障伝搬条件を表すCNF式を生成する．
void
BoolDiffEngine::make_dchain_cnf(
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
    DEBUG_OUT << node->str() << ": dvar(" << dlit << ") -> "
	      << glit << " != " << flit << endl;
  }

  if ( node->is_ppo() ) {
    mSolver.add_clause(~glit,  flit,  dlit);
    mSolver.add_clause( glit, ~flit,  dlit);

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
    int nfo = node->fanout_num();
    if ( nfo == 1 ) {
      auto odlit = mDvarMap(node->fanout(0));
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
	  DEBUG_OUT << node->str() << "dvar(" << dlit << ") -> "
		    << odlit << endl;
	}
      }
    }
  }
}

// @brief 値割り当てをリテラルに変換する．
SatLiteral
BoolDiffEngine::conv_to_literal(
  NodeTimeVal node_val
)
{
  auto node = node_val.node();
  bool inv = !node_val.val(); // 0 の時が inv = true
  auto vid = (node_val.time() == 0) ? hvar(node) : gvar(node);
  return vid * inv;
}

// @brief 値割り当てをリテラルのリストに変換する．
void
BoolDiffEngine::add_to_literal_list(
  const NodeTimeValList& assign_list,
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

// @brief 直前の check() が成功したときの十分条件を求める．
NodeTimeValList
BoolDiffEngine::extract_sufficient_condition(
  const TpgNode* root
)
{
  auto& model = mSolver.model();
  return DRUID_NAMESPACE::extract_sufficient_condition(root, mGvarMap,
						       mFvarMap, model,
						       mExOption);
}

// @brief 与えられた割当の正当化を行う．
NodeTimeValList
BoolDiffEngine::justify(
  const NodeTimeValList& assign_list
)
{
  auto& model = mSolver.model();
  return mJustifier(assign_list, mHvarMap, mGvarMap, model);
}

END_NAMESPACE_DRUID
