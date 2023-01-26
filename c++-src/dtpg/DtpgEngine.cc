
/// @file DtpgEngine.cc
/// @brief DtpgEngine の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2022, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgEngine.h"

#include "TpgNetwork.h"
#include "TpgFault.h"
#include "TpgDff.h"
#include "GateType.h"
#include "GateEnc.h"
#include "Val3.h"
#include "NodeValList.h"
#include "Extractor.h"
#include "MultiExtractor.h"
#include "Justifier.h"
#include "TestVector.h"

#include "ym/SatSolver.h"
#include "ym/SatStats.h"
#include "ym/Range.h"

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

// @brief コンストラクタ
DtpgEngine::DtpgEngine(
  const TpgNetwork& network,
  FaultType fault_type,
  const TpgNode* root,
  const string& just_type,
  const SatSolverType& solver_type
) : mSolver{solver_type},
    mNetwork{network},
    mFaultType{fault_type},
    mRoot{root},
    mMarkArray(mNetwork.node_num(), 0U),
    mHvarMap{network.node_num()},
    mGvarMap{network.node_num()},
    mFvarMap{network.node_num()},
    mDvarMap{network.node_num()},
    mJustifier{just_type, network},
    mTimerEnable{true}
{
  mTfoList.reserve(network.node_num());
  mTfiList.reserve(network.node_num());
  mTfi2List.reserve(network.node_num());
  mOutputList.reserve(network.ppo_num());
}

// @brief CNF の生成を行う．
void
DtpgEngine::make_cnf()
{
  cnf_begin();

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

  cnf_end();
}

// @brief タイマーをスタートする．
void
DtpgEngine::cnf_begin()
{
  timer_start();
}

// @brief タイマーを止めて CNF 作成時間に加える．
void
DtpgEngine::cnf_end()
{
  auto time = timer_stop();
  mStats.mCnfGenTime += time;
  ++ mStats.mCnfGenCount;
}

// @brief 時間計測を開始する．
void
DtpgEngine::timer_start()
{
  if ( mTimerEnable ) {
    mTimer.reset();
    mTimer.start();
  }
}

/// @brief 時間計測を終了する．
double
DtpgEngine::timer_stop()
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
DtpgEngine::prepare_vars()
{
  // root の TFO を mTfoList に入れる．
  set_tfo_mark(mRoot);
  for ( int rpos = 0; rpos < mTfoList.size(); ++ rpos ) {
    // set_tfo_mark() 中で mTfoList に要素を追加しているので
    // 古いタイプの for 文を用いている．
    auto node = mTfoList[rpos];
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
    // set_tfi_mark() 中で mTfiList に要素を追加しているので
    // 古いタイプの for 文を用いている．
    auto node = mTfiList[rpos];
    for ( auto inode: node->fanin_list() ) {
      set_tfi_mark(inode);
    }
  }

  // TFI に含まれる DFF のさらに TFI を mTfi2List に入れる．
  if ( mFaultType == FaultType::TransitionDelay ) {
    if ( mRoot->is_dff_output() ) {
      mDffList.push_back(mRoot->dff());
    }
    for ( auto dff: mDffList ) {
      auto node = dff->input();
      mTfi2List.push_back(node);
    }
    set_tfi2_mark(mRoot);
    for ( int rpos = 0; rpos < mTfi2List.size(); ++ rpos) {
      // set_tfi2_mark() 中で mTfi2List に要素を追加しているので
      // 古いタイプの for 文を用いている．
      auto node = mTfi2List[rpos];
      for ( auto inode: node->fanin_list() ) {
	set_tfi2_mark(inode);
      }
    }
  }

  // TFO の部分に変数を割り当てる．
  for ( auto node: mTfoList ) {
    auto gvar = mSolver.new_variable(true);
    auto fvar = mSolver.new_variable(true);
    auto dvar = mSolver.new_variable(true);

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
DtpgEngine::gen_good_cnf()
{
  //////////////////////////////////////////////////////////////////////
  // 正常回路の CNF を生成
  //////////////////////////////////////////////////////////////////////
  GateEnc gval_enc{mSolver, mGvarMap};
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
    auto onode = dff->output();
    auto inode = dff->input();
    // DFF の入力の1時刻前の値と出力の値が等しい．
    auto olit = gvar(onode);
    auto ilit = hvar(inode);
    mSolver.add_buffgate(olit, ilit);
  }

  GateEnc hval_enc{mSolver, mHvarMap};
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
DtpgEngine::gen_faulty_cnf()
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
    make_dchain_cnf(node);
  }
}

