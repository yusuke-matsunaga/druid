
/// @file CondGenMgr.cc
/// @brief CondGenMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "CondGenMgr.h"
#include "CondGen.h"
#include "CnfGenMgr.h"
#include "TpgNetwork.h"
#include "TpgFault.h"
#include "FFRFaultList.h"
#include "StructEngine.h"
#include "BoolDiffEnc.h"
#include "OpBase.h"
#include "ym/Timer.h"


#define DBG_OUT cerr

BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

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
CondGenMgr::make_ffr_cond(
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
CondGenMgr::make_ffr_cond_cnf(
  StructEngine& engine,
  const vector<DetCond>& cond_list,
  const JsonValue& option
)
{
  auto lits_array = CnfGenMgr::make_cnf(engine, cond_list, option);
  return lits_array;
}

// @brief FFRの故障伝搬条件を表すCNFのサイズを求める．
CondGenStats
CondGenMgr::calc_ffr_cond_size(
  const TpgNetwork& network,
  const vector<DetCond>& cond_list,
  const JsonValue& option
)
{
  auto cnfgen_option = get_cnfgen_option(option);

  auto stats = CondGenStats();
  stats.total_raw_size = CnfSize::zero();
  stats.naive_size = CnfSize::zero();
  stats.opt_size = CnfSize::zero();
  stats.sop_num = 0;
  stats.rest_size = CnfSize::zero();
  stats.rest_num = 0;

  stats.total_raw_size = CnfGenMgr::calc_raw_cnf_size(network);
  stats.naive_size = CnfGenMgr::calc_naive_cnf_size(network, cond_list);
  stats.opt_size = CnfGenMgr::calc_cnf_size(network, cond_list, cnfgen_option);

  return stats;
}

END_NAMESPACE_DRUID
