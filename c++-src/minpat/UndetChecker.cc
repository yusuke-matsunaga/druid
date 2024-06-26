﻿
/// @file UndetChecker.cc
/// @brief UndetChecker の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.

#include "UndetChecker.h"

#include "TpgNetwork.h"
#include "TpgFault.h"
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
UndetChecker::UndetChecker(
  const TpgNetwork& network,
  const TpgFault* fault,
  const SatInitParam& init_param
) : mSolver{init_param},
    mNetwork{network},
    mFault{fault},
    mMarkArray(mNetwork.node_num(), 0U),
    mHvarMap(network.node_num()),
    mGvarMap(network.node_num()),
    mFvarMap(network.node_num()),
    mGvalEnc{mSolver, mGvarMap},
    mHvalEnc{mSolver, mHvarMap},
    mFvalEnc{mSolver, mFvarMap},
    mTimerEnable{true}
{
  mRoot = fault->origin_node();
  mTfiList.reserve(network.node_num());
  mPrevTfiList.reserve(network.node_num());
  mTfoList.reserve(network.node_num());
  mOutputList.reserve(network.ppo_num());

  // 変数割り当て
  prepare_vars();

  // 正常回路の CNF を生成
  gen_good_cnf();

  // 故障回路の CNF を生成
  gen_faulty_cnf();

  //////////////////////////////////////////////////////////////////////
  // 故障の非検出条件
  // 全ての出力の正常値と故障値が等しい
  //////////////////////////////////////////////////////////////////////
  for ( auto node: mOutputList ) {
    SatLiteral glit(gvar(node));
    SatLiteral flit(fvar(node));
    mSolver.add_clause( glit, ~flit);
    mSolver.add_clause(~glit,  flit);
  }
}

// @brief デストラクタ
UndetChecker::~UndetChecker()
{
}

// @brief テスト生成を行なう．
SatBool3
UndetChecker::check(
  const NodeTimeValList& cond
)
{
  vector<SatLiteral> assumptions;
  if ( !conv_to_assumptions(cond, assumptions) ) {
    return SatBool3::X;
  }

  SatBool3 res = solve(assumptions);

  return res;
}

// @brief タイマーをスタートする．
void
UndetChecker::cnf_begin()
{
  timer_start();
}

// @brief タイマーを止めて CNF 作成時間に加える．
void
UndetChecker::cnf_end()
{
  auto time = timer_stop();
  mStats.update_cnf(time);
}

// @brief 時間計測を開始する．
void
UndetChecker::timer_start()
{
  if ( mTimerEnable ) {
    mTimer.reset();
    mTimer.start();
  }
}

/// @brief 時間計測を終了する．
double
UndetChecker::timer_stop()
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
UndetChecker::prepare_vars()
{
  // root[pos] の TFO を mTfoList に入れる．
  set_tfo_mark(mRoot);
  for ( int rpos = 0; rpos < mTfoList.size(); ++ rpos ) {
    const TpgNode* node = mTfoList[rpos];
    for ( auto onode: node->fanout_list() ) {
      set_tfo_mark(onode);
    }
  }

  for ( int rpos = 0; rpos < mTfiList.size(); ++ rpos ) {
    const TpgNode* node = mTfiList[rpos];
    for ( auto inode: node->fanin_list() ) {
      set_tfi_mark(inode);
    }
  }

  // TFI に含まれる DFF のさらに TFI を mPrevTfiList に入れる．
  if ( mFaultType == FaultType::TransitionDelay ) {
    if ( mRoot->is_dff_output() ) {
      mDffInputList.push_back(mRoot->alt_node());
    }
    for ( auto node: mDffInputList ) {
      mPrevTfiList.push_back(node);
    }
    set_prev_tfi_mark(mRoot);
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

    set_gvar(node, gvar);

    if ( debug_dtpg ) {
      DEBUG_OUT << "gvar(Node#" << node->id() << ") = " << gvar << endl;
    }
  }

  // TFO の部分に変数を割り当てる．
  for ( auto node: mTfoList ) {
    auto gvar = mSolver.new_variable(true);
    auto fvar = mSolver.new_variable(true);

    set_gvar(node, gvar);
    set_fvar(node, fvar);

    if ( debug_dtpg ) {
      DEBUG_OUT << "gvar(Node#" << node->id() << ") = " << mGvarMap(node) << endl
		<< "fvar(Node#" << node->id() << ") = " << fvar << endl;
    }
  }


  // prev TFI の部分に変数を割り当てる．
  for ( auto node: mPrevTfiList ) {
    auto hvar = mSolver.new_variable(true);

    set_hvar(node, hvar);

    if ( debug_dtpg ) {
      DEBUG_OUT << "hvar(Node#" << node->id() << ") = " << hvar << endl;
    }
  }
}

