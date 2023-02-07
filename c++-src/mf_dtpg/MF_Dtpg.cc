
/// @file MF_Dtpg.cc
/// @brief MF_Dtpg の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2019, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "MF_Dtpg.h"
#include "Extractor.h"
#include "TpgDff.h"
#include "TpgFault.h"
#include "GateEnc.h"
#include "FaultyGateEnc.h"
#include "NodeValList.h"

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

//////////////////////////////////////////////////////////////////////
// クラス MF_Dtpg
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
MF_Dtpg::MF_Dtpg(
  const TpgNetwork& network,
  FaultType fault_type,
  const string& just_type,
  const SatSolverType& solver_type
) : mSolver{solver_type},
    mNetwork(network),
    mFaultType(fault_type),
    mMarkArray(network.node_num(), 0U),
    mHvarMap(network.node_num()),
    mGvarMap(network.node_num()),
    mFvarMap(network.node_num()),
    mDvarMap(network.node_num()),
    mJustifier(just_type, network),
    mTimerEnable(true)
{
  mTfoList.reserve(network.node_num());
  mTfiList.reserve(network.node_num());
  mTfi2List.reserve(network.node_num());
  mOutputList.reserve(network.ppo_num());
}

// @brief デストラクタ
MF_Dtpg::~MF_Dtpg()
{
}

// @brief テスト生成を行なう．
DtpgResult
MF_Dtpg::gen_pattern(
  const vector<const TpgFault*>& fault_list
)
{
  mFaultList = fault_list;
  mRootList.clear();
  mRootList.reserve(fault_list.size());
  for ( auto fault: fault_list ) {
    auto root = fault->tpg_onode();
    set_root_mark(root);
    mRootList.push_back(root);
  }

  cnf_begin();

  // 変数割り当て
  prepare_vars();

  // 正常回路の CNF を生成
  gen_good_cnf();

  // 故障回路の CNF を生成
  gen_faulty_cnf();

  //////////////////////////////////////////////////////////////////////
  // 故障の検出条件(正確には mRootNodeList から外部出力までの故障の伝搬条件)
  //////////////////////////////////////////////////////////////////////
  {
    // 外部出力のうち最低１つは dlit が１でなければならない．
    vector<SatLiteral> odiff;
    odiff.reserve(output_list().size());
    for ( auto node: output_list() ) {
      SatLiteral dlit(dvar(node));
      odiff.push_back(dlit);
    }
    solver().add_clause(odiff);
  }

  if ( mFaultType == FaultType::StuckAt ) {
    // root のうち最低１つは dlit が1でなければならない．
    vector<SatLiteral> tmp_lits;
    tmp_lits.reserve(root_node_list().size());
    for ( auto root: root_node_list() ) {
      SatLiteral dlit(dvar(root));
      tmp_lits.push_back(dlit);
    }
    solver().add_clause(tmp_lits);
  }
  else {
    // 遷移故障の場合はさらに故障ノードの値が１時刻前と値が異なっていなければならない．
    vector<SatLiteral> tmp_lits;
    tmp_lits.reserve(root_node_list().size());
    for ( auto fault: mFaultList ) {
      auto root = fault->tpg_onode();
      auto inode = fault->tpg_inode();
      SatLiteral glit(gvar(inode));
      SatLiteral hlit(hvar(inode));
      SatLiteral dlit(dvar(root));
      auto xlit = solver().new_variable();
      solver().add_clause( glit,  hlit, ~xlit);
      solver().add_clause(~glit, ~hlit, ~xlit);
      solver().add_clause( dlit, ~xlit);
      tmp_lits.push_back(xlit);
    }
    solver().add_clause(tmp_lits);
  }

  cnf_end();

  SatBool3 sat_res = solve({});
  if ( sat_res == SatBool3::True ) {
    NodeValList suf_cond = get_sufficient_condition();
    TestVector testvect = backtrace(suf_cond);
    return DtpgResult{testvect};
  }
  else if ( sat_res == SatBool3::False ) {
    return DtpgResult{FaultStatus::Untestable};
  }
  else { // sat_res == SatBool3::X
    return DtpgResult{FaultStatus::Undetected};
  }
}

// @brief タイマーをスタートする．
void
MF_Dtpg::cnf_begin()
{
  timer_start();
}

// @brief タイマーを止めて CNF 作成時間に加える．
void
MF_Dtpg::cnf_end()
{
  double time = timer_stop();
  mStats.mCnfGenTime += time;
  ++ mStats.mCnfGenCount;
}

