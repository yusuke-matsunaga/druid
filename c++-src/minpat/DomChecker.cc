
/// @file DomChecker.cc
/// @brief DomChecker の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "DomChecker.h"

#include "TpgNetwork.h"
#include "TpgFault.h"
#include "TpgNodeSet.h"
#include "GateEnc.h"
#include "Val3.h"
#include "NodeTimeValList.h"

#include "ym/SatSolver.h"
#include "ym/SatStats.h"
#include "ym/Range.h"

//#define DEBUG_DTPG

#define DEBUG_OUT cout
BEGIN_NONAMESPACE
#ifdef DEBUG_DTPG
int debug_dtpg = 1;
#else
const int debug_dtpg = 0;
#endif

END_NONAMESPACE


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
DomChecker::DomChecker(
  const TpgNetwork& network,
  const TpgNode* root,
  const TpgFault* fault,
  const SatInitParam& init_param
) :
  mSolver{init_param},
  mNetwork{network},
  mFault2{fault},
  mRoot1{root},
  mRoot2{fault->origin_node()},
  mHvarMap(network.node_num()),
  mGvarMap(network.node_num()),
  mFvarMap1(network.node_num()),
  mFvarMap2(network.node_num()),
  mDvarMap(network.node_num()),
  mTimerEnable{true}
{
  vector<const TpgNode*> tmp_list;
  mTfoList1 = TpgNodeSet::get_tfo_list(mNetwork.node_num(), mRoot1,
				       [&](const TpgNode* node) {
					 if ( node->is_ppo() ) {
					   mOutputList1.push_back(node);
					 }
					 tmp_list.push_back(node);
				       });
  mTfoList2 = TpgNodeSet::get_tfo_list(mNetwork.node_num(), mRoot2,
				       [&](const TpgNode* node) {
					 if ( node->is_ppo() ) {
					   mOutputList2.push_back(node);
					 }
					 tmp_list.push_back(node);
				       });
  auto has_prev_state = mNetwork.has_prev_state();
  mTfiList = TpgNodeSet::get_tfi_list(mNetwork.node_num(), tmp_list,
				      [&](const TpgNode* node) {
					if ( has_prev_state && node->is_dff_output() ) {
					  auto alt_node = node->alt_node();
					  mDffInputList.push_back(alt_node);
					}
				      });
  if ( has_prev_state ) {
    auto tmp_list = mDffInputList;
    tmp_list.push_back(mRoot1);
    if ( mRoot1->is_dff_output() ) {
      tmp_list.push_back(mRoot1->alt_node());
    }
    tmp_list.push_back(mRoot2);
    if ( mRoot2->is_dff_output() ) {
      tmp_list.push_back(mRoot2->alt_node());
    }
    mPrevTfiList = TpgNodeSet::get_tfi_list(mNetwork.node_num(), tmp_list);
  }

  // 変数割り当て
  prepare_vars();

  // 正常回路の CNF を生成
  gen_good_cnf();

  // 故障回路1の CNF を生成
  gen_faulty_cnf1();

  // 故障回路2の CNF を生成
  gen_faulty_cnf2();
}

// @brief デストラクタ
DomChecker::~DomChecker()
{
}

// @brief チェックする．
SatBool3
DomChecker::check(
  const TpgFault* fault
)
{
  auto ffr_cond = fault->ffr_propagate_condition();

  vector<SatLiteral> assumptions;
  conv_to_assumptions(ffr_cond, assumptions);

  SatBool3 res = solve(assumptions);

  return res;
}

// @brief タイマーをスタートする．
void
DomChecker::cnf_begin()
{
  timer_start();
}

// @brief タイマーを止めて CNF 作成時間に加える．
void
DomChecker::cnf_end()
{
  auto time = timer_stop();
  mStats.update_cnf(time);
}

// @brief 時間計測を開始する．
void
DomChecker::timer_start()
{
  if ( mTimerEnable ) {
    mTimer.reset();
    mTimer.start();
  }
}

/// @brief 時間計測を終了する．
double
DomChecker::timer_stop()
{
  if ( mTimerEnable ) {
    mTimer.stop();
    return mTimer.get_time();
  }
  else {
    return 0.0;
  }
}

