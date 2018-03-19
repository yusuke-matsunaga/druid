
/// @file StructEnc.cc
/// @brief StructEnc の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2015, 2017 Yusuke Matsunaga
/// All rights reserved.


#include "StructEnc.h"
#include "SimplePropCone.h"
#include "MffcPropCone.h"
#include "Justifier.h"
#include "NodeValList.h"

#include "TpgFault.h"
#include "TpgNode.h"
#include "TpgDff.h"
#include "TpgMFFC.h"

#include "ValMap_model.h"

#include "GateLitMap_vid.h"
#include "GateLitMap_vid2.h"


BEGIN_NAMESPACE_YM_SATPG_STRUCTENC

BEGIN_NONAMESPACE

const ymuint debug_make_vars = 1U;
const ymuint debug_make_node_cnf = 2U;
const ymuint debug_extract = 32U;
const ymuint debug_justify = 64U;

END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// クラス StructEnc
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] max_node_id ノード番号の最大値
// @param[in] fault_type 故障の種類
// @param[in] sat_type SATソルバの種類を表す文字列
// @param[in] sat_option SATソルバに渡すオプション文字列
// @param[in] sat_outp SATソルバ用の出力ストリーム
StructEnc::StructEnc(ymuint max_node_id,
		     FaultType fault_type,
		     const string& sat_type,
		     const string& sat_option,
		     ostream* sat_outp) :
  mFaultType(fault_type),
  mSolver(sat_type, sat_option, sat_outp),
  mMaxId(max_node_id),
  mMark(max_node_id, false)
{
  for (ymuint i = 0; i < 2; ++ i) {
    mVarMap[i].init(max_node_id);
  }
  mDebugFlag = 0;

#if 0
  mDebugFlag |= debug_extract;
  mDebugFlag |= debug_justify;
#endif
}

// @brief デストラクタ
StructEnc::~StructEnc()
{
  for (ymuint i = 0; i < mConeList.size(); ++ i) {
    PropCone* cone = mConeList[i];
    delete cone;
  }
}

// @brief fault cone を追加する．
// @param[in] fnode 故障のあるノード
// @param[in] detect 故障を検出する時に true にするフラグ
// @return 作成されたコーン番号を返す．
//
// fnode から到達可能な外部出力までの故障伝搬条件を考える．
ymuint
StructEnc::add_simple_cone(const TpgNode* fnode,
			   bool detect)
{
  return add_simple_cone(fnode, nullptr, detect);
}

// @brief fault cone を追加する．
// @param[in] fnode 故障のあるノード
// @param[in] bnode ブロックノード
// @param[in] detect 故障を検出する時に true にするフラグ
// @return 作成されたコーン番号を返す．
//
// bnode までの故障伝搬条件を考える．
ymuint
StructEnc::add_simple_cone(const TpgNode* fnode,
			   const TpgNode* bnode,
			   bool detect)
{
  PropCone* focone = new SimplePropCone(*this, fnode, bnode, detect);
  ymuint cone_id = mConeList.size();
  mConeList.push_back(focone);

  if ( fault_type() == kFtTransitionDelay ) {
    add_prev_node(fnode);
  }
  make_tfi_list(focone->tfo_node_list());

  return cone_id;
}

// @brief MFFC cone を追加する．
// @param[in] mffc MFFC の情報
// @param[in] detect 故障を検出する時に true にするフラグ
// @return 作成されたコーン番号を返す．
//
// fnode から到達可能な外部出力までの故障伝搬条件を考える．
ymuint
StructEnc::add_mffc_cone(const TpgMFFC* mffc,
			 bool detect)
{
  return add_mffc_cone(mffc, nullptr, detect);
}

