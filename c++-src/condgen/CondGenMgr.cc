
/// @file CondGenMgr.cc
/// @brief CondGenMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "CondGenMgr.h"
#include "CondGen.h"
#include "TpgNetwork.h"
#include "TpgFFR.h"
#include "TpgFault.h"
#include "StructEngine.h"
#include "BoolDiffEnc.h"
#include "OpBase.h"
#include "ExprGen.h"
#include "ym/AigMgr.h"
#include "ym/Timer.h"


#define DBG_OUT cerr

BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// デフォルトのループ回数
static const SizeType DEFAULT_LOOP_LIMIT = 1000;

// @brief オプション中から "loop_limit" 属性を取り出す．
//
// * "loop_limit" の値を持たなければデフォルト値を返す．
// * "loop_limit" の値が int ならそのまま返す．
// * それ以外は例外を送出する．
SizeType
get_loop_limit(
  const JsonValue& option ///< [in] オプション
)
{
  const char* KEY = "loop_limit";
  int val = DEFAULT_LOOP_LIMIT;
  OpBase::get_int(option, KEY, val);
  return val;
}

END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// クラス CondGenMgr
//////////////////////////////////////////////////////////////////////

// @brief 故障検出条件を求める．
std::vector<DetCond>
CondGenMgr::make_cond(
  const TpgNetwork& network,
  const JsonValue& option
)
{
  int debug = OpBase::get_debug(option);
  auto limit = get_loop_limit(option);

  auto ffr_num = network.ffr_num();
  // ffr->id() をキーとして個々のFFRの伝搬条件を記録する配列
  std::vector<DetCond> cond_list;
  cond_list.reserve(ffr_num);
  for ( auto ffr: network.ffr_list() ) {
    auto cond = CondGen::root_cond(network, ffr, limit, option);
    cond_list.push_back(cond);
  }
  return cond_list;
}

BEGIN_NONAMESPACE

void
dfs(
  const Expr& expr,
  std::vector<SizeType>& input_id_list,
  std::unordered_set<SizeType>& mark
)
{
  if ( expr.is_constant() ) {
    return;
  }
  if ( expr.is_literal() ) {
    auto varid = expr.varid();
    if ( mark.count(varid) == 0 ) {
      mark.emplace(varid);
      input_id_list.push_back(varid);
    }
    return;
  }
  for ( auto& expr1: expr.operand_list() ) {
    dfs(expr1, input_id_list, mark);
  }
}

END_NONAMESPACE

// @brief FFRの故障伝搬条件を表すCNF式を作る．
std::vector<std::vector<SatLiteral>>
CondGenMgr::make_cnf(
  StructEngine& engine,
  const std::vector<DetCond>& cond_list,
  const JsonValue& option
)
{
  make_base_cnf(engine);
  auto expr_list = make_expr(cond_list, option);
  auto lits_list1 = expr_to_cnf(engine, expr_list, option);
  auto lit_list2 = make_bd(engine, cond_list);

  // 結果の配列を作る．
  std::vector<std::vector<SatLiteral>> lits_array;
  lits_array.reserve(cond_list.size());

  // lits_list1/lits_list2 の結果を反映する．
  SizeType pos1 = 0;
  SizeType pos2 = 0;
  for ( auto& cond: cond_list ) {
    auto id = cond.ffr_id();
    if ( cond.type() == DetCond::Detected ) {
      auto& lits = lits_list1[pos1];
      lits_array.push_back(lits);
      ++ pos1;
    }
    else if ( cond.type() == DetCond::PartialDetected ) {
      auto& lits1 = lits_list1[pos1];
      auto lit2 = lit_list2[pos2];
      auto lits = lits1;
      lits.push_back(lit2);
      lits_array.push_back(lits);
      ++ pos1;
      ++ pos2;
    }
    else if ( cond.type() == DetCond::Overflow ) {
      auto lit = lit_list2[pos2];
      lits_array.push_back({lit});
      ++ pos2;
    }
    else {
      lits_array.push_back({});
    }
  }

  return lits_array;
}

// @brief 正常回路用の CNF を作る．
void
CondGenMgr::make_base_cnf(
  StructEngine& engine
)
{
  auto& network = engine.network();
  for ( auto ffr: network.ffr_list() ) {
    auto root = ffr->root();
    engine.add_cur_node(root);
    engine.add_prev_node(root);
  }
}

// @brief DetCond から Expr を作る．
std::vector<Expr>
CondGenMgr::make_expr(
  const std::vector<DetCond>& cond_list,
  const JsonValue& option
)
{
  auto expr_gen = ExprGen::new_obj(option);

  // DetCond を解析して Expr のリストと出力のリストを作る．
  std::vector<Expr> expr_list;
  auto nd = cond_list.size();
  expr_list.reserve(nd);
  for ( auto& cond: cond_list ) {
    if ( cond.type() == DetCond::Detected ||
	 cond.type() == DetCond::PartialDetected ) {
      auto expr = expr_gen->cond_to_expr(cond);
      expr_list.push_back(expr);
    }
  }
  return expr_list;
}

// @brief Expr を CNF に変換する．
std::vector<vector<SatLiteral>>
CondGenMgr::expr_to_cnf(
  StructEngine& engine,
  const std::vector<Expr>& expr_list,
  const JsonValue& option
)
{
  // expr_list 中に現れる入力番号のリストを作る．
  std::vector<SizeType> input_id_list;
  std::unordered_set<SizeType> mark;
  for ( auto& expr: expr_list ) {
    dfs(expr, input_id_list, mark);
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

  auto method = string{"naive"};
  OpBase::get_string(option, "method", method);
  if ( method == "aig" ) {
    // 一旦 AIG に変換する．
    AigMgr mgr;
    auto aig_list = mgr.from_expr_list(expr_list);

    // AIG を CNF に変換する．
    auto lits_list = engine.solver().add_aig(aig_list, lit_map);
    return lits_list;
  }
  else {
    // Expr を CNF に変換する．
    std::vector<vector<SatLiteral>> lits_list;
    lits_list.reserve(expr_list.size());
    for ( auto& expr: expr_list ) {
      auto lits = engine.solver().add_expr(expr, lit_map);
      lits_list.push_back(lits);
    }
    return lits_list;
  }
}

// @brief BoolDiffEnc を用いた条件を作る．
std::vector<SatLiteral>
CondGenMgr::make_bd(
  StructEngine& engine,
  const std::vector<DetCond>& cond_list
)
{
  std::vector<SatLiteral> lit_list;
  lit_list.reserve(cond_list.size());
  for ( auto& cond: cond_list ) {
    if ( cond.type() == DetCond::PartialDetected ||
	 cond.type() == DetCond::Overflow ) {
      auto bd_enc = new BoolDiffEnc(cond.root(), cond.output_list());
      engine.add_subenc(std::unique_ptr<SubEnc>{bd_enc});
      auto lit = bd_enc->prop_var();
      lit_list.push_back(lit);
    }
  }
  return lit_list;
}

END_NAMESPACE_DRUID
