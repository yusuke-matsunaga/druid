
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

// 文字列型のオプションを取り出す．
//
// 結果は value に上書きされる．
// エラーが起こったら std::invalid_argument 例外を送出する．
void
get_string(
  const JsonValue& option,
  const string& keyword,
  string& value
)
{
  if ( option.is_object() && option.has_key(keyword) ) {
    auto value_obj = option.at(keyword);
    if ( value_obj.is_string() ) {
      value = value_obj.get_string();
    }
    else {
      ostringstream buf;
      buf << "'" << keyword << "' should be a string";
      throw std::invalid_argument{buf.str()};
    }
  }
}

// デフォルトのループ回数
static const SizeType DEFAULT_LOOP_LIMIT = 300;

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
  if ( option.is_object() && option.has_key(KEY) ) {
    auto val = option.get(KEY);
    if ( val.is_int() ) {
      return val.get_int();
    }
    throw std::invalid_argument{"'loop_limit' should be an integer"};
  }
  return DEFAULT_LOOP_LIMIT;
}

// @brief オプションの中から "cnfgen" 属性を取り出す．
JsonValue
get_cnfgen_option(
  const JsonValue& option ///< [in] オプション
)
{
  const char* KEY = "cnfgen";
  if ( option.is_object() ) {
    return option.get(KEY);
  }
  return JsonValue{};
}

END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// クラス CondGenMgr
//////////////////////////////////////////////////////////////////////

// @brief 故障検出条件を求める．
vector<DetCond>
CondGenMgr::make_cond(
  const TpgNetwork& network,
  const JsonValue& option
)
{
  int debug = OpBase::get_debug(option);
  auto limit = get_loop_limit(option);

  auto ffr_num = network.ffr_num();
  // ffr->id() をキーとして個々のFFRの伝搬条件を記録する配列
  vector<DetCond> cond_list;
  cond_list.reserve(ffr_num);
  for ( auto ffr: network.ffr_list() ) {
    auto cond = CondGen::root_cond(network, ffr, limit, option);
    cond_list.push_back(cond);
  }
  return cond_list;
}

// @brief FFRの故障伝搬条件を表すCNF式を作る．
vector<vector<SatLiteral>>
CondGenMgr::make_cnf(
  StructEngine& engine,
  const vector<DetCond>& cond_list,
  const JsonValue& option
)
{
  auto expr_gen = ExprGen::new_obj(option);

  // DetCond を解析して Expr のリストと出力のリストを作る．
  std::vector<Expr> expr_array;
  std::vector<CnfGen::BdInfo> bd_list;
  auto nd = cond_list.size();
  expr_array.reserve(nd);
  bd_list.reserve(nd);
  for ( auto& cond: cond_list ) {
    auto expr = expr_gen->cond_to_expr(cond);
    expr_array.push_back(expr);
    if ( cond.output_list().empty() ) {
      continue;
    }
    auto bd_info = CnfGen::BdInfo{cond.ffr_id(),
				  cond.root(),
				  cond.output_list()};
    bd_list.push_back(bd_info);
  }

  // Expr のリストを AIG に変形する．
  auto method = string{"naive"};
  get_string(option, "method", method);
  auto sharing = method == "aig";
  AigMgr mgr;
  Expr2Aig expr2aig(mgr, sharing);
  auto aig_array = expr2aig.conv_to_aig(expr_array);

  // CNF を生成する．
  auto lits_array = CnfGen::make_cnf(engine, aig_array, bd_list);
  return lits_array;
}

END_NAMESPACE_DRUID