// @brief MFFC cone を追加する．
// @param[in] mffc MFFC の情報
// @param[in] bnode ブロックノード
// @param[in] detect 故障を検出する時に true にするフラグ
// @return 作成されたコーン番号を返す．
//
// bnode までの故障伝搬条件を考える．
ymuint
StructEnc::add_mffc_cone(const TpgMFFC* mffc,
			 const TpgNode* bnode,
			 bool detect)
{
  PropCone* mffccone = new MffcPropCone(*this, mffc, bnode, detect);
  ymuint cone_id = mConeList.size();
  mConeList.push_back(mffccone);

  if ( fault_type() == kFtTransitionDelay ) {
    add_prev_node(mffc->root());
  }
  make_tfi_list(mffccone->tfo_node_list());

  return cone_id;
}

// @brief 故障を検出する条件を作る．
// @param[in] fault 故障
// @param[in] cone_id コーン番号
// @param[out] assumptions 結果の仮定を表すリテラルのリスト
void
StructEnc::make_fault_condition(const TpgFault* fault,
				ymuint cone_id,
				vector<SatLiteral>& assumptions)
{
  // FFR 内の故障伝搬条件を assign_list に入れる．
  NodeValList assign_list;
  const TpgNode* ffr_root = fault->tpg_onode()->ffr_root();
  add_ffr_condition(ffr_root, fault, assign_list);

  /// FFR より出力側の故障伝搬条件を assumptions に入れる．
  ASSERT_COND( cone_id < mConeList.size() );
  mConeList[cone_id]->make_prop_condition(ffr_root, assumptions);

  // assign_list を変換して assumptions に追加する．
  assumptions.reserve(assumptions.size() + assign_list.size());
  for (ymuint i = 0; i < assign_list.size(); ++ i) {
    NodeVal nv = assign_list[i];
    SatLiteral lit = nv_to_lit(nv);
    assumptions.push_back(lit);
  }
}

// @brief 故障の検出条件を割当リストに追加する．
// @param[in] fault 故障
// @param[out] assign_list 条件を表す割当リスト
void
StructEnc::add_fault_condition(const TpgFault* fault,
			       NodeValList& assign_list)
{
  // 故障の活性化条件
  const TpgNode* inode = fault->tpg_inode();
  // 0縮退故障の時 1にする．
  bool val = (fault->val() == 0);
  assign_list.add(inode, 1, val);

  if ( fault_type() == kFtTransitionDelay ) {
    // 1時刻前の値が逆の値である条件を作る．
    assign_list.add(inode, 0, !val);
  }

  // ブランチの故障の場合，ゲートの出力までの伝搬条件を作る．
  if ( fault->is_branch_fault() ) {
    // 故障の伝搬条件
    const TpgNode* onode = fault->tpg_onode();
    Val3 nval = onode->nval();
    if ( nval != kValX ) {
      ymuint ni = onode->fanin_num();
      bool val = (nval == kVal1);
      // inode -> onode の伝搬条件
      for (ymuint i = 0; i < ni; ++ i) {
	const TpgNode* inode1 = onode->fanin(i);
	if ( inode1 == inode ) {
	  continue;
	}
	assign_list.add(inode1, 1, val);
      }
    }
  }
}

// @brief FFR内の故障の伝搬条件を割当リストに追加する．
// @param[in] root_node FFRの根のノード
// @param[in] fault 故障
// @param[out] assign_list 条件を表す割当リスト
void
StructEnc::add_ffr_condition(const TpgNode* root_node,
			     const TpgFault* fault,
			     NodeValList& assign_list)
{
  // ノードに対する故障の伝搬条件
  add_fault_condition(fault, assign_list);

  // FFR の根までの伝搬条件
  for (const TpgNode* node = fault->tpg_onode(); node != root_node;
       node = node->fanout(0)) {
    ASSERT_COND( node->fanout_num() == 1 );
    const TpgNode* onode = node->fanout(0);
    ymuint ni = onode->fanin_num();
    if ( ni == 1 ) {
      // サイドインプットがなければスキップ
      continue;
    }
    Val3 nval = onode->nval();
    if ( nval == kValX ) {
      // 非制御値がなければスキップ
      // 実際には制御値がない時にスキップ
      // 例えば XOR ゲートの場合がそれにあたる．
      continue;
    }
    // サイドインプットの値を非制御値にする．
    bool val = (nval == kVal1);
    for (ymuint i = 0; i < ni; ++ i) {
      const TpgNode* inode = onode->fanin(i);
      if ( inode == node ) {
	continue;
      }
      assign_list.add(inode, 1, val);
    }
  }
}