// @brief 対象の部分回路の関係を表す変数を用意する．
void
DomChecker::prepare_vars()
{
  // TFI の部分に変数を割り当てる．
  for ( auto node: mTfiList ) {
    auto gvar = mSolver.new_variable(true);

    mGvarMap.set_vid(node, gvar);
    mFvarMap1.set_vid(node, gvar);
    mFvarMap2.set_vid(node, gvar);

    if ( debug_dtpg ) {
      DEBUG_OUT << "gvar(" << node->str() << ") = " << gvar
		<< endl;
    }
  }

  // 故障1の TFO の部分に変数を割り当てる．
  for ( auto node: mTfoList1 ) {
    auto fvar = mSolver.new_variable(true);
    mFvarMap1.set_vid(node, fvar);
    auto dvar = mSolver.new_variable();
    mDvarMap.set_vid(node, dvar);

    if ( debug_dtpg ) {
      DEBUG_OUT << "gvar(" << node->str() << ") = " << gvar(node)
		<< endl;
      DEBUG_OUT << "fvar1("
		<< node->str()
		<< ") = " << fvar << endl;
      DEBUG_OUT << "dvar("
		<< node->str()
		<< ") = " << dvar << endl;
    }
  }

  // 故障2の TFO の部分に変数を割り当てる．
  for ( auto node: mTfoList2 ) {
    auto fvar = mSolver.new_variable(true);
    mFvarMap2.set_vid(node, fvar);

    if ( debug_dtpg ) {
      DEBUG_OUT << "gvar(" << node->str() << ") = " << gvar(node)
		<< endl;
      DEBUG_OUT << "fvar2("
		<< node->str()
		<< ") = " << fvar << endl;
    }
  }

  // prev TFI の部分に変数を割り当てる．
  for ( auto node: mPrevTfiList ) {
    auto hvar = mSolver.new_variable(true);
    mHvarMap.set_vid(node, hvar);

    if ( debug_dtpg ) {
      DEBUG_OUT << "hvar("
		<< node->str()
		<< ") = " << hvar << endl;
    }
  }
}

// @brief 対象の部分回路の正常値の関係を表す CNF 式を作る．
void
DomChecker::gen_good_cnf()
{
  //////////////////////////////////////////////////////////////////////
  // 正常回路の CNF を生成
  //////////////////////////////////////////////////////////////////////
  GateEnc gval_enc(mSolver, mGvarMap);
  for ( auto node: mTfiList ) {
    gval_enc.make_cnf(node);

    if ( debug_dtpg ) {
      DEBUG_OUT << node->str()
		<< ": gvar(" << gvar(node)
		<< ") := " << node->gate_type() << "(";
      for ( auto inode: node->fanin_list() ) {
	DEBUG_OUT << " "
		  << node->str()
		  << ": gvar(" << gvar(inode) << ")";
      }
      DEBUG_OUT << ")" << endl;
    }
  }

  for ( auto inode: mDffInputList ) {
    auto onode = inode->alt_node();
    // DFF の入力の1時刻前の値と出力の値が等しい．
    auto olit = gvar(onode);
    auto ilit = hvar(inode);
    mSolver.add_buffgate(olit, ilit);
  }
  GateEnc hval_enc(mSolver, mHvarMap);
  for ( auto node: mPrevTfiList ) {
    hval_enc.make_cnf(node);

    if ( debug_dtpg ) {
      DEBUG_OUT << node->str()
		<< ": hvar(" << hvar(node)
		<< ") := " << node->gate_type() << "(";
      for ( auto inode: node->fanin_list() ) {
	DEBUG_OUT << " "
		  << inode->str()
		  << ": hvar(" << hvar(inode) << ")";
      }
      DEBUG_OUT << ")" << endl;
    }
  }
}

// @brief 対象の部分回路の故障値の関係を表す CNF 式を作る．
void
DomChecker::gen_faulty_cnf1()
{
  //////////////////////////////////////////////////////////////////////
  // 故障回路1の CNF を生成
  //////////////////////////////////////////////////////////////////////
  GateEnc fval_enc{mSolver, mFvarMap1};
  for ( auto node: mTfoList1 ) {
    if ( node != mRoot1 ) {
      fval_enc.make_cnf(node);

      if ( debug_dtpg ) {
	DEBUG_OUT << node->str()
		  << ": fvar1("
		  << fvar1(node) << ") := "
		  << node->gate_type() << "(";
	for ( auto inode: node->fanin_list() ) {
	  DEBUG_OUT << " "
		    << node->str()
		    << ": fvar1("
		    << fvar1(inode) << ")";
	}
	DEBUG_OUT << ")" << endl;
      }
    }
    make_dchain_cnf(node);
  }
  {
    SizeType no = mOutputList1.size();
    vector<SatLiteral> odiff;
    odiff.reserve(no);
    for ( auto node: mOutputList1 ) {
      auto dlit = dvar(node);
      odiff.push_back(dlit);
    }
    mSolver.add_clause(odiff);
  }
  if ( !mRoot1->is_ppo() ) {
    // mRoot の dlit が1でなければならない．
    mSolver.add_clause(SatLiteral(dvar(mRoot1)));
  }
}

