
/// @file DomChecker.cc
/// @brief DomChecker の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.

#include "DomChecker.h"

#include "TpgNetwork.h"
#include "TpgFault.h"
#include "TpgDFF.h"
#include "GateEnc.h"
#include "FaultyGateEnc.h"
#include "Val3.h"
#include "NodeValList.h"

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
// @param[in] network 対象のネットワーク
// @param[in] fault_type 故障の種類
// @param[in] root 故障伝搬の起点となるノード
// @param[in] fault 故障伝搬をさせない故障
// @param[in] solver_type SATソルバの実装タイプ
DomChecker::DomChecker(const TpgNetwork& network,
		       FaultType fault_type,
		       const TpgNode* root,
		       const TpgFault* fault,
		       const SatSolverType& solver_type) :
  mSolver(solver_type),
  mNetwork(network),
  mFaultType(fault_type),
  mFault(fault),
  mMarkArray(mNetwork.node_num(), 0U),
  mHvarMap(network.node_num()),
  mGvarMap(network.node_num()),
  mTimerEnable(true)
{
  mRoot[0] = root;
  mRoot[1] = fault->tpg_onode();
  mTfiList.reserve(network.node_num());
  mPrevTfiList.reserve(network.node_num());
  for ( int pos: { 0, 1 } ) {
    mTfoList[pos].reserve(network.node_num());
    mOutputList[pos].reserve(network.ppo_num());
    mFvarMap[pos].init(network.node_num());
    mDvarMap.init(network.node_num());
  }

  // 変数割り当て
  prepare_vars();

  // 正常回路の CNF を生成
  gen_good_cnf();

  // 故障回路の CNF を生成
  gen_faulty_cnf();

  //////////////////////////////////////////////////////////////////////
  // 故障の検出条件(正確には mRoot から外部出力までの故障の伝搬条件)
  //////////////////////////////////////////////////////////////////////
  {
    int no = mOutputList[0].size();
    vector<SatLiteral> odiff;
    odiff.reserve(no);
    for ( auto node: mOutputList[0] ) {
      SatLiteral dlit(dvar(node));
      odiff.push_back(dlit);
    }
    mSolver.add_clause(odiff);
  }
  if ( !mRoot[0]->is_ppo() ) {
    // mRoot の dlit が1でなければならない．
    mSolver.add_clause(SatLiteral(dvar(mRoot[0])));
  }

  //////////////////////////////////////////////////////////////////////
  // 故障の非検出条件(正確には mRoot から外部出力までの故障の伝搬条件)
  //////////////////////////////////////////////////////////////////////
  for ( auto node: mOutputList[1] ) {
    SatLiteral glit(gvar(node));
    SatLiteral flit(fvar(node, 1));
    mSolver.add_clause( glit, ~flit);
    mSolver.add_clause(~glit,  flit);
  }
}

// @brief デストラクタ
DomChecker::~DomChecker()
{
}

