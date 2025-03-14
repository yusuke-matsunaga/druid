
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

// @brief ブール微分用の情報
struct BdInfo {
  SizeType id;                             ///< FFR番号
  const TpgNode* root;                     ///< 起点のノード
  std::vector<const TpgNode*> output_list; ///< 出力のリスト
};

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

// AIG を CNF に変換する．
std::vector<std::vector<SatLiteral>>
aig_to_cnf(
  StructEngine& engine,
  const std::vector<AigHandle>& aig_list
)
{
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

  // AIG を CNF に変換する．
  auto lits_list = engine.solver().add_aig(aig_list, lit_map);
  return lits_list;
}

void
bd_to_cnf(
  StructEngine& engine,
  const std::vector<BdInfo>& bd_list,
  const std::unordered_map<SizeType, SizeType>& id_map,
  std::vector<CondGenMgr::CondLits>& cond_lits_array
)
{
  for ( auto& info: bd_list ) {
    auto id = info.id;
    auto bd_enc = new BoolDiffEnc(info.root, info.output_list);
    engine.add_subenc(std::unique_ptr<SubEnc>{bd_enc});
    auto plit = bd_enc->prop_var();
    auto pos = id_map.at(id);
    auto& data = cond_lits_array[pos];
    if ( data.detected ) {
      auto old_lits = data.lits;
      if ( old_lits.size() == 1 ) {
	auto lit1 = old_lits.front();
	auto new_lit = engine.solver().new_variable(true);
	engine.solver().add_clause(~new_lit, lit1, plit);
	data.lits = {new_lit};
      }
      else {
	auto and_lit = engine.solver().new_variable(false);
	for ( auto lit: old_lits ) {
	  engine.solver().add_clause(~and_lit, lit);
	}
	auto new_lit = engine.solver().new_variable(true);
	engine.solver().add_clause(~new_lit, and_lit, plit);
	data.lits = {new_lit};
      }
    }
    else {
      cond_lits_array[pos] = CondGenMgr::CondLits{id, true, {plit}};
    }
  }
}

END_NONAMESPACE

// @brief FFRの故障伝搬条件を表すCNF式を作る．
std::vector<CondGenMgr::CondLits>
CondGenMgr::make_cnf(
  StructEngine& engine,
  const std::vector<DetCond>& cond_list,
  const JsonValue& option
)
{
  auto expr_gen = ExprGen::new_obj(option);

  // DetCond を解析して Expr のリストと出力のリストを作る．
  std::vector<Expr> expr_list;
  std::vector<SizeType> ffr_id_list;
  std::vector<BdInfo> bd_list;
  auto nd = cond_list.size();
  expr_list.reserve(nd);
  ffr_id_list.reserve(nd);
  bd_list.reserve(nd);
  for ( auto& cond: cond_list ) {
    if ( cond.type() != DetCond::Undetected ) {
      engine.add_cur_node(cond.root());
      engine.add_prev_node(cond.root());
    }
    if ( cond.type() == DetCond::Detected ||
	 cond.type() == DetCond::PartialDetected ) {
      auto expr = expr_gen->cond_to_expr(cond);
      expr_list.push_back(expr);
      ffr_id_list.push_back(cond.ffr_id());
    }
    if ( cond.type() == DetCond::PartialDetected ||
	 cond.type() == DetCond::Overflow ) {
      auto bd_info = BdInfo{cond.ffr_id(),
			    cond.root(),
			    cond.output_list()};
      bd_list.push_back(bd_info);
    }
  }

  // 結果の配列を作る．
  std::vector<CondLits> cond_lits_array(nd);
  // 初期化する．
  std::unordered_map<SizeType, SizeType> id_map;
  for ( SizeType i = 0; i < nd; ++ i ) {
    auto& cond = cond_list[i];
    auto id = cond.ffr_id();
    cond_lits_array[i] = CondLits{id, false, {}};
    id_map.emplace(id, i);
  }

  // Expr のリストを AIG に変形する．
  auto method = string{"naive"};
  OpBase::get_string(option, "method", method);
  auto sharing = method == "aig";
  if ( sharing ) {
    AigMgr mgr;
    auto aig_list = mgr.from_expr_list(expr_list);

    // AIG に対応する CNF を生成する．
    auto aig_lits_list = aig_to_cnf(engine, aig_list);

    // aig_lits_list の結果を反映させる．
    for ( SizeType i = 0; i < expr_list.size(); ++ i ) {
      auto id = ffr_id_list[i];
      auto& lits = aig_lits_list[i];
      auto pos = id_map.at(id);
      cond_lits_array[pos] = CondLits{id, true, lits};
    }
  }
  else {
    // 個々の要素ごとに処理を行う．
    for ( SizeType i = 0; i < expr_list.size(); ++ i ) {
      AigMgr mgr;
      auto aig = mgr.from_expr(expr_list[i]);

      // AIG に対応する CNF を生成する．
      auto aig_lits_list = aig_to_cnf(engine, {aig});
      auto aig_lits = aig_lits_list.front();

      auto id = ffr_id_list[i];
      auto pos = id_map.at(id);
      cond_lits_array[pos] = CondLits{id, true, aig_lits};
    }
  }

  // オーバーフローした分のブール微分回路を作って cond_lits_array
  // 反映させる．
  bd_to_cnf(engine, bd_list, id_map, cond_lits_array);

  return cond_lits_array;
}

// @brief FFRの故障伝搬条件を表すCNF式を単純な方法で作る．
std::vector<CondGenMgr::CondLits>
CondGenMgr::make_raw_cnf(
  StructEngine& engine,
  const JsonValue& option
)
{
  std::vector<BdInfo> bd_list;
  std::unordered_map<SizeType, SizeType> id_map;
  auto& network = engine.network();
  SizeType id = 0;
  for ( auto ffr: network.ffr_list() ) {
    auto bd_info = BdInfo{ffr->id(), ffr->root()};
    bd_list.push_back(bd_info);
    id_map.emplace(id, ffr->id());
    ++ id;
  }

  // 結果の配列を作る．
  std::vector<CondLits> cond_lits_array(network.ffr_num());
  // 初期化する．
  for ( SizeType id = 0; id < network.ffr_num(); ++ id ) {
    cond_lits_array[id] = CondLits{id, false, {}};
  }

  bd_to_cnf(engine, bd_list, id_map, cond_lits_array);

  return cond_lits_array;
}

END_NAMESPACE_DRUID
