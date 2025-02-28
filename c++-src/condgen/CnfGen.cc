
/// @file CnfGen.cc
/// @brief CnfGen の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "CnfGen.h"
#include "CnfGenNaive.h"
#include "CnfGenFactor.h"
#include "CnfGenBdd.h"
#include "StructEngine.h"


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

// @brief 複数の論理式を CNF に変換する．
vector<vector<SatLiteral>>
CnfGen::make_cnf(
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
  if ( method == "factor" ) {
    // 一旦 SopCover に変換して その後さらにファクタリングを行い，CNF を作る．
    CnfGenFactor gen;
    return gen.make_cnf(engine, cond_list);
  }
  if ( method == "bdd" ) {
    // 一旦 Bdd に変換して CNF を作る．
    SizeType limit = 1000;
    CnfGenBdd gen{limit};
    return gen.make_cnf(engine, cond_list);
  }
  // デフォルトフォールバック
  CnfGenNaive gen;
  return gen.make_cnf(engine, cond_list);
}

// @brief 複数の論理式を CNF に変換した際の項数とリテラル数を数える．
CnfSize
CnfGen::calc_cnf_size(
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
    return gen.calc_cnf_size(cond_list);
  }
  if ( method == "factor" ) {
    // 一旦 SopCover に変換して その後さらにファクタリングを行い，CNF を作る．
    CnfGenFactor gen;
    return gen.calc_cnf_size(cond_list);
  }
  if ( method == "bdd" ) {
    // 一旦 Bdd に変換して CNF を作る．
    SizeType limit = 10000;
    CnfGenBdd gen{limit};
    return gen.calc_cnf_size(cond_list);
  }
  // デフォルトフォールバック
  CnfGenNaive gen;
  return gen.calc_cnf_size(cond_list);
}

// @brief 複数の論理式をそのまま CNF に変換した際の項数とリテラル数を数える．
CnfSize
CnfGen::calc_naive_cnf_size(
  const vector<DetCond>& cond_list
)
{
  CnfGenNaive gen;
  return gen.calc_cnf_size(cond_list);
}

END_NAMESPACE_DRUID