// @brief テスト生成を行なう．
// @param[in] fault 対象の故障
// @return 結果を返す．
SatBool3
DomChecker::check_detectable(const TpgFault* fault)
{
  NodeValList ffr_cond = fault->ffr_propagate_condition(mFaultType);

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
  mStats.mCnfGenTime += time;
  ++ mStats.mCnfGenCount;
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
  for ( int pos: { 0, 1 } ) {
    // root[pos] の TFO を mTfoList[pos] に入れる．
    set_tfo_mark(mRoot[pos], pos);
    for ( int rpos = 0; rpos < mTfoList[pos].size(); ++ rpos ) {
      const TpgNode* node = mTfoList[pos][rpos];
      for ( auto onode: node->fanout_list() ) {
	set_tfo_mark(onode, pos);
      }
    }
  }

  for ( int rpos = 0; rpos < mTfiList.size(); ++ rpos ) {
    const TpgNode* node = mTfiList[rpos];
    for ( auto inode: node->fanin_list() ) {
      set_tfi_mark(inode);
    }
  }

  // TFI に含まれる DFF のさらに TFI を mTfi2List に入れる．
  if ( mFaultType == FaultType::TransitionDelay ) {
    for ( int pos: { 0, 1 } ) {
      if ( mRoot[pos]->is_dff_output() ) {
	mDffInputList.push_back(mRoot[pos]->alt_node());
      }
    }
    for ( auto node: mDffInputList ) {
      mPrevTfiList.push_back(node);
    }
    set_prev_tfi_mark(mRoot[0]);
    set_prev_tfi_mark(mRoot[1]);
    for ( int rpos = 0; rpos < mPrevTfiList.size(); ++ rpos) {
      const TpgNode* node = mPrevTfiList[rpos];
      for ( auto inode: node->fanin_list() ) {
	set_prev_tfi_mark(inode);
      }
    }
  }

  // TFI の部分に変数を割り当てる．
  for ( auto node: mTfiList ) {
    auto gvar = mSolver.new_variable(true);

    mGvarMap.set_vid(node, gvar);
    mFvarMap[0].set_vid(node, gvar);
    mFvarMap[1].set_vid(node, gvar);

    if ( debug_dtpg ) {
      DEBUG_OUT << "gvar(";
      print_node(DEBUG_OUT, mNetwork, node);
      DEBUG_OUT << ") = " << gvar << endl;
    }
  }

  for ( int pos: { 0, 1 } ) {
    // TFO の部分に変数を割り当てる．
    for ( auto node: mTfoList[pos] ) {
      auto fvar = mSolver.new_variable(true);

      mFvarMap[pos].set_vid(node, fvar);
      if ( pos == 0 ) {
	auto dvar = mSolver.new_variable();
	mDvarMap.set_vid(node, dvar);
      }

      if ( debug_dtpg ) {
	DEBUG_OUT << "gvar(";
	print_node(DEBUG_OUT, mNetwork, node);
	DEBUG_OUT << ") = " << gvar(node) << endl;
	DEBUG_OUT << "fvar[" << pos << "](";
	print_node(DEBUG_OUT, mNetwork, node);
	DEBUG_OUT << ") = " << fvar << endl;
	if ( pos == 0 ) {
	  DEBUG_OUT << "dvar(";
	  print_node(DEBUG_OUT, mNetwork, node);
	  DEBUG_OUT << ") = " << dvar(node) << endl;
	}
      }
    }
  }

  // prev TFI の部分に変数を割り当てる．
  for ( auto node: mPrevTfiList ) {
    auto hvar = mSolver.new_variable(true);
    mHvarMap.set_vid(node, hvar);

    if ( debug_dtpg ) {
      DEBUG_OUT << "hvar(";
      print_node(DEBUG_OUT, mNetwork, node);
      DEBUG_OUT << ") = " << hvar << endl;
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
      print_node(DEBUG_OUT, mNetwork, node);
      DEBUG_OUT << ": gvar(" << gvar(node) << ") := " << node->gate_type() << "(";
      for ( auto inode: node->fanin_list() ) {
	DEBUG_OUT << " ";
	print_node(DEBUG_OUT, mNetwork, inode);
	DEBUG_OUT << ": gvar(" << gvar(inode) << ")";
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
      print_node(DEBUG_OUT, mNetwork, node);
      DEBUG_OUT << ": hvar(" << hvar(node) << ") := " << node->gate_type() << "(";
      for ( auto inode: node->fanin_list() ) {
	DEBUG_OUT << " ";
	print_node(DEBUG_OUT, mNetwork, inode);
	DEBUG_OUT << ": hvar(" << hvar(inode) << ")";
      }
      DEBUG_OUT << ")" << endl;
    }
  }
}

// @brief 対象の部分回路の故障値の関係を表す CNF 式を作る．
void
DomChecker::gen_faulty_cnf()
{
  //////////////////////////////////////////////////////////////////////
  // 故障回路の CNF を生成
  //////////////////////////////////////////////////////////////////////
  FaultyGateEnc fenc2(mSolver, mFvarMap[1], mFault);
  fenc2.make_cnf();

  for ( int pos: { 0, 1 } ) {
    GateEnc fval_enc(mSolver, mFvarMap[pos]);
    for ( auto node: mTfoList[pos] ) {
      if ( node != mRoot[pos] ) {
	fval_enc.make_cnf(node);

	if ( debug_dtpg ) {
	  print_node(DEBUG_OUT, mNetwork, node);
	  DEBUG_OUT << ": fvar[" << pos << "](" << fvar(node, pos) << ") := "
		    << node->gate_type() << "(";
	  for ( auto inode: node->fanin_list() ) {
	    DEBUG_OUT << " ";
	    print_node(DEBUG_OUT, mNetwork, node);
	    DEBUG_OUT << ": fvar[" << pos << "](" << fvar(inode, pos) << ")";
	  }
	  DEBUG_OUT << ")" << endl;
	}
      }
      if ( pos == 0 ) {
	make_dchain_cnf(node);
      }
    }
  }
}

// @brief 故障伝搬条件を表すCNF式を生成する．
// @param[in] node 対象のノード
void
DomChecker::make_dchain_cnf(const TpgNode* node)
{
  SatLiteral glit(mGvarMap(node));
  SatLiteral flit(mFvarMap[0](node));
  SatLiteral dlit(mDvarMap(node));

  // dlit -> XOR(glit, flit) を追加する．
  // 要するに正常回路と故障回路で異なっているとき dlit が 1 となる．
  mSolver.add_clause(~glit, ~flit, ~dlit);
  mSolver.add_clause( glit,  flit, ~dlit);

  if ( debug_dtpg ) {
    print_node(DEBUG_OUT, mNetwork, node);
    DEBUG_OUT << ": dvar -> " << glit << " != " << flit << endl;
  }

  if ( node->is_ppo() ) {
    mSolver.add_clause(~glit,  flit,  dlit);
    mSolver.add_clause( glit, ~flit,  dlit);

    if ( debug_dtpg ) {
      print_node(DEBUG_OUT, mNetwork, node);
      DEBUG_OUT << ": !dvar -> " << glit << " == " << flit << endl;
    }
  }
  else {
    // dlit -> ファンアウト先のノードの dlit の一つが 1

    if ( debug_dtpg ) {
      print_node(DEBUG_OUT, mNetwork, node);
      DEBUG_OUT << ": dvar -> ";
    }
    int nfo = node->fanout_num();
    if ( nfo == 1 ) {
      auto onode = node->fanout_list()[0];
      SatLiteral odlit(mDvarMap(onode));
      mSolver.add_clause(~dlit, odlit);

      if ( debug_dtpg ) {
	print_node(DEBUG_OUT, mNetwork, onode);
	DEBUG_OUT << ": " << odlit << endl;
      }
    }
    else {
      vector<SatLiteral> tmp_lits;
      tmp_lits.reserve(nfo + 1);
      for ( auto onode: node->fanout_list() ) {
	SatLiteral dlit1(mDvarMap(onode));
	tmp_lits.push_back(dlit1);

	if ( debug_dtpg ) {
	  DEBUG_OUT << " ";
	  print_node(DEBUG_OUT, mNetwork, onode);
	  DEBUG_OUT << ": " << dlit1;
	}
      }

      if ( debug_dtpg ) {
	DEBUG_OUT << endl;
      }
      tmp_lits.push_back(~dlit);
      mSolver.add_clause(tmp_lits);

      const TpgNode* imm_dom = node->imm_dom();
      if ( imm_dom != nullptr ) {
	SatLiteral odlit(mDvarMap(imm_dom));
	mSolver.add_clause(~dlit, odlit);

	if ( debug_dtpg ) {
	  print_node(DEBUG_OUT, mNetwork, node);
	  DEBUG_OUT << ": dvar -> ";
	  print_node(DEBUG_OUT, mNetwork, imm_dom);
	  DEBUG_OUT << ": " << odlit << endl;
	}
      }
    }
  }
}

// @brief 値割り当てをリテラルに変換する．
SatLiteral
DomChecker::conv_to_literal(NodeVal node_val)
{
  const TpgNode* node = node_val.node();
  bool inv = !node_val.val(); // 0 の時が inv = true
  auto vid = (node_val.time() == 0) ? hvar(node) : gvar(node);
  if ( inv ) {
    vid = ~vid;
  }
  return vid;
}

// @brief 値割り当てをリテラルのリストに変換する．
// @param[in] assign_list 値の割り当てリスト
// @param[out] assumptions 変換したリテラルを追加するリスト
void
DomChecker::conv_to_assumptions(const NodeValList& assign_list,
				vector<SatLiteral>& assumptions)
{
  int n0 = assumptions.size();
  int n = assign_list.size();
  assumptions.reserve(n + n0);
  for ( auto nv: assign_list ) {
    auto lit = conv_to_literal(nv);
    assumptions.push_back(lit);
  }
}

// @brief 一つの SAT問題を解く．
// @param[in] assumptions 値の決まっている変数のリスト
// @param[out] model SAT モデル
// @return 結果を返す．
SatBool3
DomChecker::solve(const vector<SatLiteral>& assumptions)
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
    mStats.update_det(sat_stats, time);
  }
  else if ( ans == SatBool3::False ) {
    // 検出不能と判定された．
    mStats.update_red(sat_stats, time);
  }
  else {
    // ans == SatBool3::X つまりアボート
    mStats.update_abort(sat_stats, time);
  }

  return ans;
}

END_NAMESPACE_DRUID
