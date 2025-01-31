
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
vector<vector<SatLiteral>>
CondGenMgr::make_ffr_cond(
  StructEngine& engine,
  const TpgNetwork& network,
  const JsonValue& option
)
{
  int debug = OpBase::get_debug(option);
  auto limit = get_loop_limit(option);
  auto cnfgen_option = get_cnfgen_option(option);

  auto ffr_num = network.ffr_num();
  // ffr->id() をキーとして cond_list 中のインデックスを保持する辞書
  // 上限を超えた場合，cond_list には登録されない．
  std::unordered_map<SizeType, SizeType> id_map;
  vector<DetCond> cond_list;
  cond_list.reserve(ffr_num);
  vector<BoolDiffEnc*> bd_array(ffr_num, nullptr);
  vector<const TpgNode*> root_list;
  root_list.reserve(ffr_num);
  for ( auto ffr: network.ffr_list() ) {
    auto root = ffr->root();
    root_list.push_back(root);
    CondGen gen{network, ffr, option};
    auto cond = gen.root_cond(limit);
    if ( cond.cube_list().size() == limit ) {
      // オーバーフローした場合は本当の式を作る．
      auto bd_enc = new BoolDiffEnc{engine, root};
      bd_array[ffr->id()] = bd_enc;
    }
    else {
      auto id = cond_list.size();
      id_map.emplace(ffr->id(), id);
      cond_list.push_back(cond);
    }
  }
  engine.make_cnf(root_list, root_list);

  auto tmp_lits_array = CnfGen::make_cnf(engine, cond_list, cnfgen_option);
  vector<vector<SatLiteral>> lits_array(ffr_num);
  for ( auto ffr: network.ffr_list() ) {
    auto bd = bd_array[ffr->id()];
    if ( bd != nullptr ) {
      auto lit = bd->prop_var();
      lits_array[ffr->id()] = vector<SatLiteral>{lit};
    }
    else {
      auto id = id_map.at(ffr->id());
      lits_array[ffr->id()] = tmp_lits_array[id];
    }
  }
  return lits_array;
}

// @brief FFRの故障伝搬条件を表すCNFのサイズを求める．
CnfSize
CondGenMgr::calc_ffr_cond_size(
  const TpgNetwork& network,
  const JsonValue& option
)
{
  auto limit = get_loop_limit(option);
  auto cnfgen_option = get_cnfgen_option(option);

  auto total_size = CnfSize::zero();
  vector<DetCond> cond_list;
  cond_list.reserve(network.ffr_num());
  SizeType total_cube_num = 0;
  SizeType cond_num = 0;
  for ( auto ffr: network.ffr_list() ) {
    CondGen gen{network, ffr, option};
    auto cond = gen.root_cond(limit);
    if ( cond.cube_list().size() == limit ) {
      // オーバーフローした場合は本当の式を作る．
      auto root = ffr->root();
      StructEngine engine0{network};
      engine0.make_cnf({root}, {root});
      auto size0 = engine0.solver().cnf_size();
      StructEngine engine1{network};
      auto bd_enc = new BoolDiffEnc{engine1, root};
      engine1.make_cnf({}, {root});
      auto size1 = engine1.solver().cnf_size();
      auto size = size1 - size0;
      total_size += size;
    }
    else {
      cond_list.push_back(cond);
      total_cube_num += cond.cube_list().size();
      ++ cond_num;
    }
  }
  auto size1 = CnfGen::calc_cnf_size(cond_list, cnfgen_option);
  total_size += size1;
  auto ave_cube_num = cond_num > 0 ? total_cube_num / cond_num : 0;
  cout << "Ave. cube num = " << ave_cube_num << endl;

  return total_size;
}

END_NAMESPACE_DRUID