#if 0
// @brief root の影響が外部出力まで伝搬する条件のCNF式を作る．
void
DtpgEngine::gen_detect_cnf()
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
  int no = mOutputList.size();
  vector<SatLiteral> odiff(no);
  for (int i = 0; i < no; ++ i) {
    auto node = mOutputList[i];
    auto dlit = dvar(node);
    odiff[i] = dlit;
  }
  mSolver.add_clause(odiff);

  if ( !mRoot->is_ppo() ) {
    // mRoot の dlit が1でなければならない．
    mSolver.add_clause(SatLiteral(dvar(mRoot)));
  }
}

// @brief 故障の伝搬しない条件を表す CNF 式を作る．
void
DtpgEngine::gen_undetect_cnf()
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
  for ( auto node: mOutputList ) {
    auto dlit = dvar(node);
    mSolver.add_clause(~dlit);
  }
}
#endif

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
	  DEBUG_OUT << "dvar(Node#" << node->id() << ") -> "
		    << odlit << endl;
	}
      }
    }
  }
}

// @brief テストパタンを求める．
DtpgResult
DtpgEngine::gen_pattern(
  const TpgFault* fault
)
{
  // 追加の条件
  auto assumptions = gen_assumptions(fault);

  // FFR 内の故障伝搬条件を ffr_cond に入れる．
  auto ffr_cond = fault->ffr_propagate_condition(fault_type());

  // ffr_cond の内容を assumptions に追加する．
  add_to_literal_list(ffr_cond, assumptions);

  auto sat_res = check(assumptions);
  if ( sat_res == SatBool3::True ) {
    auto testvect = backtrace(fault->tpg_onode()->ffr_root(), ffr_cond);
    return DtpgResult{testvect};
  }
  else if ( sat_res == SatBool3::False ) {
    return DtpgResult{FaultStatus::Untestable};
  }
  else { // sat_res == SatBool3::X
    return DtpgResult{FaultStatus::Undetected};
  }
}

