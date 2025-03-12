
/// @file CnfGenAig.cc
/// @brief CnfGenAig の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "CnfGenAig.h"
#include "ym/SopCover.h"
#include "ym/Expr.h"
#include "ym/AigMgr.h"
#include "TpgNetwork.h"
#include "Expr2Aig.h"
#include "AigCnfCalc.h"



BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

void
dfs(
  const AigHandle& aig,
  std::unordered_set<AigHandle>& mark,
  std::vector<SizeType>& input_id_list
)
{
  if ( aig.is_const() ) {
    return;
  }
  if ( mark.count(aig) > 0 ) {
    return;
  }
  mark.emplace(aig);

  if ( aig.is_input() ) {
    auto input_id = aig.input_id();
    input_id_list.push_back(input_id);
    return;
  }

  dfs(aig.fanin0(), mark, input_id_list);
  dfs(aig.fanin1(), mark, input_id_list);
}

END_NONAMESPACE

// @brief Expr のリストから CNF を作る．
vector<vector<SatLiteral>>
CnfGenAig::expr_to_cnf(
  StructEngine& engine,
  const vector<Expr>& expr_list
)
{
  AigMgr mgr;
  Expr2Aig expr2aig(mgr);
  auto aig_list = expr2aig.conv_to_aig(expr_list);

  // aig_list 中に現れる入力番号のリストを得る．
  std::vector<SizeType> input_id_list;
  std::unordered_set<AigHandle> mark;
  for ( auto& aig: aig_list ) {
    dfs(aig, mark, input_id_list);
  }

  // 入力番号と SatLiteral の対応を表す辞書
  SatSolver::LitMap lit_map;
  for ( SizeType input_id: input_id_list ) {
    auto node_id = input_id / 2;
    auto time = input_id % 2;
    auto node = engine.network().node(node_id);
    auto as = Assign(node, time, true);
    auto lit = engine.conv_to_literal(as);
    lit_map.emplace(input_id, lit);
  }

  return engine.solver().add_aig(aig_list, lit_map);
}

// @brief Expr のリストから CNF サイズを見積もる．
CnfSize
CnfGenAig::expr_cnf_size(
  const vector<Expr>& expr_list
)
{
  AigMgr mgr;
  Expr2Aig expr2aig(mgr);
  auto aig_list = expr2aig.conv_to_aig(expr_list);

  AigCnfCalc calc;
  for ( auto& aig: aig_list ) {
    calc.calc_size(aig);
  }
  auto size = calc.size();
  return size;
}

END_NAMESPACE_DRUID
