
/// @file CondGenMgr.cc
/// @brief CondGenMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include <thread>
#include <mutex>
#include "condgen/CondGenMgr.h"
#include "CondGen.h"
#include "condgen/CondGenStats.h"
#include "types/TpgNetwork.h"
#include "types/TpgFFR.h"
#include "types/TpgFault.h"
#include "dtpg/StructEngine.h"
#include "dtpg/BoolDiffEnc.h"
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
  return get_int(option, KEY, DEFAULT_LOOP_LIMIT);
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
  auto limit = get_loop_limit(option);
  bool multi_thread = get_bool(option, "multi_thread", false);

  // ffr.id() をキーとして個々のFFRの伝搬条件を記録する配列
  std::vector<DetCond> cond_list(network.ffr_num());

  if ( multi_thread ) {
    // スレッド数
    SizeType thread_num = get_int(option, "thread_num", 0);
    if ( thread_num == 0 ) {
      thread_num = std::thread::hardware_concurrency();
    }
    // スレッドのリスト
    std::vector<std::thread> thr_list(thread_num);

    // 次に処理すべき FFR 番号
    SizeType ffr_id = 0;
    // ffr_id 用のミューテックス
    std::mutex mtx;
    // スレッドを生成する．
    for ( SizeType i = 0; i < thread_num; ++ i ) {
      thr_list[i] = std::thread{[&](){
	for ( ; ; ) {
	  SizeType my_id = 0;
	  { // unique_lock 用のスコープ
	    std::unique_lock lck{mtx};
	    if ( ffr_id >= network.ffr_num() ) {
	      break;
	    }
	    my_id = ffr_id;
	    ++ ffr_id;
	  }
	  auto ffr = network.ffr(my_id);
	  auto cond = CondGen::root_cond(network, ffr, limit, option);
	  cond_list[my_id] = cond;
	}
      }};
    }
    // 子スレッドが終了するまで待つ．
    for ( auto& thr: thr_list ) {
      thr.join();
    }
  }
  else {
    for ( auto ffr: network.ffr_list() ) {
      auto cond = CondGen::root_cond(network, ffr, limit, option);
      cond_list[ffr.id()] = cond;
    }
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
  const JsonValue& option,
  CondGenStats& stats
)
{
  make_base_cnf(engine);
  engine.update();
  auto size1 = engine.solver().cnf_size();
  auto expr_list = make_expr(cond_list, option);

  auto lits_list1 = expr_to_cnf(engine, expr_list, option);
  auto size2 = engine.solver().cnf_size();
  auto lit_list2 = make_bd(engine, cond_list);
  auto size3 = engine.solver().cnf_size();

  // 結果の配列を作る．
  auto cond_num = cond_list.size();
  std::vector<std::vector<SatLiteral>> lits_array;
  lits_array.reserve(cond_num);

  // lits_list1/lits_list2 の結果を反映する．
  for ( SizeType id = 0; id < cond_num; ++ id ) {
    auto& cond = cond_list[id];
    if ( cond.type() == DetCond::Detected ) {
      auto& lits = lits_list1[id];
      lits_array.push_back(lits);
    }
    else if ( cond.type() == DetCond::PartialDetected ) {
      auto& lits1 = lits_list1[id];
      auto lit2 = lit_list2[id];
      auto lits = lits1;
      lits.push_back(lit2);
      lits_array.push_back(lits);
    }
    else if ( cond.type() == DetCond::Overflow ) {
      auto lit = lit_list2[id];
      lits_array.push_back({lit});
    }
    else {
      lits_array.push_back({});
    }
  }

  stats = CondGenStats(size1, size3 - size2, size2 - size1);

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
    auto root = ffr.root();
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

  auto multi_thread = get_bool(option, "multi_thread", false);

  auto cond_num = cond_list.size();
  // 結果の配列
  auto expr_list = std::vector<Expr>(cond_num, Expr::one());

  if ( multi_thread ) {
    // スレッド数
    SizeType thread_num = get_int(option, "thread_num", 0);
    if ( thread_num == 0 ) {
      thread_num = std::thread::hardware_concurrency();
    }
    // スレッドのリスト
    auto thr_list = std::vector<std::thread>(thread_num);

    // 次に処理すべきcond番号
    SizeType cond_id = 0;
    // cond_id 用のミューテックス
    std::mutex mtx;
    // スレッドを生成する．
    for ( SizeType i = 0; i < thread_num; ++ i ) {
      thr_list[i] = std::thread{[&](){
	for ( ; ; ) {
	  SizeType my_id = 0;
	  { // unique_lock 用のスコープ
	    std::unique_lock lck{mtx};
	    if ( cond_id >= cond_num ) {
	      break;
	    }
	    my_id = cond_id;
	    ++ cond_id;
	  }
	  auto cond = cond_list[my_id];
	  if ( cond.type() == DetCond::Detected ||
	       cond.type() == DetCond::PartialDetected ) {
	    auto expr = expr_gen->cond_to_expr(cond);
	    expr_list[my_id] = expr;
	  }
	}
      }};
    }
    // 子スレッドが終了するまで待つ．
    for ( auto& thr: thr_list ) {
      thr.join();
    }
  }
  else {
    for ( SizeType cond_id = 0; cond_id < cond_num; ++ cond_id ) {
      auto& cond = cond_list[cond_id];
      if ( cond.type() == DetCond::Detected ||
	   cond.type() == DetCond::PartialDetected ) {
	auto expr = expr_gen->cond_to_expr(cond);
	expr_list[cond_id] = expr;
      }
    }
  }
  return expr_list;
}