// @brief 割当リストに従って値を固定する．
// @param[in] assignment 割当リスト
void
StructEnc::add_assignments(const NodeValList& assignment)
{
  ymuint n = assignment.size();
  for (ymuint i = 0; i < n; ++ i) {
    NodeVal nv = assignment[i];
    SatLiteral alit = nv_to_lit(nv);
    mSolver.add_clause(alit);
  }
}

// @brief 割当リストの否定の節を加える．
// @param[in] assignment 割当リスト
void
StructEnc::add_negation(const NodeValList& assignment)
{
  ymuint n = assignment.size();
  vector<SatLiteral> tmp_lits(n);
  for (ymuint i = 0; i < n; ++ i) {
    NodeVal nv = assignment[i];
    SatLiteral alit = nv_to_lit(nv);
    tmp_lits[i] = ~alit;
  }
  mSolver.add_clause(tmp_lits);
}

// @brief 割当リストを仮定のリテラルに変換する．
// @param[in] assign_list 割当リスト
// @param[out] assumptions 仮定を表すリテラルのリスト
//
// 必要に応じて使われているリテラルに関するCNFを追加する．
void
StructEnc::conv_to_assumption(const NodeValList& assign_list,
			      vector<SatLiteral>& assumptions)
{
  ymuint n = assign_list.size();
  for (ymuint i = 0; i < n; ++ i) {
    NodeVal nv = assign_list[i];
    SatLiteral alit = nv_to_lit(nv);
    assumptions.push_back(alit);
  }
}

// @brief 与えられたノード(のリスト)のTFIのリストを作る．
// @param[in] node_list ノードのリスト
void
StructEnc::make_tfi_list(const vector<const TpgNode*>& node_list)
{
  // node_list を mCurNodeList に入れる．
  for (ymuint i = 0; i < node_list.size(); ++ i) {
    const TpgNode* node = node_list[i];
    if ( !cur_mark(node) ) {
      add_cur_node(node);
    }
  }
  for (ymuint rpos = 0; rpos < mCurNodeList.size(); ++ rpos) {
    const TpgNode* node = mCurNodeList[rpos];

    // node のファンインを mCurNodeList に追加する．
    ymuint ni = node->fanin_num();
    for (ymuint i = 0; i < ni; ++ i) {
      const TpgNode* inode = node->fanin(i);
      if ( !cur_mark(inode) ) {
	add_cur_node(inode);
      }
    }

    // 遷移故障モードの場合には１時刻前の回路も作る．
    if ( fault_type() == kFtTransitionDelay && node->is_dff_output() ) {
      const TpgNode* inode = node->dff()->input();
      add_prev_node(inode);
    }
  }

  for (ymuint rpos = 0; rpos < mPrevNodeList.size(); ++ rpos) {
    const TpgNode* node = mPrevNodeList[rpos];

    // node のファンインを mCurNodeList に追加する．
    ymuint ni = node->fanin_num();
    for (ymuint i = 0; i < ni; ++ i) {
      const TpgNode* inode = node->fanin(i);
      if ( !prev_mark(inode) ) {
	add_prev_node(inode);
      }
    }
  }
}

// @brief 関係あるノードに変数を割り当てる．
void
StructEnc::make_vars()
{
  for (ymuint i = 0; i < mCurNodeList.size(); ++ i) {
    const TpgNode* node = mCurNodeList[i];
    if ( !var_mark(node, 1) ) {
      set_new_var(node, 1);
      if ( debug() & debug_make_vars ) {
	cout << node->name() << "@1 -> " << var(node, 1) << endl;
      }
    }
  }
  for (ymuint i = 0; i < mPrevNodeList.size(); ++ i) {
    const TpgNode* node = mPrevNodeList[i];
    if ( !var_mark(node, 0) ) {
      // FF の入力の場合は1時刻後の出力の変数を用いる．
      if ( node->is_dff_input() ) {
	const TpgNode* onode = node->dff()->output();
	if ( var_mark(onode, 1) ) {
	  _set_var(node, 0, var(onode, 1));
	}
	else {
	  // ここの制御ロジックが美しくない．
	  set_new_var(node, 0);
	}
      }
      else {
	set_new_var(node, 0);
      }
      if ( debug() & debug_make_vars ) {
	cout << node->name() << "@0 -> " << var(node, 0) << endl;
      }
    }
  }

  for (ymuint i = 0; i < mConeList.size(); ++ i) {
    PropCone* focone = mConeList[i];
    focone->make_vars();
  }
}

