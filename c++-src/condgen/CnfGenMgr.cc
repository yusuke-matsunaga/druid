
/// @file CnfGenMgr.cc
/// @brief CnfGenMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "CnfGenMgr.h"
#include "CnfGenNaive.h"
#include "CnfGenCover.h"
#include "CnfGenFactor.h"
#include "CnfGenAig.h"
#include "TpgNetwork.h"
#include "TpgFFR.h"
#include "StructEngine.h"
#include "BoolDiffEnc.h"


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

END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// クラス CnfGenMgr
//////////////////////////////////////////////////////////////////////

// @brief 複数の論理式を CNF に変換する．
vector<vector<SatLiteral>>
CnfGenMgr::make_cnf(
  StructEngine& engine,
  const vector<DetCond>& cond_list,
  const JsonValue& option
)
{
  if ( cond_list.empty() ) {
    return {};
  }

  string method{"naive"};
  get_string(option, "method", method);

  if ( method == "naive" ) {
    // ナイーブなやり方
    // キューブごとにリテラルを割り当て，その OR 条件を作る．
    CnfGenNaive gen;
    return gen.make_cnf(engine, cond_list);
  }
  if ( method == "cover" ) {
    // 一旦 SopCover に変換して CNF を作る．
    CnfGenCover gen;
    return gen.make_cnf(engine, cond_list);
  }
  if ( method == "factor" ) {
    // 一旦 SopCover に変換して その後さらにファクタリングを行い，CNF を作る．
    CnfGenFactor gen;
    return gen.make_cnf(engine, cond_list);
  }
  // デフォルトフォールバック
  CnfGenNaive gen;
  return gen.make_cnf(engine, cond_list);
}

// @brief 複数の論理式を CNF に変換した際の項数とリテラル数を数える．
CnfSize
CnfGenMgr::calc_cnf_size(
  const TpgNetwork& network,
  const vector<DetCond>& cond_list,
  const JsonValue& option
)
{
  if ( cond_list.empty() ) {
    return CnfSize::zero();
  }

  string method{"naive"};
  get_string(option, "method", method);

  if ( method == "naive" ) {
    // ナイーブなやり方
    // キューブごとにリテラルを割り当て，その OR 条件を作る．
    CnfGenNaive gen;
    return gen.calc_cnf_size(network, cond_list);
  }
  if ( method == "cover" ) {
    // 一旦 SopCover に変換して CNF を作る．
    CnfGenCover gen;
    return gen.calc_cnf_size(network, cond_list);
  }
  if ( method == "factor" ) {
    // 一旦 SopCover に変換して その後さらにファクタリングを行い，CNF を作る．
    CnfGenFactor gen;
    return gen.calc_cnf_size(network, cond_list);
  }
  if ( method == "aig" ) {
    // 一旦 SopCover に変換して その後さらにファクタリングを行い，CNF を作る．
    CnfGenAig gen;
    return gen.calc_cnf_size(network, cond_list);
  }
  // デフォルトフォールバック
  CnfGenNaive gen;
  return gen.calc_cnf_size(network, cond_list);
}

// @brief 複数の論理式を CNF に変換する．
vector<vector<SatLiteral>>
CnfGenMgr::make_raw_cnf(
  StructEngine& engine,
  const TpgNetwork& network
)
{
  std::vector<BoolDiffEnc*> enc_list(network.ffr_num());
  for ( auto ffr: network.ffr_list() ) {
    auto root = ffr->root();
    auto bd_enc = new BoolDiffEnc(root);
    engine.add_subenc(std::unique_ptr<SubEnc>{bd_enc});
    engine.add_prev_node(root);
    enc_list[ffr->id()] = bd_enc;
  }
  engine.update();
  vector<vector<SatLiteral>> lits_list;
  lits_list.reserve(network.ffr_num());
  for ( auto ffr: network.ffr_list() ) {
    auto bd_enc = enc_list[ffr->id()];
    auto plit = bd_enc->prop_var();
    lits_list.push_back({plit});
  }
  return lits_list;
}

// @brief 複数の論理式をそのまま CNF に変換した際の項数とリテラル数を数える．
CnfSize
CnfGenMgr::calc_raw_cnf_size(
  const TpgNetwork& network
)
{
  auto size = CnfSize::zero();
  for ( auto ffr: network.ffr_list() ) {
    auto root = ffr->root();
    StructEngine engine0(network);
    engine0.add_cur_node(root);
    engine0.add_prev_node(root);
    auto size0 = engine0.solver().cnf_size();
    StructEngine engine1(network);
    auto bd_enc = new BoolDiffEnc(root);
    engine1.add_subenc(std::unique_ptr<SubEnc>{bd_enc});
    engine1.add_prev_node(root);
    auto size1 = engine1.solver().cnf_size();
    auto raw_size = size1 - size0;
    size += raw_size;
  }
  return size;
}

// @brief 複数の論理式を CNF に変換する．
vector<vector<SatLiteral>>
CnfGenMgr::make_naive_cnf(
  StructEngine& engine,
  const vector<DetCond>& cond_list
)
{
  CnfGenNaive gen;
  return gen.make_cnf(engine, cond_list);
}

// @brief 複数の論理式をそのまま CNF に変換した際の項数とリテラル数を数える．
CnfSize
CnfGenMgr::calc_naive_cnf_size(
  const TpgNetwork& network,
  const vector<DetCond>& cond_list
)
{
  CnfGenNaive gen;
  return gen.calc_cnf_size(network, cond_list);
}

END_NAMESPACE_DRUID
