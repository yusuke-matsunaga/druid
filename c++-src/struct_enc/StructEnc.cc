
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
#include "TpgDFF.h"
#include "TpgMFFC.h"
#include "TpgNodeSet.h"

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
    mMaxId{network.node_num()}
{
  for (int i = 0; i < 2; ++ i) {
    mVarMap[i].init(mMaxId);
  }

  mDebugFlag = 0;
  //mDebugFlag |= debug_extract;
  //mDebugFlag |= debug_justify;
}

// @brief デストラクタ
StructEnc::~StructEnc()
{
}

// @brief fault cone を追加する．
SizeType
StructEnc::add_simple_cone(
  const TpgNode* fnode,
  const TpgNode* bnode,
  bool detect
)
{
  auto focone = new SimplePropCone{*this, fnode, bnode, detect};
  SizeType cone_id = mConeList.size();
  mConeList.push_back(unique_ptr<PropCone>{focone});
  return cone_id;
}

// @brief MFFC cone を追加する．
SizeType
StructEnc::add_mffc_cone(
  const TpgMFFC& mffc,
  const TpgNode* bnode,
  bool detect
)
{
  auto mffccone = new MffcPropCone{*this, mffc, bnode, detect};
  SizeType cone_id = mConeList.size();
  mConeList.push_back(unique_ptr<PropCone>{mffccone});
  return cone_id;
}

/// @brief 故障の伝搬条件を求める．
vector<SatLiteral>
StructEnc::make_prop_condition(
  const TpgNode* ffr_root,
  SizeType cone_id
)
{
  /// FFR より出力側の故障伝搬条件を assumptions に入れる．
  ASSERT_COND( cone_id < mConeList.size() );
  return mConeList[cone_id]->make_condition(ffr_root);
}

// @brief 割当リストに従って値を固定する．
void
StructEnc::add_assignments(
  const NodeValList& assignment
)
{
  for ( auto nv: assignment ) {
    auto alit = nv_to_lit(nv);
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
    auto alit = nv_to_lit(nv);
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
    auto alit = nv_to_lit(nv);
    ans_list.push_back(alit);
  }
  return ans_list;
}

// @brief 関係あるノードに変数を割り当てる．
void
StructEnc::make_vars()
{
  vector<const TpgNode*> tmp_list;
  for ( auto& focone: mConeList ) {
    auto& src_list = focone->tfo_node_list();
    tmp_list.insert(tmp_list.end(), src_list.begin(), src_list.end());
  }

  mCurNodeList = TpgNodeSet::get_tfi_list(max_node_id(), tmp_list);

  if ( fault_type() == FaultType::TransitionDelay ) {
    tmp_list.clear();
    for ( auto& focone: mConeList ) {
      auto root = focone->root_node();
      tmp_list.push_back(root);
      if ( root->is_dff_output() ) {
	tmp_list.push_back(root->alt_node());
      }
    }
    for ( auto node: mCurNodeList ) {
      if ( node->is_dff_output() ) {
	auto inode = node->alt_node();
	tmp_list.push_back(inode);
      }
    }
    mPrevNodeList = TpgNodeSet::get_tfi_list(max_node_id(), tmp_list);
  }

  for ( auto node: mCurNodeList ) {
    set_new_var(node, 1);
    if ( debug() & debug_make_vars ) {
      cout << mNetwork.node_name(node->id()) << "@1 -> " << var(node, 1) << endl;
    }
  }
  for ( auto node: mPrevNodeList ) {
    // FF の入力の場合は1時刻後の出力の変数を用いる．
    if ( node->is_dff_input() ) {
      auto onode = node->alt_node();
      _set_var(node, 0, var(onode, 1));
    }
    else {
      set_new_var(node, 0);
    }
    if ( debug() & debug_make_vars ) {
      cout << mNetwork.node_name(node->id()) << "@0 -> " << var(node, 0) << endl;
    }
  }

  for ( auto& focone: mConeList ) {
    focone->make_vars();
  }
}

// @brief 関係あるノードの入出力の関係を表すCNFを作る．
void
StructEnc::make_cnf()
{
  GateEnc gate_enc1(mSolver, var_map(1));
  for ( auto node: mCurNodeList ) {
    gate_enc1.make_cnf(node);
  }

  GateEnc gate_enc0(mSolver, var_map(0));
  for ( auto node: mPrevNodeList ) {
    gate_enc0.make_cnf(node);
  }

  for ( auto& focone: mConeList ) {
    focone->make_cnf();
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

// @brief 伝搬条件を求める．
NodeValList
StructEnc::extract_prop_condition(
  const TpgNode* ffr_root,
  SizeType cone_id,
  const SatModel& model
)
{
  ASSERT_COND( cone_id < mConeList.size() );
  return mConeList[cone_id]->extract_condition(model, ffr_root);
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

  auto testvect = justifier(mFaultType, assign_list, var_map(0), var_map(1), model);

  if ( debug() & debug_justify ) {
    cout << " => " << testvect.bin_str() << endl;
  }

  return testvect;
}

END_NAMESPACE_DRUID_STRUCTENC