// @brief 関係あるノードの入出力の関係を表すCNFを作る．
void
StructEnc::make_cnf()
{
  for (ymuint i = 0; i < mCurNodeList.size(); ++ i) {
    const TpgNode* node = mCurNodeList[i];
    if ( !cnf_mark(node, 1) ) {
      set_cnf_mark(node, 1);
      make_node_cnf(node, var_map(1));
    }
  }
  for (ymuint i = 0; i < mPrevNodeList.size(); ++ i) {
    const TpgNode* node = mPrevNodeList[i];
    if ( !cnf_mark(node, 0) ) {
      set_cnf_mark(node, 0);
      make_node_cnf(node, var_map(0));
    }
  }

  for (ymuint i = 0; i < mConeList.size(); ++ i) {
    PropCone* focone = mConeList[i];
    focone->make_cnf();
  }
}

// @brief node の TFI の部分に変数を割り当てる．
// @param[in] node 対象のノード
// @param[in] time 時刻(0 or 1)
//
// 縮退故障モードの場合の時刻は 1
void
StructEnc::make_tfi_var(const TpgNode* node,
			int time)
{
  if ( var_mark(node, time) ) {
    return;
  }
  set_new_var(node, time);

  // 先に TFI のノードの変数を作る．
  ymuint ni = node->fanin_num();
  for (ymuint i = 0; i < ni; ++ i) {
    const TpgNode* inode = node->fanin(i);
    make_tfi_var(inode, time);
  }

  // 遷移故障モードの時は前の時刻の回路も作る．
  if ( fault_type() == kFtTransitionDelay &&
       node->is_dff_output() && time == 1 ) {
    const TpgNode* inode = node->dff()->input();
    make_tfi_var(inode, 0);
  }
}

// @brief node の TFI の CNF を作る．
// @param[in] node 対象のノード
// @param[in] time 時刻(0 or 1)
//
// 縮退故障モードの場合の時刻は 1
void
StructEnc::make_tfi_cnf(const TpgNode* node,
			int time)
{
  if ( cnf_mark(node, time) ) {
    return;
  }
  set_cnf_mark(node, time);

  make_tfi_var(node, time);

  // node の入出力の関係を表す節を作る．
  make_node_cnf(node, var_map(time));

  // TFI のノードの節を作る．
  ymuint ni = node->fanin_num();
  for (ymuint i = 0; i < ni; ++ i) {
    const TpgNode* inode = node->fanin(i);
    make_tfi_cnf(inode, time);
  }

  // 遷移故障モードの時は前の時刻の回路も作る．
  if ( fault_type() == kFtTransitionDelay &&
       node->is_dff_output() && time == 1 ) {
    const TpgNode* inode = node->dff()->input();
    make_tfi_cnf(inode, 0);
  }
}

// @brief チェックを行う．
// @param[out] sat_model SATの場合の解
SatBool3
StructEnc::check_sat(vector<SatBool3>& sat_model)
{
  return mSolver.solve(sat_model);
}

// @brief 割当リストのもとでチェックを行う．
// @param[in] assign_list 割当リスト
// @param[out] sat_model SATの場合の解
SatBool3
StructEnc::check_sat(const NodeValList& assign_list,
		     vector<SatBool3>& sat_model)
{
  vector<SatLiteral> assumptions;
  conv_to_assumption(assign_list, assumptions);

  return mSolver.solve(assumptions, sat_model);
}

