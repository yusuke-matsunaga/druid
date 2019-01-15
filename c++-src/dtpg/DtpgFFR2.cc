
/// @file DtpgFFR2.cc
/// @brief DtpgFFR2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgFFR2.h"

#include "TpgFault.h"
#include "TpgFFR.h"
#include "NodeValList.h"
#include "TestVector.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
// @param[in] sat_type SATソルバの種類を表す文字列
// @param[in] sat_option SATソルバに渡すオプション文字列
// @param[in] sat_outp SATソルバ用の出力ストリーム
// @param[in] fault_type 故障の種類
// @param[in] bt バックトレーサー
// @param[in] network 対象のネットワーク
// @param[in] root 故障伝搬の起点となるノード
DtpgFFR2::DtpgFFR2(const string& sat_type,
		   const string& sat_option,
		   ostream* sat_outp,
		   FaultType fault_type,
		   const string& just_type,
		   const TpgNetwork& network,
		   const TpgFFR& ffr) :
  DtpgEngine(sat_type, sat_option, sat_outp, fault_type, just_type, network, ffr.root())
{
  cnf_begin();

  gen_undetect_cnf();

  //gen_ffr2_cnf();

  cnf_end();
}

// @brief デストラクタ
DtpgFFR2::~DtpgFFR2()
{
}

// @brief fault が検出不能か調べる．
// @param[in] fault 対象の故障
// @param[in] condition 制約条件
// @return 結果を返す．
SatBool3
DtpgFFR2::check_untestable(const TpgFault* fault,
			   const NodeValList& condition)
{
  const TpgNode* ffr_root = fault->tpg_onode()->ffr_root();
  ASSERT_COND( ffr_root == root_node() );

  // fault の非検出条件を作る．
  SatVarId nvar = solver().new_variable();
  SatLiteral nlit(nvar);
#if 0
  auto node = fault->tpg_onode();
  SatLiteral dlit = get_plit(node);
  NodeValList tmp_cond;
  if ( fault->is_branch_fault() ) {
    add_side_input(node, fault->tpg_pos(), tmp_cond);
  }
  vector<SatLiteral> tmp_lits;
  tmp_lits.reserve(tmp_cond.size() + 2);
  tmp_lits.push_back(~nlit);
  for ( auto nv: tmp_cond ) {
    auto inode = nv.node();
    bool inv = (nv.val() == 0);
    SatLiteral ilit(gvar(inode), inv);
    tmp_lits.push_back(~ilit);
  }
  tmp_lits.push_back(dlit);
#else
  NodeValList ffr_cond = ffr_propagate_condition(fault, fault_type());
  vector<SatLiteral> tmp_lits;
  tmp_lits.reserve(ffr_cond.size() + 1);
  tmp_lits.push_back(~nlit);
  for ( auto nv: ffr_cond ) {
    SatLiteral lit = conv_to_literal(nv);
    tmp_lits.push_back(~lit);
  }
#endif
  solver().add_clause(tmp_lits);

  // condition の内容を assumptions に追加する．
  vector<SatLiteral> assumptions;
  conv_to_assumptions(condition, assumptions);
  assumptions.push_back(nlit);

  vector<SatBool3> model;
  SatBool3 sat_res = solve(assumptions, model);
  return sat_res;
}

BEGIN_NONAMESPACE

void
dfs(const TpgNode* node,
    vector<const TpgNode*>& node_list)
{
  if ( node->ffr_root() == node ) {
    // 他の FFR のノードだった．
    return;
  }

  node_list.push_back(node);
  for ( auto inode: node->fanin_list() ) {
    dfs(inode, node_list);
  }
}

END_NONAMESPACE

// @brief FFR 内の故障差が伝搬しない条件を作る．
void
DtpgFFR2::gen_ffr2_cnf()
{
  // FFR 内のノードを DFS の preorder でリストに入れる．
  vector<const TpgNode*> tmp_list;
  for ( auto node: root_node()->fanin_list() ) {
    dfs(node, tmp_list);
  }

  for ( auto node: tmp_list ) {
    SatVarId dvar1 = new_variable();
    SatLiteral dlit1(dvar1);
    mPvarMap.add(node->id(), dlit1);
    vector<SatLiteral> tmp_lits;
    const TpgNode* onode = node->fanout(0);
    SatLiteral dlit2 = get_plit(onode);
    NodeValList tmp_cond;
    //add_side_input(onode, node, tmp_cond);
    tmp_lits.reserve(tmp_cond.size() + 2);
    tmp_lits.push_back(~dlit1);
    for ( auto nv: tmp_cond ) {
      auto inode = nv.node();
      bool inv = (nv.val() == 0);
      SatLiteral ilit(gvar(inode), inv);
      tmp_lits.push_back(~ilit);
    }
    tmp_lits.push_back(dlit2);
    solver().add_clause(tmp_lits);
  }
}

// @brief node の plit を得る．
SatLiteral
DtpgFFR2::get_plit(const TpgNode* node)
{
  if ( node == root_node() ) {
    return SatLiteral(dvar(node));
  }
  else {
    SatLiteral lit;
    bool res = mPvarMap.find(node->id(), lit);
    ASSERT_COND( res );
    return lit;
  }
}

END_NAMESPACE_DRUID
