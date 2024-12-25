
/// @file CnfGen.cc
/// @brief CnfGen の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "CnfGen.h"
#include "CnfGenNaive.h"
#include "CnfGenBdd.h"
#include "StructEngine.h"
//#include "TpgNetwork.h"
//#include "DetCond.h"
//#include "ym/BddVar.h"


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
// クラス CnfGen
//////////////////////////////////////////////////////////////////////

// @brief 論理式を CNF に変換する．
vector<SatLiteral>
CnfGen::make_cnf(
  StructEngine& engine,
  const DetCond& cond,
  const JsonValue& option
)
{
  if ( cond.empty() ) {
    return {};
  }

  string method{"naive"};
  get_string(option, "method", method);

  if ( method == "naive" ) {
    // ナイーブなやり方
    // キューブごとにリテラルを割り当て，その OR 条件を作る．
    CnfGenNaive gen{engine};
    return gen.make_cnf(cond);
  }
  if ( method == "bdd" ) {
    // 一旦 Bdd に変換して CNF を作る．
    SizeType limit = 10000;
    CnfGenBdd gen{engine, limit};
    return gen.make_cnf(cond);
  }
  // デフォルトフォールバック
  CnfGenNaive gen{engine};
  return gen.make_cnf(cond);
}

// @brief 複数の論理式を CNF に変換する．
vector<vector<SatLiteral>>
CnfGen::make_cnf(
  StructEngine& engine,
  const vector<DetCond>& cond_list,
  const JsonValue& option
)
{
  vector<vector<SatLiteral>> assumptions_list;
  for ( auto& cond: cond_list ) {
    auto assumptions = make_cnf(engine, cond, option);
    assumptions_list.push_back(assumptions);
  }
  return assumptions_list;
}

// @brief 論理式を CNF に変換した際の項数とリテラル数を数える．
CnfSize
CnfGen::calc_cnf_size(
  StructEngine& engine,
  const DetCond& cond,
  const JsonValue& option
)
{
  if ( cond.empty() ) {
    return CnfSize::zero();
  }

  string method{"naive"};
  get_string(option, "method", method);

  if ( method == "naive" ) {
    // ナイーブなやり方
    // キューブごとにリテラルを割り当て，その OR 条件を作る．
    CnfGenNaive gen{engine};
    return gen.calc_cnf_size(cond);
  }
  if ( method == "bdd" ) {
    // 一旦 Bdd に変換して CNF を作る．
    SizeType limit = 10000;
    CnfGenBdd gen{engine, limit};
    return gen.calc_cnf_size(cond);
  }
  // デフォルトフォールバック
  CnfGenNaive gen{engine};
  return gen.calc_cnf_size(cond);
}

// @brief 複数の論理式を CNF に変換した際の項数とリテラル数を数える．
CnfSize
CnfGen::calc_cnf_size(
  StructEngine& engine,
  const vector<DetCond>& cond_list,
  const JsonValue& option
)
{
  CnfSize size = CnfSize::zero();
  for ( auto& cond: cond_list ) {
    size += calc_cnf_size(engine, cond, option);
  }
  return size;
}

END_NAMESPACE_DRUID