// @brief 割当リストのもとでチェックを行う．
// @param[in] assign_list1, assign_list2 割当リスト
// @param[out] sat_model SATの場合の解
SatBool3
StructEnc::check_sat(const NodeValList& assign_list1,
		     const NodeValList& assign_list2,
		     vector<SatBool3>& sat_model)
{
  vector<SatLiteral> assumptions;

  conv_to_assumption(assign_list1, assumptions);
  conv_to_assumption(assign_list2, assumptions);

  return mSolver.solve(assumptions, sat_model);
}

/// @brief 結果のなかで必要なものだけを取り出す．
// @param[in] model SAT のモデル
// @param[in] fault 対象の故障
// @param[in] cone_id コーン番号
// @param[out] 値の割り当て結果を入れるリスト
void
StructEnc::extract(const vector<SatBool3>& model,
		   const TpgFault* fault,
		   ymuint cone_id,
		   NodeValList& assign_list)
{
  if ( debug() & debug_extract ) {
    cout << endl
	 << "StructEnc::extract(" << fault->str() << ")" << endl;
  }

  // fault から FFR の根までの条件を求める．
  const TpgNode* ffr_root = fault->tpg_onode()->ffr_root();
  add_ffr_condition(ffr_root, fault, assign_list);

  // ffr_root より先の条件を求める．
  ASSERT_COND( cone_id < mConeList.size() );
  mConeList[cone_id]->extract(model, ffr_root, assign_list);

  if ( debug() & debug_extract ) {
    cout << "  result = " << assign_list << endl;
  }
}

// @brief 外部入力の値割り当てを求める．
// @param[in] model SAT のモデル
// @param[in] assign_list 値割り当てのリスト
// @param[in] justifier 正当化を行うファンクタ
// @param[out] pi_assign_list 外部入力における値割り当てのリスト
//
// このクラスでの仕事はValMapに関する適切なオブジェクトを生成して
// justifier を呼ぶこと．
void
StructEnc::justify(const vector<SatBool3>& model,
		   const NodeValList& assign_list,
		   Justifier& justifier,
		   NodeValList& pi_assign_list)
{
  if ( debug() & debug_justify ) {
    cout << endl
	 << "StructEnc::justify(" << assign_list << ")" << endl;
  }

  ValMap_model val_map(var_map(0), var_map(1), var_map(1), model);
  justifier(assign_list, val_map, pi_assign_list);
  if ( debug() & debug_justify ) {
    cout << " => " << pi_assign_list << endl;
  }
}

// @brief ノードの入出力の関係を表すCNF式を作る．
// @param[in] node 対象のノード
// @param[in] var_map 変数マップ
void
StructEnc::make_node_cnf(const TpgNode* node,
			 const VidMap& var_map)
{
  GateLitMap_vid litmap(node, var_map);
  _make_node_cnf(node, litmap);
}

// @brief ノードの入出力の関係を表すCNF式を作る．
// @param[in] node 対象のノード
// @param[in] var_map 変数マップ
// @param[in] ovar 出力の変数
void
StructEnc::make_node_cnf(const TpgNode* node,
			 const VidMap& var_map,
			 SatVarId ovar)
{
  GateLitMap_vid2 litmap(node, var_map, ovar);
  _make_node_cnf(node, litmap);
}

