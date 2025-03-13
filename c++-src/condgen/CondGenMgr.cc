
/// @file CondGenMgr.cc
/// @brief CondGenMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "CondGenMgr.h"
#include "CondGen.h"
#include "CnfGen.h"
#include "TpgNetwork.h"
#include "TpgFault.h"
#include "StructEngine.h"
//#include "BoolDiffEnc.h"
#include "OpBase.h"
#include "ym/Timer.h"
#include "ExprGen.h"
#include "Expr2Aig.h"


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
  std::vector<CnfGen::BdInfo> bd_list;
  auto nd = cond_list.size();
  expr_list.reserve(nd);
  ffr_id_list.reserve(nd);
  bd_list.reserve(nd);
  for ( auto& cond: cond_list ) {
    if ( cond.type() != DetCond::Detected ) {
      auto expr = expr_gen->cond_to_expr(cond);
      expr_list.push_back(expr);
      ffr_id_list.push_back(cond.ffr_id());
    }
    if ( !cond.output_list().empty() ) {
      auto bd_info = CnfGen::BdInfo{cond.ffr_id(),
				    cond.root(),
				    cond.output_list()};
      bd_list.push_back(bd_info);
    }
  }

  // Expr のリストを AIG に変形する．
  auto method = string{"naive"};
  OpBase::get_string(option, "method", method);
  auto sharing = method == "aig";
  AigMgr mgr;
  Expr2Aig expr2aig(mgr, sharing);
  auto aig_list = expr2aig.conv_to_aig(expr_list);

  // CNF を生成する．
  auto lits_array = CnfGen::make_cnf(engine, aig_list, ffr_id_list, bd_list);
  return lits_array;
}

END_NAMESPACE_DRUID