// @brief Expr を CNF に変換する．
std::vector<std::vector<SatLiteral>>
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
  SatLiteralMap lit_map;
  for ( SizeType input_id: input_id_list ) {
    auto node_id = input_id / 2;
    auto time = input_id % 2;
    auto node = engine.network().node(node_id);
    auto as = Assign(node, time, true);
    auto lit = engine.conv_to_literal(as);
    lit_map.emplace(input_id, lit);
  }

  auto method = get_string(option, "method", "naive");
  if ( method == "aig" ) {
    // 一旦 AIG に変換する．
    AigMgr mgr;
    auto aig_list = mgr.from_expr_list(expr_list);
    for ( auto aig: aig_list ) {
      mgr.new_output(aig);
    }
    {
      auto aiger_env = getenv("AIGER_FILE");
      if ( aiger_env != nullptr ) {
	std::ofstream s{aiger_env};
	if ( s ) {
	  mgr.write_aig(s);
	}
      }
    }
    bool rewrite = get_bool(option, "rewrite", false);
    if ( rewrite ) {
      // 簡単化を行う．
      mgr.rewrite();
    }
    // AIG を CNF に変換する．
    auto lits_list = engine.solver().add_aig(mgr, lit_map);
    return lits_list;
  }
  else {
    // Expr を CNF に変換する．
    std::vector<std::vector<SatLiteral>> lits_list;
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
  auto cond_num = cond_list.size();
  std::vector<SatLiteral> lit_list(cond_num, SatLiteral::X);
  for ( SizeType id = 0; id < cond_num; ++ id ) {
    auto& cond = cond_list[id];
    if ( cond.type() == DetCond::PartialDetected ||
	 cond.type() == DetCond::Overflow ) {
      auto bd_enc = new BoolDiffEnc(cond.root(), cond.output_list());
      engine.add_subenc(std::unique_ptr<SubEnc>{bd_enc});
      auto lit = bd_enc->prop_var();
      lit_list[id] = lit;
    }
  }
  return lit_list;
}

// @brief FFRの故障伝搬条件を表すCNF式を作る(ナイーブバージョン)
std::vector<std::vector<SatLiteral>>
CondGenMgr::make_cnf_naive(
  StructEngine& engine,
  const JsonValue& option,
  CondGenStats& stats
)
{
  engine.update();
  auto size1 = engine.solver().cnf_size();

  auto& network = engine.network();
  auto ffr_num = network.ffr_num();
  std::vector<std::vector<SatLiteral>> lits_array(ffr_num);
  for ( auto ffr: network.ffr_list() ) {
    auto bd_enc = new BoolDiffEnc(ffr.root());
    engine.add_subenc(std::unique_ptr<SubEnc>{bd_enc});
    auto lit = bd_enc->prop_var();
    lits_array[ffr.id()] = std::vector<SatLiteral>{lit};
  }

  auto size2 = engine.solver().cnf_size();
  stats = CondGenStats(size1, CnfSize::zero(), size2 - size1);
  return lits_array;
}

END_NAMESPACE_DRUID