// @brief ノードの入出力の関係を表すCNF式を作る．
// @param[in] node 対象のノード
// @param[in] litmap 入出力のリテラル
void
StructEnc::_make_node_cnf(const TpgNode* node,
			  const GateLitMap& litmap)
{
  SatLiteral olit = litmap.output();
  ymuint ni = litmap.input_size();
  switch ( node->gate_type() ) {
  case kGateCONST0:
    if ( debug() & debug_make_node_cnf ) {
      cout << "_make_node_cnf(CONST0): " << "| " << olit << endl;
    }
    mSolver.add_clause(~olit);
    break;

  case kGateCONST1:
    if ( debug() & debug_make_node_cnf ) {
      cout << "_make_node_cnf(CONST1): " << "| " << olit << endl;
    }
    mSolver.add_clause( olit);
    break;

  case kGateINPUT:
    // なにもしない．
    break;

  case kGateBUFF:
    if ( debug() & debug_make_node_cnf ) {
      cout << "_make_node_cnf(BUFF): " << litmap.input(0) << "| " << olit << endl;
    }
    {
      SatLiteral ilit = litmap.input(0);
      mSolver.add_eq_rel( ilit,  olit);
    }
    break;

  case kGateNOT:
    if ( debug() & debug_make_node_cnf ) {
      cout << "_make_node_cnf(NOT):  " << litmap.input(0) << "| " << olit << endl;
    }
    {
      SatLiteral ilit = litmap.input(0);
      mSolver.add_eq_rel( ilit, ~olit);
    }
    break;

  case kGateAND:
    if ( debug() & debug_make_node_cnf ) {
      cout << "_make_node_cnf(AND):  ";
      for (ymuint i = 0; i < ni; ++ i) {
	cout << " " << litmap.input(i);
      }
      cout << "| " << olit << endl;
    }
    switch ( ni ) {
    case 2:
      {
	SatLiteral ilit0 = litmap.input(0);
	SatLiteral ilit1 = litmap.input(1);
	mSolver.add_andgate_rel( olit, ilit0, ilit1);
      }
      break;

    case 3:
      {
	SatLiteral ilit0 = litmap.input(0);
	SatLiteral ilit1 = litmap.input(1);
	SatLiteral ilit2 = litmap.input(2);
	mSolver.add_andgate_rel( olit, ilit0, ilit1, ilit2);
      }
      break;

    case 4:
      {
	SatLiteral ilit0 = litmap.input(0);
	SatLiteral ilit1 = litmap.input(1);
	SatLiteral ilit2 = litmap.input(2);
	SatLiteral ilit3 = litmap.input(3);
	mSolver.add_andgate_rel( olit, ilit0, ilit1, ilit2, ilit3);
      }
      break;

    default:
      ASSERT_COND( ni > 4 );
      {
	vector<SatLiteral> ilits(ni);
	for (ymuint i = 0; i < ni; ++ i) {
	  ilits[i] = litmap.input(i);
	}
	mSolver.add_andgate_rel( olit, ilits);
      }
      break;
    }
    break;

  case kGateNAND:
    if ( debug() & debug_make_node_cnf ) {
      cout << "_make_node_cnf(NAND):  ";
      for (ymuint i = 0; i < ni; ++ i) {
	cout << " " << litmap.input(i);
      }
      cout << "| " << olit << endl;
    }
    switch ( ni ) {
    case 2:
      {
	SatLiteral ilit0 = litmap.input(0);
	SatLiteral ilit1 = litmap.input(1);
	mSolver.add_nandgate_rel( olit, ilit0, ilit1);
      }
      break;

    case 3:
      {
	SatLiteral ilit0 = litmap.input(0);
	SatLiteral ilit1 = litmap.input(1);
	SatLiteral ilit2 = litmap.input(2);
	mSolver.add_nandgate_rel( olit, ilit0, ilit1, ilit2);
      }
      break;

    case 4:
      {
	SatLiteral ilit0 = litmap.input(0);
	SatLiteral ilit1 = litmap.input(1);
	SatLiteral ilit2 = litmap.input(2);
	SatLiteral ilit3 = litmap.input(3);
	mSolver.add_nandgate_rel( olit, ilit0, ilit1, ilit2, ilit3);
      }
      break;

    default:
      ASSERT_COND( ni > 4 );
      {
	vector<SatLiteral> ilits(ni);
	for (ymuint i = 0; i < ni; ++ i) {
	  ilits[i] = litmap.input(i);
	}
	mSolver.add_nandgate_rel( olit, ilits);
      }
      break;
    }
    break;

  case kGateOR:
    if ( debug() & debug_make_node_cnf ) {
      cout << "_make_node_cnf(OR):  ";
      for (ymuint i = 0; i < ni; ++ i) {
	cout << " " << litmap.input(i);
      }
      cout << "| " << olit << endl;
    }
    switch ( ni ) {
    case 2:
      {
	SatLiteral ilit0 = litmap.input(0);
	SatLiteral ilit1 = litmap.input(1);
	mSolver.add_orgate_rel( olit, ilit0, ilit1);
      }
      break;

    case 3:
      {
	SatLiteral ilit0 = litmap.input(0);
	SatLiteral ilit1 = litmap.input(1);
	SatLiteral ilit2 = litmap.input(2);
	mSolver.add_orgate_rel( olit, ilit0, ilit1, ilit2);
      }
      break;

    case 4:
      {
	SatLiteral ilit0 = litmap.input(0);
	SatLiteral ilit1 = litmap.input(1);
	SatLiteral ilit2 = litmap.input(2);
	SatLiteral ilit3 = litmap.input(3);
	mSolver.add_orgate_rel( olit, ilit0, ilit1, ilit2, ilit3);
      }
      break;

    default:
      ASSERT_COND( ni > 4 );
      {
	vector<SatLiteral> ilits(ni);
	for (ymuint i = 0; i < ni; ++ i) {
	  ilits[i] = litmap.input(i);
	}
	mSolver.add_orgate_rel( olit, ilits);
      }
      break;
    }
    break;

  case kGateNOR:
    if ( debug() & debug_make_node_cnf ) {
      cout << "_make_node_cnf(NOR):  ";
      for (ymuint i = 0; i < ni; ++ i) {
	cout << " " << litmap.input(i);
      }
      cout << "| " << olit << endl;
    }
    switch ( ni ) {
    case 2:
      {
	SatLiteral ilit0 = litmap.input(0);
	SatLiteral ilit1 = litmap.input(1);
	mSolver.add_norgate_rel( olit, ilit0, ilit1);
      }
      break;

    case 3:
      {
	SatLiteral ilit0 = litmap.input(0);
	SatLiteral ilit1 = litmap.input(1);
	SatLiteral ilit2 = litmap.input(2);
	mSolver.add_norgate_rel( olit, ilit0, ilit1, ilit2);
      }
      break;

    case 4:
      {
	SatLiteral ilit0 = litmap.input(0);
	SatLiteral ilit1 = litmap.input(1);
	SatLiteral ilit2 = litmap.input(2);
	SatLiteral ilit3 = litmap.input(3);
	mSolver.add_norgate_rel( olit, ilit0, ilit1, ilit2, ilit3);
      }
      break;

    default:
      ASSERT_COND( ni > 4 );
      {
	vector<SatLiteral> ilits(ni);
	for (ymuint i = 0; i < ni; ++ i) {
	  ilits[i] = litmap.input(i);
	}
	mSolver.add_norgate_rel( olit, ilits);
      }
      break;
    }
    break;

  case kGateXOR:
    if ( debug() & debug_make_node_cnf ) {
      cout << "_make_node_cnf(XOR):  ";
      for (ymuint i = 0; i < ni; ++ i) {
	cout << " " << litmap.input(i);
      }
      cout << "| " << olit << endl;
    }
    ASSERT_COND( ni == 2 );
    {
      SatLiteral ilit0 = litmap.input(0);
      SatLiteral ilit1 = litmap.input(1);
      mSolver.add_xorgate_rel( olit, ilit0, ilit1);
    }
    break;

  case kGateXNOR:
    if ( debug() & debug_make_node_cnf ) {
      cout << "_make_node_cnf(XNOR):  ";
      for (ymuint i = 0; i < ni; ++ i) {
	cout << " " << litmap.input(i);
      }
      cout << "| " << olit << endl;
    }
    ASSERT_COND( ni == 2 );
    {
      SatLiteral ilit0 = litmap.input(0);
      SatLiteral ilit1 = litmap.input(1);
      mSolver.add_xnorgate_rel( olit, ilit0, ilit1);
    }
    break;

  default:
    ASSERT_NOT_REACHED;
    break;
  }
}

END_NAMESPACE_YM_SATPG_STRUCTENC