// @brief バックトレースを行う．
TestVector
DtpgEngine::backtrace(
  const TpgNode* ffr_root,
  const NodeValList& ffr_cond
)
{
  Timer timer;
  timer.start();

  auto suf_cond = get_sufficient_condition(ffr_root);
  suf_cond.merge(ffr_cond);

  // バックトレースを行う．
  auto testvect = mJustifier(mFaultType, suf_cond, mHvarMap, mGvarMap, mSatModel);

  timer.stop();
  mStats.mBackTraceTime += timer.get_time();

  return testvect;
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

// @brief SAT問題が充足可能か調べる．
SatBool3
DtpgEngine::check(
  const vector<SatLiteral>& assumptions
)
{
  Timer timer;
  timer.start();

  auto prev_stats = mSolver.get_stats();

  auto ans = mSolver.solve(assumptions);

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

// @brief 直前の solve() の結果からテストベクタを作る．
// @return 作成したテストベクタを返す．
//
// この関数では単純に外部入力の値を記録する．
TestVector
DtpgEngine::get_tv()
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

// @brief 十分条件を取り出す．
NodeValList
DtpgEngine::get_sufficient_condition(
  const TpgNode* ffr_root
)
{
  Extractor extractor{mGvarMap, mFvarMap, mSatModel};
  return extractor.get_assignment({ffr_root});
}

// @brief 複数の十分条件を取り出す．
//
// FFR内の故障伝搬条件は含まない．
Expr
DtpgEngine::get_sufficient_conditions(
  const TpgNode* ffr_root
)
{
  MultiExtractor extractor{mGvarMap, mFvarMap, mSatModel};
  return extractor.get_assignments(ffr_root);
}

// @brief 必要条件を取り出す．
NodeValList
DtpgEngine::get_mandatory_condition(
  const NodeValList& ffr_cond,
  const NodeValList& suf_cond
)
{
  NodeValList mand_cond;
  auto assumptions = conv_to_literal_list(ffr_cond);
  for ( auto nv: suf_cond ) {
    auto lit = conv_to_literal(nv);
    auto assumptions1 = assumptions;
    assumptions1.push_back(~lit);
    auto tmp_res = check(assumptions1);
    if ( tmp_res == SatBool3::False ) {
      mand_cond.add(nv);
      assumptions.push_back(lit);
    }
  }
  // ffr_cond も mandatory_condition に加える．
  mand_cond.merge(ffr_cond);

  return mand_cond;
}

// @brief SATソルバに論理式の否定を追加する．
void
DtpgEngine::add_negation(
  const Expr& expr,
  SatLiteral clit
)
{
  if ( expr.is_posi_literal() ) {
    int id = expr.varid();
    auto node = mNetwork.node(id);
    auto lit = gvar(node);
    solver().add_clause(~clit, ~lit);
  }
  else if ( expr.is_nega_literal() ) {
    int id = expr.varid();
    auto node = mNetwork.node(id);
    auto lit = gvar(node);
    solver().add_clause(~clit,  lit);
  }
  else if ( expr.is_and() ) {
    SizeType n = expr.operand_num();
    ASSERT_COND( n > 0 );
    vector<SatLiteral> tmp_lits;
    tmp_lits.reserve(n + 1);
    tmp_lits.push_back(~clit);
    for ( auto expr1: expr.operand_list() ) {
      auto lit1 = _add_negation_sub(expr1);
      tmp_lits.push_back(~lit1);
    }
    solver().add_clause(tmp_lits);
  }
  else if ( expr.is_or() ) {
    for ( auto expr1: expr.operand_list() ) {
      auto lit1 = _add_negation_sub(expr1);
      solver().add_clause(~clit, ~lit1);
    }
  }
  else {
    ASSERT_NOT_REACHED;
  }
}

// @brief add_negation の下請け関数
SatLiteral
DtpgEngine::_add_negation_sub(
  const Expr& expr
)
{
  if ( expr.is_posi_literal() ) {
    int id = expr.varid();
    auto node = mNetwork.node(id);
    auto lit = gvar(node);
    return lit;
  }
  else if ( expr.is_nega_literal() ) {
    int id = expr.varid();
    auto node = mNetwork.node(id);
    auto lit = gvar(node);
    return ~lit;
  }
  else if ( expr.is_and() ) {
    SizeType n = expr.operand_num();
    auto nlit = solver().new_variable();
    vector<SatLiteral> tmp_lits;
    tmp_lits.reserve(n + 1);
    tmp_lits.push_back(nlit);
    for ( auto expr1: expr.operand_list() ) {
      auto lit1 = _add_negation_sub(expr1);
      tmp_lits.push_back(~lit1);
    }
    solver().add_clause(tmp_lits);
    return nlit;
  }
  else if ( expr.is_or() ) {
    auto nlit = solver().new_variable();
    for ( auto expr1: expr.operand_list() ) {
      auto lit1 = _add_negation_sub(expr1);
      solver().add_clause(nlit, ~lit1);
    }
    return nlit;
  }

  ASSERT_NOT_REACHED;
  return SatLiteral();
}

END_NAMESPACE_DRUID
