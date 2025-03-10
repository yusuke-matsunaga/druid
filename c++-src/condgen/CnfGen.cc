
/// @file CnfGen.cc
/// @brief CnfGen の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "CnfGen.h"
#include "TpgNetwork.h"
#include "TpgFFR.h"
#include "StructEngine.h"
#include "BoolDiffEnc.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス CnfGen
//////////////////////////////////////////////////////////////////////

// @brief 条件を CNF に変換する．
vector<vector<SatLiteral>>
CnfGen::make_cnf(
  StructEngine& engine,
  const vector<DetCond>& cond_list
)
{
  // 条件を Expr に変換する．
  auto expr_list = _make_expr_list(cond_list);

  // Expr のリストからCNFを作る．
  auto lits_list = expr_to_cnf(engine, expr_list);

  // overflow した出力を持つ場合に追加のエンコーダを作る．
  auto n = cond_list.size();
  for ( SizeType i = 0; i < n; ++ i ) {
    auto& cond = cond_list[i];
#if 0
    if ( cond.type() == DetCond::PartialDetected && !cond.output_list().empty() ) {
      auto bd_enc = new BoolDiffEnc(cond.root(), cond.output_list());
      engine.add_subenc(std::unique_ptr<SubEnc>{bd_enc});
      auto plit = bd_enc->prop_var();
      auto old_lits = lits_list[i];
      auto and_lit = engine.solver().new_variable(false);
      for ( auto lit: old_lits ) {
	engine.solver().add_clause(~and_lit, lit);
      }
      auto new_lit = engine.solver().new_variable(true);
      engine.solver().add_clause(~new_lit, and_lit, plit);
      lits_list[i] = {new_lit};
    }
    else if ( cond.type() == DetCond::Overflow ) {
      auto bd_enc = new BoolDiffEnc(engine, cond.root(), cond.output_list());
      if ( !lits_list[i].empty() ) {
	throw std::logic_error{"lits_list for overflowed condition should be empty"};
      }
      lits_list[i] = {bd_enc->prop_var()};
    }
#else
    if ( cond.type() == DetCond::PartialDetected || cond.type() == DetCond::Overflow ) {
      ; // 未完
    }
#endif
  }

  return lits_list;
}

// @brief 条件を CNF に変換した時の CNF のサイズを見積もる．
CnfSize
CnfGen::calc_cnf_size(
  const TpgNetwork& network,
  const vector<DetCond>& cond_list
)
{
  // 条件を Expr に変換する．
  auto expr_list = _make_expr_list(cond_list);

  // Expr のリストの CNF サイズを見積もる．
  auto size = expr_cnf_size(expr_list);

  // overflow した出力を持つ場合に追加のCNF式を作る．
  for ( auto& cond: cond_list ) {
    if ( cond.type() == DetCond::PartialDetected && !cond.output_list().empty() ) {
      auto root = cond.root();
      StructEngine engine0(network);
      engine0.add_cur_node(root);
      engine0.add_prev_node(root);
      auto size0 = engine0.solver().cnf_size();

      StructEngine engine1(network);
      auto bd_enc = new BoolDiffEnc(root, cond.output_list());
      engine1.add_subenc(std::unique_ptr<SubEnc>{bd_enc});
      engine1.add_prev_node(root);
      auto size1 = engine1.solver().cnf_size();

      auto raw_size = size1 - size0;
      size += raw_size;
      // 本当はもう少しリテラルが増える．
    }
    else if ( cond.type() == DetCond::Overflow ) {
      auto root = cond.root();

      StructEngine engine0(network);
      engine0.add_cur_node(root);
      engine0.add_prev_node(root);
      auto size0 = engine0.solver().cnf_size();

      StructEngine engine1(network);
      auto bd_enc = new BoolDiffEnc(root, cond.output_list());
      engine1.add_subenc(std::unique_ptr<SubEnc>{bd_enc});
      engine1.add_prev_node(root);
      auto size1 = engine1.solver().cnf_size();

      auto raw_size = size1 - size0;
      size += raw_size;
    }
  }

  return size;
}

// @brief 条件を Expr に変換する．
vector<Expr>
CnfGen::_make_expr_list(
  const vector<DetCond>& cond_list
)
{
  vector<Expr> expr_list;
  expr_list.reserve(cond_list.size());
  for ( auto& cond: cond_list ) {
    if ( cond.type() == DetCond::Detected ) {
      auto expr = cond_to_expr(cond.cond());
      expr_list.push_back(expr);
    }
    else if ( cond.type() == DetCond::PartialDetected ) {
      auto expr = Expr::zero();
      for ( auto& cond1: cond.cond_list() ) {
	auto expr1 = cond_to_expr(cond1);
	expr |= expr1;
      }
      expr_list.push_back(expr);
    }
    else {
      expr_list.push_back(Expr::one());
    }
  }
  return expr_list;
}

END_NAMESPACE_DRUID
