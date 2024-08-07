
/// @file StructEnc.cc
/// @brief StructEnc の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "StructEnc.h"
#include "SimplePropCone.h"
#include "MffcPropCone.h"
#include "NodeValList.h"

#include "TpgNetwork.h"
#include "TpgFault.h"
#include "TpgNode.h"
#include "TpgMFFC.h"
#include "TpgNodeSet.h"
#include "GateEnc.h"
#include "Val3.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

const int debug_make_vars = 1U;
const int debug_make_node_cnf = 2U;
const int debug_extract = 32U;
const int debug_justify = 64U;

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

END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// クラス StructEnc
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
StructEnc::StructEnc(
  const TpgNetwork& network,
  const JsonValue& option
) : mNetwork{network},
    mHasPrevState{network.has_prev_state()},
    mSolver{get_sat_param(option)},
    mMaxId{network.node_num()},
    mGvarMap(mMaxId),
    mHvarMap(mMaxId)
{
  mDebugFlag = 0;
  //mDebugFlag |= debug_extract;
  //mDebugFlag |= debug_justify;
}

// @brief デストラクタ
StructEnc::~StructEnc()
{
}

// @brief fault cone を追加する．
SatLiteral
StructEnc::add_simple_cone(
  const TpgNode* fnode
)
{
  auto focone = new SimplePropCone{*this, fnode};
  SizeType cone_id = mConeList.size();
  mConeList.push_back(unique_ptr<PropCone>{focone});
  mConeDict.emplace(fnode->id(), cone_id);
  return focone->prop_var();
}

// @brief MFFC cone を追加する．
SatLiteral
StructEnc::add_mffc_cone(
  const TpgMFFC* mffc
)
{
  if ( mffc->ffr_num() == 1 ) {
    // FFRモードで作る．
    return add_simple_cone(mffc->root());
  }
  auto mffccone = new MffcPropCone{*this, mffc};
  SizeType cone_id = mConeList.size();
  mConeList.push_back(unique_ptr<PropCone>{mffccone});
  for ( auto ffr: mffc->ffr_list() ) {
    auto root = ffr->root();
    mConeDict.emplace(root->id(), cone_id);
  }
  return mffccone->prop_var();
}

// @brief 故障の伝搬条件を求める．
vector<SatLiteral>
StructEnc::make_prop_condition(
  const TpgNode* ffr_root
)
{
  ASSERT_COND( mConeDict.count(ffr_root->id()) > 0 );
  auto cone_id = mConeDict.at(ffr_root->id());
  // FFR より出力側の故障伝搬条件を assumptions に入れる．
  return mConeList[cone_id]->make_condition(ffr_root);
}

// @brief 割当リストに従って値を固定する．
void
StructEnc::add_assignments(
  const AssignList& assignments
)
{
  for ( auto nv: assignments ) {
    auto alit = nv_to_lit(nv);
    mSolver.add_clause(alit);
  }
}

// @brief 割当リストの否定の節を加える．
void
StructEnc::add_negation(
  const AssignList& assignments
)
{
  vector<SatLiteral> tmp_lits;
  tmp_lits.reserve(assignments.size());
  for ( auto nv: assignments ) {
    auto alit = nv_to_lit(nv);
    tmp_lits.push_back(~alit);
  }
  mSolver.add_clause(tmp_lits);
}