// @brief 時間計測を開始する．
void
MF_Dtpg::timer_start()
{
  if ( mTimerEnable ) {
    mTimer.reset();
    mTimer.start();
  }
}

/// @brief 時間計測を終了する．
double
MF_Dtpg::timer_stop()
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
MF_Dtpg::prepare_vars()
{
  // root の TFO を mTfoList に入れる．
  for ( auto root: mRootList ) {
    set_tfo_mark(root);
  }
  for ( int rpos = 0; rpos < mTfoList.size(); ++ rpos ) {
    const TpgNode* node = mTfoList[rpos];
    for ( auto onode: node->fanout_list() ) {
      set_tfo_mark(onode);
    }
  }

  // TFO の TFI を mNodeList に入れる．
  for ( auto node: mTfoList ) {
    for ( auto inode: node->fanin_list() ) {
      set_tfi_mark(inode);
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
    for ( auto root: mRootList ) {
      if ( root->is_dff_output() ) {
	mDffList.push_back(root->dff());
      }
    }
    for ( auto dff: mDffList ) {
      const TpgNode* node = dff->input();
      mTfi2List.push_back(node);
    }
    for ( auto root: mRootList ) {
      set_tfi2_mark(root);
    }
    for ( int rpos = 0; rpos < mTfi2List.size(); ++ rpos) {
      const TpgNode* node = mTfi2List[rpos];
      for ( auto inode: node->fanin_list() ) {
	set_tfi2_mark(inode);
      }
    }
  }

  // TFO の部分に変数を割り当てる．
  for ( auto node: mTfoList ) {
    auto gvar = mSolver.new_variable(true);
    auto fvar = mSolver.new_variable(true);
    auto dvar = mSolver.new_variable();

    mGvarMap.set_vid(node, gvar);
    mFvarMap.set_vid(node, fvar);
    mDvarMap.set_vid(node, dvar);

    if ( debug_dtpg ) {
      DEBUG_OUT << "gvar(Node#" << node->id() << ") = " << gvar << endl
		<< "fvar(Node#" << node->id() << ") = " << fvar << endl
		<< "dvar(Node#" << node->id() << ") = " << dvar << endl;
    }
  }

  // TFI の部分に変数を割り当てる．
  for ( auto node: mTfiList ) {
    auto gvar = mSolver.new_variable(true);

    mGvarMap.set_vid(node, gvar);
    mFvarMap.set_vid(node, gvar);

    if ( debug_dtpg ) {
      DEBUG_OUT << "gvar(Node#" << node->id() << ") = " << gvar << endl
		<< "fvar(Node#" << node->id() << ") = " << gvar << endl;
    }
  }

  // TFI2 の部分に変数を割り当てる．
  for ( auto node: mTfi2List ) {
    auto hvar = mSolver.new_variable(true);

    mHvarMap.set_vid(node, hvar);

    if ( debug_dtpg ) {
      DEBUG_OUT << "hvar(Node#" << node->id() << ") = " << hvar << endl;
    }
  }
}

// @brief 対象の部分回路の正常値の関係を表す CNF 式を作る．
void
MF_Dtpg::gen_good_cnf()
{
  //////////////////////////////////////////////////////////////////////
  // 正常回路の CNF を生成
  //////////////////////////////////////////////////////////////////////
  GateEnc gval_enc(mSolver, mGvarMap);
  for ( auto node: mTfoList ) {
    gval_enc.make_cnf(node);

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
  for ( auto node: mTfiList ) {
    gval_enc.make_cnf(node);

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

  for ( auto dff: mDffList ) {
    const TpgNode* onode = dff->output();
    const TpgNode* inode = dff->input();
    // DFF の入力の1時刻前の値と出力の値が等しい．
    auto olit = gvar(onode);
    auto ilit = hvar(inode);
    mSolver.add_buffgate(olit, ilit);
  }

  GateEnc hval_enc(mSolver, mHvarMap);
  for ( auto node: mTfi2List ) {
    hval_enc.make_cnf(node);

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
MF_Dtpg::gen_faulty_cnf()
{
  //////////////////////////////////////////////////////////////////////
  // 故障回路の CNF を生成
  //////////////////////////////////////////////////////////////////////
  for ( auto fault: mFaultList ) {
    FaultyGateEnc fgate_enc(mSolver, mFvarMap, fault);
    fgate_enc.make_cnf();
  }
  GateEnc fval_enc(mSolver, mFvarMap);
  for ( auto node: mTfoList ) {
    if ( !root_mark(node) ) {
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
    make_dchain_cnf(node);
  }
}

// @brief 故障伝搬条件を表すCNF式を生成する．
// @param[in] node 対象のノード
void
MF_Dtpg::make_dchain_cnf(
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
    DEBUG_OUT << "dvar(Node#" << node->id() << ") -> "
	      << glit << " XOR " << flit << endl;
  }

  if ( node->is_ppo() ) {
    mSolver.add_clause(~glit,  flit,  dlit);
    mSolver.add_clause( glit, ~flit,  dlit);

    if ( debug_dtpg ) {
      DEBUG_OUT << "!dvar(Node#" << node->id() << ") -> "
		<< glit << " = " << flit << endl;
    }
  }
  else {
    // dlit -> ファンアウト先のノードの dlit の一つが 1

    if ( debug_dtpg ) {
      DEBUG_OUT << "dvar(Node#" << node->id() << ") -> ";
    }
    int nfo = node->fanout_num();
    if ( nfo == 1 ) {
      SatLiteral odlit(mDvarMap(node->fanout_list()[0]));
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

      const TpgNode* imm_dom = node->imm_dom();
      if ( imm_dom != nullptr ) {
	SatLiteral odlit(mDvarMap(imm_dom));
	mSolver.add_clause(~dlit, odlit);

	if ( debug_dtpg ) {
	  DEBUG_OUT << "dvar(Node#" << node->id() << ") -> "
		    << odlit << endl;
	}
      }
    }
  }
}

// @brief 十分条件を取り出す．
NodeValList
MF_Dtpg::get_sufficient_condition()
{
  Extractor extractor{mGvarMap, mFvarMap, mSatModel};
  auto suf_cond = extractor.get_assignment(mRootList);

  for ( auto fault: mFaultList ) {
    auto onode = fault->tpg_onode();
    if ( fault->is_branch_fault() ) {
      // ブランチの故障の場合はファンインの割当を条件に加える．
      for ( int i: Range(onode->fanin_num()) ) {
	auto inode = onode->fanin(i);
	bool val = (gval(inode) == Val3::_1);
	suf_cond.add(inode, 1, val);
      }
    }
    else {
      // ステムの故障の場合は出力の割当を条件に加える．
      bool val = (gval(onode) == Val3::_1);
      suf_cond.add(onode, 1, val);
    }
  }
  if ( mFaultType == FaultType::TransitionDelay ) {
    // 遷移故障の場合は１時刻前の値も条件に加える．
    for ( auto fault: mFaultList ) {
      auto inode = fault->tpg_inode();
      if ( gval(inode) != hval(inode) ) {
	// ただし異なっているときのみ
	bool val = (hval(inode) == Val3::_1);
	suf_cond.add(inode, 0, val);
      }
    }
  }

  return suf_cond;
}

// @brief バックトレースを行う．
TestVector
MF_Dtpg::backtrace(const NodeValList& suf_cond)
{
  Timer timer;
  timer.start();

  // バックトレースを行う．
  auto testvect = mJustifier(mFaultType, suf_cond, mHvarMap, mGvarMap, mSatModel);

  timer.stop();
  mStats.mBackTraceTime += timer.get_time();

  return testvect;
}

// @brief 一つの SAT問題を解く．
SatBool3
MF_Dtpg::solve(
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
    mSatModel = mSolver.model();
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

// @brief SAT問題が充足可能か調べる．
// @param[in] assumptions 値の決まっている変数のリスト
// @return 結果を返す．
//
// solve() との違いは結果のモデルを保持しない．
SatBool3
MF_Dtpg::check(const vector<SatLiteral>& assumptions)
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

// @brief 直前の solve() の結果からテストベクタを作る．
// @return 作成したテストベクタを返す．
//
// この関数では単純に外部入力の値を記録する．
TestVector
MF_Dtpg::get_tv()
{
  NodeValList assign_list;
  if ( mFaultType == FaultType::StuckAt ) {
    for ( auto node: mPPIList ) {
      bool val = gval(node) == Val3::_1;
      assign_list.add(node, 1, val);
    }
  }
  else {
    for ( auto node: mPPIList ) {
      bool val = hval(node) == Val3::_1;
      assign_list.add(node, 0, val);
    }
    for ( auto node: mAuxInputList ) {
      bool val = gval(node) == Val3::_1;
      assign_list.add(node, 1, val);
    }
  }
  return TestVector::new_from_assign_list(mNetwork.input_num(), mNetwork.dff_num(), mFaultType,
					  assign_list);
}

END_NAMESPACE_DRUID
