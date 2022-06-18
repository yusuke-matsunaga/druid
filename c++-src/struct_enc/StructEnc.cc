
/// @file StructEnc.cc
/// @brief StructEnc の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2015, 2017, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "StructEnc.h"
#include "SimplePropCone.h"
#include "MffcPropCone.h"
#include "Justifier.h"
#include "NodeValList.h"
#include "TestVector.h"

#include "TpgNetwork.h"
#include "TpgFault.h"
#include "TpgNode.h"
#include "TpgDff.h"
#include "TpgMFFC.h"

#include "GateType.h"
#include "GateEnc.h"

#include "Val3.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID_STRUCTENC

BEGIN_NONAMESPACE

const int debug_make_vars = 1U;
const int debug_make_node_cnf = 2U;
const int debug_extract = 32U;
const int debug_justify = 64U;

END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// クラス StructEnc
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
StructEnc::StructEnc(
  const TpgNetwork& network,
  FaultType fault_type,
  const SatSolverType& solver_type
) : mNetwork{network},
    mFaultType{fault_type},
    mSolver{solver_type},
    mMaxId{network.node_num()},
    mMark(mMaxId, 0)
{
  for (int i = 0; i < 2; ++ i) {
    mVarMap[i].init(mMaxId);
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
  for ( auto cone: mConeList ) {
    delete cone;
  }
}

// @brief fault cone を追加する．
int
StructEnc::add_simple_cone(
  const TpgNode* fnode,
  bool detect
)
{
  return add_simple_cone(fnode, nullptr, detect);
}

// @brief fault cone を追加する．
int
StructEnc::add_simple_cone(
  const TpgNode* fnode,
  const TpgNode* bnode,
  bool detect
)
{
  auto focone = new SimplePropCone(*this, fnode, bnode, detect);
  int cone_id = mConeList.size();
  mConeList.push_back(focone);

  if ( fault_type() == FaultType::TransitionDelay ) {
    add_prev_node(fnode);
  }
  make_tfi_list(focone->tfo_node_list());

  return cone_id;
}

// @brief MFFC cone を追加する．
int
StructEnc::add_mffc_cone(
  const TpgMFFC& mffc,
  bool detect
)
{
  return add_mffc_cone(mffc, nullptr, detect);
}

// @brief MFFC cone を追加する．
int
StructEnc::add_mffc_cone(
  const TpgMFFC& mffc,
  const TpgNode* bnode,
  bool detect
)
{
  auto mffccone = new MffcPropCone(*this, mffc, bnode, detect);
  int cone_id = mConeList.size();
  mConeList.push_back(mffccone);

  if ( fault_type() == FaultType::TransitionDelay ) {
    add_prev_node(mffc.root());
  }
  make_tfi_list(mffccone->tfo_node_list());

  return cone_id;
}

// @brief 故障を検出する条件を作る．
vector<SatLiteral>
StructEnc::make_fault_condition(
  const TpgFault* fault,
  int cone_id
)
{
  // FFR 内の故障伝搬条件を assign_list に入れる．
  NodeValList assign_list;
  const TpgNode* ffr_root = fault->tpg_onode()->ffr_root();
  add_ffr_condition(ffr_root, fault, assign_list);

  /// FFR より出力側の故障伝搬条件を assumptions に入れる．
  ASSERT_COND( cone_id < mConeList.size() );
  auto assumptions = mConeList[cone_id]->make_prop_condition(ffr_root);

  // assign_list を変換して assumptions に追加する．
  auto as2 = conv_to_literal_list(assign_list);
  assumptions.insert(assumptions.end(), as2.begin(), as2.end());
  return assumptions;
}

// @brief 故障の検出条件を割当リストに追加する．
void
StructEnc::add_fault_condition(
  const TpgFault* fault,
  NodeValList& assign_list
)
{
  // 故障の活性化条件
  const TpgNode* inode = fault->tpg_inode();
  // 0縮退故障の時 1にする．
  bool val = (fault->val() == 0);
  assign_list.add(inode, 1, val);

  if ( fault_type() == FaultType::TransitionDelay ) {
    // 1時刻前の値が逆の値である条件を作る．
    assign_list.add(inode, 0, !val);
  }

  // ブランチの故障の場合，ゲートの出力までの伝搬条件を作る．
  if ( fault->is_branch_fault() ) {
    // 故障の伝搬条件
    const TpgNode* onode = fault->tpg_onode();
    Val3 nval = onode->nval();
    if ( nval != Val3::_X ) {
      bool val = (nval == Val3::_1);
      // inode -> onode の伝搬条件
      for ( auto ipos: Range(onode->fanin_num()) ) {
	if ( ipos != fault->tpg_pos() ) {
	  auto inode1 = onode->fanin(ipos);
	  assign_list.add(inode1, 1, val);
	}
      }
    }
  }
}

// @brief FFR内の故障の伝搬条件を割当リストに追加する．
void
StructEnc::add_ffr_condition(
  const TpgNode* root_node,
  const TpgFault* fault,
  NodeValList& assign_list
)
{
  // ノードに対する故障の伝搬条件
  add_fault_condition(fault, assign_list);

  // FFR の根までの伝搬条件
  for ( const TpgNode* node = fault->tpg_onode(); node != root_node;
       node = node->fanout_list()[0] ) {
    ASSERT_COND( node->fanout_num() == 1 );
    const TpgNode* onode = node->fanout_list()[0];
    int ni = onode->fanin_num();
    if ( ni == 1 ) {
      // サイドインプットがなければスキップ
      continue;
    }
    Val3 nval = onode->nval();
    if ( nval == Val3::_X ) {
      // 非制御値がなければスキップ
      // 実際には制御値がない時にスキップ
      // 例えば XOR ゲートの場合がそれにあたる．
      continue;
    }
    // サイドインプットの値を非制御値にする．
    bool val = (nval == Val3::_1);
    for ( auto inode: onode->fanin_list() ) {
      if ( inode == node ) {
	continue;
      }
      assign_list.add(inode, 1, val);
    }
  }
}

// @brief 割当リストに従って値を固定する．
void
StructEnc::add_assignments(
  const NodeValList& assignment
)
{
  for ( auto nv: assignment ) {
    SatLiteral alit = nv_to_lit(nv);
    mSolver.add_clause(alit);
  }
}

// @brief 割当リストの否定の節を加える．
void
StructEnc::add_negation(
  const NodeValList& assignment
)
{
  vector<SatLiteral> tmp_lits;
  tmp_lits.reserve(assignment.size());
  for ( auto nv: assignment ) {
    SatLiteral alit = nv_to_lit(nv);
    tmp_lits.push_back(~alit);
  }
  mSolver.add_clause(tmp_lits);
}

// @brief 割当リストを仮定のリテラルに変換する．
//
vector<SatLiteral>
StructEnc::conv_to_literal_list(
  const NodeValList& assign_list
)
{
  vector<SatLiteral> ans_list;
  ans_list.reserve(assign_list.size());
  for ( auto nv: assign_list ) {
    SatLiteral alit = nv_to_lit(nv);
    ans_list.push_back(alit);
  }
  return ans_list;
}

// @brief 与えられたノード(のリスト)のTFIのリストを作る．
void
StructEnc::make_tfi_list(
  const vector<const TpgNode*>& node_list
)
{
  // node_list を mCurNodeList に入れる．
  for ( auto node: node_list ) {
    if ( !cur_mark(node) ) {
      add_cur_node(node);
    }
  }
  for ( int rpos = 0; rpos < mCurNodeList.size(); ++ rpos ) {
    const TpgNode* node = mCurNodeList[rpos];

    // node のファンインを mCurNodeList に追加する．
    for ( auto inode: node->fanin_list() ) {
      if ( !cur_mark(inode) ) {
	add_cur_node(inode);
      }
    }

    // 遷移故障モードの場合には１時刻前の回路も作る．
    if ( fault_type() == FaultType::TransitionDelay && node->is_dff_output() ) {
      const TpgNode* inode = node->dff()->input();
      add_prev_node(inode);
    }
  }

  for ( int rpos = 0; rpos < mPrevNodeList.size(); ++ rpos ) {
    const TpgNode* node = mPrevNodeList[rpos];

    // node のファンインを mCurNodeList に追加する．
    for ( auto inode: node->fanin_list() ) {
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
  for ( int i = 0; i < mCurNodeList.size(); ++ i ) {
    const TpgNode* node = mCurNodeList[i];
    if ( !var_mark(node, 1) ) {
      set_new_var(node, 1);
      if ( debug() & debug_make_vars ) {
	cout << mNetwork.node_name(node->id()) << "@1 -> " << var(node, 1) << endl;
      }
    }
  }
  for ( auto node: mPrevNodeList ) {
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
	cout << mNetwork.node_name(node->id()) << "@0 -> " << var(node, 0) << endl;
      }
    }
  }

  for ( auto focone: mConeList ) {
    focone->make_vars();
  }
}

// @brief 関係あるノードの入出力の関係を表すCNFを作る．
void
StructEnc::make_cnf()
{
  GateEnc gate_enc1(mSolver, var_map(1));
  for ( auto node: mCurNodeList ) {
    if ( !cnf_mark(node, 1) ) {
      set_cnf_mark(node, 1);
      gate_enc1.make_cnf(node);
    }
  }

  GateEnc gate_enc0(mSolver, var_map(0));
  for ( auto node: mPrevNodeList ) {
    if ( !cnf_mark(node, 0) ) {
      set_cnf_mark(node, 0);
      gate_enc0.make_cnf(node);
    }
  }

  for ( auto focone: mConeList ) {
    focone->make_cnf();
  }
}

// @brief node の TFI の部分に変数を割り当てる．
void
StructEnc::make_tfi_var(
  const TpgNode* node,
  int time
)
{
  if ( var_mark(node, time) ) {
    return;
  }
  set_new_var(node, time);

  // 先に TFI のノードの変数を作る．
  for ( auto inode: node->fanin_list() ) {
    make_tfi_var(inode, time);
  }

  // 遷移故障モードの時は前の時刻の回路も作る．
  if ( fault_type() == FaultType::TransitionDelay &&
       node->is_dff_output() && time == 1 ) {
    const TpgNode* inode = node->dff()->input();
    make_tfi_var(inode, 0);
  }
}

// @brief node の TFI の CNF を作る．
void
StructEnc::make_tfi_cnf(
  const TpgNode* node,
  int time
)
{
  if ( cnf_mark(node, time) ) {
    return;
  }
  set_cnf_mark(node, time);

  make_tfi_var(node, time);

  // node の入出力の関係を表す節を作る．
  GateEnc gate_enc(mSolver, var_map(time));
  gate_enc.make_cnf(node);

  // TFI のノードの節を作る．
  for ( auto inode: node->fanin_list() ) {
    make_tfi_cnf(inode, time);
  }

  // 遷移故障モードの時は前の時刻の回路も作る．
  if ( fault_type() == FaultType::TransitionDelay &&
       node->is_dff_output() && time == 1 ) {
    const TpgNode* inode = node->dff()->input();
    make_tfi_cnf(inode, 0);
  }
}

// @brief チェックを行う．
//
// こちらは結果のみを返す．
SatBool3
StructEnc::check_sat()
{
  return mSolver.solve();
}

// @brief 割当リストのもとでチェックを行う．
//
// こちらは結果のみを返す．
SatBool3
StructEnc::check_sat(
  const NodeValList& assign_list  ///< [in] 割当リスト
)
{
  auto assumptions = conv_to_literal_list(assign_list);
  return mSolver.solve(assumptions);
}

// @brief 割当リストのもとでチェックを行う．
SatBool3
StructEnc::check_sat(
  const NodeValList& assign_list1, ///< [in] 割当リスト1
  const NodeValList& assign_list2  ///< [in] 割当リスト2
)
{
  auto assumptions = conv_to_literal_list(assign_list1);
  auto assumptions2 = conv_to_literal_list(assign_list2);
  assumptions.insert(assumptions.end(), assumptions2.begin(), assumptions2.end());
  return mSolver.solve(assumptions);
}

/// @brief 結果のなかで必要なものだけを取り出す．
NodeValList
StructEnc::extract(
  const SatModel& model,
  const TpgFault* fault,
  int cone_id
)
{
  if ( debug() & debug_extract ) {
    cout << endl
	 << "StructEnc::extract(" << fault->str() << ")" << endl;
  }

  NodeValList assign_list;

  // fault から FFR の根までの条件を求める．
  const TpgNode* ffr_root = fault->tpg_onode()->ffr_root();
  add_ffr_condition(ffr_root, fault, assign_list);

  // ffr_root より先の条件を求める．
  ASSERT_COND( cone_id < mConeList.size() );
  NodeValList assign_list2 = mConeList[cone_id]->extract(model, ffr_root);
  assign_list.merge(assign_list2);

  if ( debug() & debug_extract ) {
    cout << "  result = " << assign_list << endl;
  }

  return assign_list;
}

// @brief 外部入力の値割り当てを求める．
TestVector
StructEnc::justify(
  const SatModel& model,
  const NodeValList& assign_list,
  Justifier& justifier
)
{
  if ( debug() & debug_justify ) {
    cout << endl
	 << "StructEnc::justify(" << assign_list << ")" << endl;
  }

  TestVector testvect;

  if ( mFaultType == FaultType::TransitionDelay ) {
    testvect = justifier(assign_list, var_map(0), var_map(1), model);
  }
  else {
    testvect = justifier(assign_list, var_map(1), model);
  }

  if ( debug() & debug_justify ) {
    cout << " => " << testvect.bin_str() << endl;
  }

  return testvect;
}

END_NAMESPACE_DRUID_STRUCTENC