// @brief 割当リストを仮定のリテラルに変換する．
//
vector<SatLiteral>
StructEnc::conv_to_literal_list(
  const AssignList& assign_list
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
  // PropCone の TFO を tmp_list に入れる．
  vector<const TpgNode*> tmp_list;
  for ( auto& focone: mConeList ) {
    auto& src_list = focone->tfo_node_list();
    tmp_list.insert(tmp_list.end(), src_list.begin(), src_list.end());
  }

  if ( mHasPrevState ) {
    // tmp_list の TFI を mCurNodeList に入れる．
    // そのうちの DFF の出力に対しては対応する入力を mDffInputList に入れる．
    mCurNodeList = TpgNodeSet::get_tfi_list(max_node_id(), tmp_list,
					    [&](const TpgNode* node) {
					      tfi_hook(node);
					    });
    // mDffInputList と PropCone の根のノードを tmp_list に入れる．
    // 根のノードが DFF の出力の場合には対応する入力も tmp_list に入れる．
    tmp_list = mDffInputList;
    for ( auto& focone: mConeList ) {
      auto root = focone->root_node();
      tmp_list.push_back(root);
      if ( root->is_dff_output() ) {
	tmp_list.push_back(root->alt_node());
      }
    }
    // tmp_list の TFI を mPrevNodeList に入れる．
    mPrevNodeList = TpgNodeSet::get_tfi_list(max_node_id(), tmp_list);
  }
  else {
    // tmp_list の TFI を mCurNodeList に入れる．
    mCurNodeList = TpgNodeSet::get_tfi_list(max_node_id(), tmp_list);
  }

  for ( auto node: mCurNodeList ) {
    auto var = mSolver.new_variable(true);
    set_gvar(node, var);
    if ( debug() & debug_make_vars ) {
      cout << node_name(node) << ": gvar = " << var << endl;
    }
  }
  for ( auto node: mPrevNodeList ) {
    auto var = mSolver.new_variable(true);
    set_hvar(node, var);
    if ( debug() & debug_make_vars ) {
      cout << node_name(node) << ": hvar = " << var << endl;
    }
  }

  for ( auto& focone: mConeList ) {
    focone->make_vars(mCurNodeList);
  }
}

// @brief 関係あるノードの入出力の関係を表すCNFを作る．
void
StructEnc::make_cnf()
{
  // 変数の割り当てを行う．
  make_vars();

  GateEnc gate_enc1(mSolver, gvar_map());
  for ( auto node: mCurNodeList ) {
    gate_enc1.make_cnf(node);
  }

  GateEnc gate_enc0(mSolver, hvar_map());
  for ( auto node: mPrevNodeList ) {
    gate_enc0.make_cnf(node);
  }

  for ( auto node: mDffInputList ) {
    auto onode = node->alt_node();
    auto olit = gvar(onode);
    auto ilit = hvar(node);
    if ( olit == SatLiteral::X ) {
      cout << node_name(onode) << ": gvar = X" << endl;
      abort();
    }
    if ( ilit == SatLiteral::X ) {
      cout << node_name(node) << ": hvar = X" << endl;
      abort();
    }
    mSolver.add_buffgate(olit, ilit);
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
  const AssignList& assign_list
)
{
  auto assumptions = conv_to_literal_list(assign_list);
  return mSolver.solve(assumptions);
}

// @brief 割当リストのもとでチェックを行う．
SatBool3
StructEnc::check_sat(
  const AssignList& assign_list1,
  const AssignList& assign_list2
)
{
  auto assumptions = conv_to_literal_list(assign_list1);
  auto assumptions2 = conv_to_literal_list(assign_list2);
  assumptions.insert(assumptions.end(), assumptions2.begin(), assumptions2.end());
  return mSolver.solve(assumptions);
}

// @brief 割当リストのもとでチェックを行う．
SatBool3
StructEnc::check_sat(
  const vector<SatLiteral>& assumptions,
  const AssignList& assign_list
)
{
  auto tmp_list1 = conv_to_literal_list(assign_list);
  auto tmp_list = assumptions;
  tmp_list.insert(tmp_list.end(), tmp_list1.begin(), tmp_list1.end());
  return mSolver.solve(tmp_list);
}

// @brief 伝搬条件を求める．
AssignList
StructEnc::extract_prop_condition(
  const TpgNode* ffr_root
)
{
  ASSERT_COND( mConeDict.count(ffr_root->id()) > 0 );
  auto cone_id = mConeDict.at(ffr_root->id());
  auto& model = mSolver.model();
  return mConeList[cone_id]->extract_condition(model, ffr_root);
}

END_NAMESPACE_DRUID
