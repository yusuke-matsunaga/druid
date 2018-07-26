﻿
/// @file DtpgEngine.cc
/// @brief DtpgEngine の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgEngine.h"

#include "TpgNetwork.h"
#include "TpgFault.h"
#include "TpgDff.h"
#include "GateType.h"
#include "GateEnc.h"
#include "Val3.h"
#include "NodeValList.h"
#include "Justifier.h"
#include "TestVector.h"

#include "ym/SatSolver.h"
#include "ym/SatStats.h"
#include "ym/StopWatch.h"
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


BEGIN_NAMESPACE_YM_SATPG

// @brief コンストラクタ
// @param[in] sat_type SATソルバの種類を表す文字列
// @param[in] sat_option SATソルバに渡すオプション文字列
// @param[in] sat_outp SATソルバ用の出力ストリーム
// @param[in] fault_type 故障の種類
// @param[in] bt バックトレーサー
// @param[in] network 対象のネットワーク
// @param[in] root 故障伝搬の起点となるノード
DtpgEngine::DtpgEngine(const string& sat_type,
		       const string& sat_option,
		       ostream* sat_outp,
		       FaultType fault_type,
		       const string& just_type,
		       const TpgNetwork& network,
		       const TpgNode* root) :
  mSolver(sat_type, sat_option, sat_outp),
  mNetwork(network),
  mFaultType(fault_type),
  mRoot(root),
  mMarkArray(mNetwork.node_num(), 0U),
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
DtpgEngine::~DtpgEngine()
{
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
  USTime time = timer_stop();
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
USTime
DtpgEngine::timer_stop()
{
  USTime time(0, 0, 0);
  if ( mTimerEnable ) {
    mTimer.stop();
    time = mTimer.time();
  }
  return time;
}

// @brief 対象の部分回路の関係を表す変数を用意する．
void
DtpgEngine::prepare_vars()
{
  // root の TFO を mTfoList に入れる．
  set_tfo_mark(mRoot);
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
    if ( mRoot->is_dff_output() ) {
      mDffList.push_back(mRoot->dff());
    }
    for ( auto dff: mDffList ) {
      const TpgNode* node = dff->input();
      mTfi2List.push_back(node);
    }
    set_tfi2_mark(mRoot);
    for ( int rpos = 0; rpos < mTfi2List.size(); ++ rpos) {
      const TpgNode* node = mTfi2List[rpos];
      for ( auto inode: node->fanin_list() ) {
	set_tfi2_mark(inode);
      }
    }
  }

  // TFO の部分に変数を割り当てる．
  for ( auto node: mTfoList ) {
    SatVarId gvar = mSolver.new_variable();
    SatVarId fvar = mSolver.new_variable();
    SatVarId dvar = mSolver.new_variable();

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
    SatVarId gvar = mSolver.new_variable();

    mGvarMap.set_vid(node, gvar);
    mFvarMap.set_vid(node, gvar);

    if ( debug_dtpg ) {
      DEBUG_OUT << "gvar(Node#" << node->id() << ") = " << gvar << endl
		<< "fvar(Node#" << node->id() << ") = " << gvar << endl;
    }
  }

  // TFI2 の部分に変数を割り当てる．
  for ( auto node: mTfi2List ) {
    SatVarId hvar = mSolver.new_variable();

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
    SatLiteral olit(gvar(onode));
    SatLiteral ilit(hvar(inode));
    mSolver.add_eq_rel(olit, ilit);
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
DtpgEngine::gen_faulty_cnf()
{
  //////////////////////////////////////////////////////////////////////
  // 故障回路の CNF を生成
  //////////////////////////////////////////////////////////////////////
  GateEnc fval_enc(mSolver, mFvarMap);
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
    const TpgNode* node = mOutputList[i];
    SatLiteral dlit(dvar(node));
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
    SatLiteral dlit(dvar(node));
    mSolver.add_clause(~dlit);
  }
}

// @brief 故障伝搬条件を表すCNF式を生成する．
// @param[in] node 対象のノード
void
DtpgEngine::make_dchain_cnf(const TpgNode* node)
{
  SatLiteral glit(mGvarMap(node));
  SatLiteral flit(mFvarMap(node));
  SatLiteral dlit(mDvarMap(node));

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

// @brief 故障の影響がFFRの根のノードまで伝搬する条件を作る．
// @param[in] fault 対象の故障
// @param[out] assign_list 結果の値割り当てリスト
NodeValList
DtpgEngine::make_ffr_condition(const TpgFault* fault)
{
  if ( debug_dtpg ) {
    DEBUG_OUT << "make_ffr_condition" << endl;
  }

  NodeValList assign_list;

  // 故障の活性化条件を作る．
  const TpgNode* inode = fault->tpg_inode();
  // 0 縮退故障の時に 1 にする．
  bool val = (fault->val() == 0);
  add_assign(assign_list, inode, 1, val);

  if ( mFaultType == FaultType::TransitionDelay ) {
    // 1時刻前の値が逆の値である条件を作る．
    add_assign(assign_list, inode, 0, !val);
  }

  // ブランチの故障の場合，ゲートの出力までの伝搬条件を作る．
  if ( fault->is_branch_fault() ) {
    const TpgNode* onode = fault->tpg_onode();
    add_side_input(onode, fault->tpg_pos(), assign_list);
  }

  // FFR の根までの伝搬条件を作る．
  for ( const TpgNode* node = fault->tpg_onode(); node->fanout_num() == 1;
	node = node->fanout_list()[0]) {
    const TpgNode* fonode = node->fanout_list()[0];
    add_side_input(fonode, node, assign_list);
  }

  if ( debug_dtpg ) {
    DEBUG_OUT << endl;
  }

  return assign_list;
}

// @brief NodeValList に追加する．
// @param[in] assign_list 追加するリスト
// @param[in] node 対象のノード
// @param[in] time 時刻 ( 0 or 1 )
// @param[in] val 値
void
DtpgEngine::add_assign(NodeValList& assign_list,
		       const TpgNode* node,
		       int time,
		       bool val)
{
  assign_list.add(node, time, val);

  if ( debug_dtpg ) {
    print_node(DEBUG_OUT, mNetwork, node);
    DEBUG_OUT << "@" << time << ": ";
    if ( val ) {
      DEBUG_OUT << "1" << endl;
    }
    else {
      DEBUG_OUT << "0" << endl;
    }
  }
}

// @brief バックトレースを行う．
// @param[in] fault 故障
// @param[in] suf_cond 十分条件の割り当て
// @param[in] model SATモデル
// @return テストパタンを返す．
TestVector
DtpgEngine::backtrace(const TpgFault* fault,
		      const NodeValList& suf_cond,
		      const vector<SatBool3>& model)
{
  StopWatch timer;
  timer.start();

  // バックトレースを行う．
  TestVector testvect;
  if ( mFaultType == FaultType::TransitionDelay ) {
    testvect = mJustifier(suf_cond, mHvarMap, mGvarMap, model);
  }
  else {
    testvect = mJustifier(suf_cond, mGvarMap, model);
  }

  timer.stop();
  mStats.mBackTraceTime += timer.time();

  return testvect;
}

// @brief 値割り当てをリテラルに変換する．
SatLiteral
DtpgEngine::conv_to_literal(NodeVal node_val)
{
  const TpgNode* node = node_val.node();
  bool inv = !node_val.val(); // 0 の時が inv = true
  SatVarId vid = (node_val.time() == 0) ? hvar(node) : gvar(node);
  return SatLiteral(vid, inv);
}

// @brief 値割り当てをリテラルのリストに変換する．
// @param[in] assign_list 値の割り当てリスト
// @param[out] assumptions 変換したリテラルを追加するリスト
void
DtpgEngine::conv_to_assumptions(const NodeValList& assign_list,
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
DtpgEngine::solve(const vector<SatLiteral>& assumptions,
		  vector<SatBool3>& model)
{
  StopWatch timer;
  timer.start();

  SatStats prev_stats;
  mSolver.get_stats(prev_stats);

  SatBool3 ans = mSolver.solve(assumptions, model);

  timer.stop();
  USTime time = timer.time();

  SatStats sat_stats;
  mSolver.get_stats(sat_stats);
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

// @brief 十分条件を取り出す．
// @param[in] fault 対象の故障
// @param[in] model SATモデル
// @return 十分条件を表す割当リストを返す．
NodeValList
DtpgEngine::get_sufficient_condition(const TpgFault* fault,
				     const vector<SatBool3>& model)
{
  extern
  NodeValList
  extract(const TpgNode* root,
	  const VidMap& gvar_map,
	  const VidMap& fvar_map,
	  const vector<SatBool3>& model);

  const TpgNode* ffr_root = fault->tpg_onode()->ffr_root();
  return extract(ffr_root, mGvarMap, mFvarMap, model);
}

// @brief 複数の十分条件を取り出す．
// @param[in] fault 対象の故障
// @param[in] model SATモデル
//
// FFR内の故障伝搬条件は含まない．
Expr
DtpgEngine::get_sufficient_conditions(const TpgFault* fault,
				      const vector<SatBool3>& model)
{
  extern
  Expr
  extract_all(const TpgNode* root,
	      const VidMap& gvar_map,
	      const VidMap& fvar_map,
	      const vector<SatBool3>& model);

  const TpgNode* ffr_root = fault->tpg_onode()->ffr_root();
  return extract_all(ffr_root, mGvarMap, mFvarMap, model);
}

// @brief SATソルバに論理式の否定を追加する．
// @param[in] expr 対象の論理式
// @param[in] clit 制御用のリテラル
//
// clit が true の時に与えられた論理式が false となる条件を追加する．
// 論理式の変数番号はノード番号に対応している．
void
DtpgEngine::add_negation(const Expr& expr,
			 SatLiteral clit)
{
  if ( expr.is_posiliteral() ) {
    int id = expr.varid().val();
    const TpgNode* node = mNetwork.node(id);
    SatLiteral lit(gvar(node));
    solver().add_clause(~clit, ~lit);
  }
  else if ( expr.is_negaliteral() ) {
    int id = expr.varid().val();
    const TpgNode* node = mNetwork.node(id);
    SatLiteral lit(gvar(node));
    solver().add_clause(~clit,  lit);
  }
  else if ( expr.is_and() ) {
    int n = expr.child_num();
    ASSERT_COND( n > 0 );
    vector<SatLiteral> tmp_lits(n + 1);
    tmp_lits[0] = ~clit;
    for ( int i: Range(n) ) {
      SatLiteral lit1 = _add_negation_sub(expr.child(i));
      tmp_lits[i + 1] = ~lit1;
    }
    solver().add_clause(tmp_lits);
  }
  else if ( expr.is_or() ) {
    int n = expr.child_num();
    for ( int i: Range(n) ) {
      SatLiteral lit1 = _add_negation_sub(expr.child(i));
      solver().add_clause(~clit, ~lit1);
    }
  }
  else {
    ASSERT_NOT_REACHED;
  }
}

// @brief add_negation の下請け関数
// @param[in] expr 論理式
SatLiteral
DtpgEngine::_add_negation_sub(const Expr& expr)
{
  if ( expr.is_posiliteral() ) {
    int id = expr.varid().val();
    const TpgNode* node = mNetwork.node(id);
    SatLiteral lit(gvar(node));
    return lit;
  }
  else if ( expr.is_negaliteral() ) {
    int id = expr.varid().val();
    const TpgNode* node = mNetwork.node(id);
    SatLiteral lit(gvar(node));
    return ~lit;
  }
  else if ( expr.is_and() ) {
    int n = expr.child_num();
    SatVarId nvar = solver().new_variable();
    SatLiteral nlit(nvar);
    vector<SatLiteral> tmp_lits(n + 1);
    tmp_lits[0] = nlit;
    for ( int i: Range(n) ) {
      SatLiteral lit1 = _add_negation_sub(expr.child(i));
      tmp_lits[i + 1] = ~lit1;
    }
    solver().add_clause(tmp_lits);
    return nlit;
  }
  else if ( expr.is_or() ) {
    int n = expr.child_num();
    SatVarId nvar = solver().new_variable();
    SatLiteral nlit(nvar);
    for ( int i: Range(n) ) {
      SatLiteral lit1 = _add_negation_sub(expr.child(i));
      solver().add_clause(nlit, ~lit1);
    }
    return nlit;
  }

  ASSERT_NOT_REACHED;
  return SatLiteral();
}

// @brief side-input の割り当てを得る．
// @param[in] node ノード
// @param[in] ipos 対象のファンイン番号
// @param[out] nodeval_list 割り当てリスト
//
// * node が非制御値を持たない場合は nodeval_list は空になる．
void
DtpgEngine::add_side_input(const TpgNode* node,
			   int ipos,
			   NodeValList& nodeval_list)
{
  Val3 nval = node->nval();
  if ( nval != Val3::_X ) {
    bool val = (nval == Val3::_1);
    for ( auto ipos1: Range(node->fanin_num()) ) {
      if ( ipos1 != ipos ) {
	auto inode1 = node->fanin(ipos1);
	add_assign(nodeval_list, inode1, 1, val);
      }
    }
  }
}

// @brief side-input の割り当てを得る．
// @param[in] node ノード
// @param[in] inode 対象のファンイン
// @param[out] nodeval_list 割り当てリスト
//
// * node が非制御値を持たない場合は nodeval_list は空になる．
// * 上の関数との違いは同じノードが重複してファンインとなっている場合
void
DtpgEngine::add_side_input(const TpgNode* node,
			   const TpgNode* inode,
			   NodeValList& nodeval_list)
{
  int ni = node->fanin_num();
  if ( ni == 1 ) {
    return;
  }
  Val3 nval = node->nval();
  if ( nval == Val3::_X ) {
    return;
  }
  bool val = (nval == Val3::_1);
  for ( auto inode1: node->fanin_list() ) {
    if ( inode1 != inode ) {
      add_assign(nodeval_list, inode1, 1, val);
    }
  }
}

END_NAMESPACE_YM_SATPG