// @brief 対象の部分回路の正常値の関係を表す CNF 式を作る．
void
UndetChecker::gen_good_cnf()
{
  //////////////////////////////////////////////////////////////////////
  // 正常回路の CNF を生成
  //////////////////////////////////////////////////////////////////////
  for ( auto node: mTfiList ) {
    mGvalEnc.make_cnf(node);

    if ( debug_dtpg ) {
      DEBUG_OUT << "Node#" << node->id() << ": gvar("
		<< gvar(node) << ") := " << node->gate_type()
		<< "(";
      for ( auto inode: node->fanin_list() ) {
	DEBUG_OUT << " " << gvar(inode);
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

  for ( auto node: mPrevTfiList ) {
    mHvalEnc.make_cnf(node);

    if ( debug_dtpg ) {
      DEBUG_OUT << "Node#" << node->id() << ": hvar("
		<< hvar(node) << ") := " << node->gate_type()
		<< "(";
      for ( auto inode: node->fanin_list() ) {
	DEBUG_OUT << " " << hvar(inode);
      }
      DEBUG_OUT << ")" << endl;
    }
  }
}

// @brief 対象の部分回路の故障値の関係を表す CNF 式を作る．
void
UndetChecker::gen_faulty_cnf()
{
  //////////////////////////////////////////////////////////////////////
  // 故障回路の CNF を生成
  //////////////////////////////////////////////////////////////////////
  GateEnc fval_enc{mSolver, mFvarMap};
  for ( auto node: mTfoList ) {
    if ( node != mRoot ) {
      fval_enc.make_cnf(node);
      if ( debug_dtpg ) {
	DEBUG_OUT << "Node#" << node->id() << ": fvar("
		  << fvar(node) << ") := " << node->gate_type()
		  << "(";
	for ( auto inode: node->fanin_list() ) {
	  DEBUG_OUT << " " << fvar(inode);
	}

	DEBUG_OUT << ")" << endl;
      }
    }
  }
}

// @brief 値割り当てをリテラルに変換する．
SatLiteral
UndetChecker::conv_to_literal(
  NodeTimeVal node_val
)
{
  const TpgNode* node = node_val.node();
  bool inv = !node_val.val(); // 0 の時が inv = true
  SatLiteral vid;
  if ( node_val.time() == 0 ) {
    if ( !has_hvar(node) ) {
      make_prev_cnf(node);
    }
    vid = hvar(node);
  }
  else {
    if ( !has_gvar(node) ) {
      make_good_cnf(node);
    }
    vid = gvar(node);
  }
  if ( inv ) {
    vid = ~vid;
  }
  return vid;
}

// @brief 値割り当てをリテラルのリストに変換する．
bool
UndetChecker::conv_to_assumptions(
  const NodeTimeValList& assign_list,
  vector<SatLiteral>& assumptions
)
{
  int n0 = assumptions.size();
  int n = assign_list.size();
  assumptions.reserve(n + n0);
  for ( auto nv: assign_list ) {
    auto lit = conv_to_literal(nv);
    if ( lit != SatLiteral::X ) {
      assumptions.push_back(lit);
    }
#if 0
    else {
      return false;
    }
#endif
  }
  return true;
}

// @brief 一つの SAT問題を解く．
SatBool3
UndetChecker::solve(
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

// @brief 直前の solve() の解を返す．
const SatModel&
UndetChecker::model()
{
  return mSolver.model();
}

// @brief 正常回路の CNF を作る．
void
UndetChecker::make_good_cnf(const TpgNode* node)
{
  if ( has_gvar(node) ) {
    return;
  }
  auto var = mSolver.new_variable(true);
  set_gvar(node, var);

  for ( auto inode: node->fanin_list() ) {
    make_good_cnf(inode);
  }

  mGvalEnc.make_cnf(node);
}

// @brief 1時刻前の正常回路の CNF を作る．
void
UndetChecker::make_prev_cnf(
  const TpgNode* node
)
{
  if ( has_hvar(node) ) {
    return;
  }
  auto var = mSolver.new_variable(true);
  set_hvar(node, var);

  for ( auto inode: node->fanin_list() ) {
    make_prev_cnf(inode);
  }

  mHvalEnc.make_cnf(node);
}

END_NAMESPACE_DRUID