// @brief 対象の部分回路の故障値の関係を表す CNF 式を作る．
void
DomChecker::gen_faulty_cnf2()
{
  //////////////////////////////////////////////////////////////////////
  // 故障回路2の CNF を生成
  //////////////////////////////////////////////////////////////////////
  GateEnc fval_enc{mSolver, mFvarMap2};
  for ( auto node: mTfoList2 ) {
    if ( node != mRoot2 ) {
      fval_enc.make_cnf(node);

      if ( debug_dtpg ) {
	DEBUG_OUT << node->str()
		  << ": fvar2("
		  << fvar2(node) << ") := "
		  << node->gate_type() << "(";
	for ( auto inode: node->fanin_list() ) {
	  DEBUG_OUT << " "
		    << node->str()
		    << ": fvar2("
		    << fvar2(inode) << ")";
	}
	DEBUG_OUT << ")" << endl;
      }
    }
  }

  // こちらは故障の影響が伝搬しない条件を創る．
  for ( auto node: mOutputList2 ) {
    auto glit = gvar(node);
    auto flit = fvar2(node);
    mSolver.add_clause( glit, ~flit);
    mSolver.add_clause(~glit,  flit);
  }
  {
    auto glit = gvar(mRoot2);
    auto flit = fvar2(mRoot2);
    // flit が glit と異なるのは fault->excitation_condition()
    // が成り立っている時．
    auto dlit = new_variable();
    mSolver.add_xorgate(dlit, glit, flit);
    auto ex_cond = mFault2->excitation_condition();
    vector<SatLiteral> tmp_lits;
    tmp_lits.reserve(ex_cond.size() + 1);
    for ( auto nv: ex_cond ) {
      auto lit = conv_to_literal(nv);
      mSolver.add_clause(lit, ~dlit);
      tmp_lits.push_back(~lit);
    }
    tmp_lits.push_back(dlit);
    mSolver.add_clause(tmp_lits);
  }
}

// @brief 故障伝搬条件を表すCNF式を生成する．
void
DomChecker::make_dchain_cnf(
  const TpgNode* node
)
{
  auto glit = mGvarMap(node);
  auto flit = mFvarMap1(node);
  auto dlit = mDvarMap(node);

  // dlit -> XOR(glit, flit) を追加する．
  // 要するに正常回路と故障回路で異なっているとき dlit が 1 となる．
  mSolver.add_clause(~glit, ~flit, ~dlit);
  mSolver.add_clause( glit,  flit, ~dlit);

  if ( debug_dtpg ) {
    DEBUG_OUT << node->str()
	      << ": dvar -> " << glit
	      << " != " << flit << endl;
  }

  if ( node->is_ppo() ) {
    mSolver.add_clause(~glit,  flit,  dlit);
    mSolver.add_clause( glit, ~flit,  dlit);

    if ( debug_dtpg ) {
      DEBUG_OUT << node->str()
		<< ": !dvar -> " << glit
		<< " == " << flit << endl;
    }
  }
  else {
    // dlit -> ファンアウト先のノードの dlit の一つが 1

    if ( debug_dtpg ) {
      DEBUG_OUT << node->str()
		<< ": dvar -> ";
    }
    SizeType nfo = node->fanout_num();
    if ( nfo == 1 ) {
      auto onode = node->fanout_list()[0];
      auto odlit = mDvarMap(onode);
      mSolver.add_clause(~dlit, odlit);

      if ( debug_dtpg ) {
	DEBUG_OUT << node->str()
		  << ": " << odlit << endl;
      }
    }
    else {
      vector<SatLiteral> tmp_lits;
      tmp_lits.reserve(nfo + 1);
      for ( auto onode: node->fanout_list() ) {
	auto dlit1 = mDvarMap(onode);
	tmp_lits.push_back(dlit1);

	if ( debug_dtpg ) {
	  DEBUG_OUT << " "
		    << node->str()
		    << ": " << dlit1;
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
	  DEBUG_OUT << node->id()
		    << ": dvar -> "
		    << imm_dom->str()
		    << ": " << odlit << endl;
	}
      }
    }
  }
}

// @brief 値割り当てをリテラルに変換する．
SatLiteral
DomChecker::conv_to_literal(
  NodeTimeVal node_val
)
{
  auto node = node_val.node();
  bool inv = !node_val.val(); // 0 の時が inv = true
  auto vid = (node_val.time() == 0) ? hvar(node) : gvar(node);
  if ( inv ) {
    vid = ~vid;
  }
  return vid;
}

// @brief 値割り当てをリテラルのリストに変換する．
void
DomChecker::conv_to_assumptions(
  const NodeTimeValList& assign_list,
  vector<SatLiteral>& assumptions
)
{
  SizeType n0 = assumptions.size();
  SizeType n = assign_list.size();
  assumptions.reserve(n + n0);
  for ( auto nv: assign_list ) {
    auto lit = conv_to_literal(nv);
    assumptions.push_back(lit);
  }
}

// @brief 一つの SAT問題を解く．
SatBool3
DomChecker::solve(
  const vector<SatLiteral>& assumptions
)
{
  Timer timer;
  timer.start();

  auto prev_stats = mSolver.get_stats();

  SatBool3 ans = mSolver.solve(assumptions);

  timer.stop();
  auto time = timer.get_time();

  auto sat_stats = mSolver.get_stats();
  //sat_stats -= prev_stats;

  if ( ans == SatBool3::True ) {
    // パタンが求まった．
    mStats.update_det(time, 0.0);
  }
  else if ( ans == SatBool3::False ) {
    // 検出不能と判定された．
    mStats.update_untest(time);
  }
  else {
    // ans == SatBool3::X つまりアボート
    mStats.update_abort(time);
  }

  return ans;
}

END_NAMESPACE_DRUID
