
/// @file ExprGen.cc
/// @brief ExprGen の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "ExprGen.h"
#include "ExprGen_Naive.h"
#include "ExprGen_Factor.h"
#include "ym/SopCover.h"
#include "LocalMap.h"


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


// @brief 継承クラスを生成するクラスメソッド
std::unique_ptr<ExprGen>
ExprGen::new_obj(
  const JsonValue& option
)
{
  auto method = string{"naive"};
  get_string(option, "method", method);

  if ( method == "naive" ) {
    // ナイーブなやり方
    // キューブごとにリテラルを割り当て，その OR 条件を作る．
    return std::unique_ptr<ExprGen>{new ExprGen_Naive};
  }
  if ( method == "factor" || method == "aig" ) {
    // 一旦 SopCover に変換して その後さらにファクタリングを行い，CNF を作る．
    return std::unique_ptr<ExprGen>{new ExprGen_Factor};
  }
  // デフォルトフォールバック
  return std::unique_ptr<ExprGen>{new ExprGen_Naive};
}

// @brief DetCond を Expr に変換する．
Expr
ExprGen::cond_to_expr(
  const DetCond& cond
)
{
  if ( cond.type() == DetCond::Undetected ) {
    return Expr::zero();
  }
  if ( cond.type() == DetCond::Detected ) {
    return conv(cond.cond());
  }
  // cond.type() == DetCond::PartialDetected:
  // cond.type() == DetCond::Overflow:
  auto expr0 = conv(cond.cond());
  std::vector<Expr> expr_list;
  expr_list.reserve(cond.cond_list().size());
  for ( auto& data1: cond.cond_list() ) {
    auto expr1 = conv(data1);
    expr_list.push_back(expr1);
  }
  return expr0 & Expr::or_op(expr_list);
}


BEGIN_NONAMESPACE

// リテラルのリストを Expr に変換する．
Expr
lits_to_expr(
  const vector<Literal>& lits
)
{
  auto expr = Expr::one();
  for ( auto lit: lits ) {
    auto expr1 = Expr::literal(lit);
    expr &= expr1;
  }
  return expr;
}

END_NONAMESPACE


//////////////////////////////////////////////////////////////////////
// クラス Exprgen_Naive
//////////////////////////////////////////////////////////////////////

// @brief DetCond::CondData から Expr を作る．
Expr
ExprGen_Naive::conv(
  const DetCond::CondData& cond
)
{
  auto expr = lits_to_expr(cond.mand_cond);
  if ( cond.cube_list.empty() ) {
    return expr;
  }
  auto cov_expr = Expr::zero();
  for ( auto lits: cond.cube_list ) {
    auto expr1 = lits_to_expr(lits);
    cov_expr |= expr1;
  }
  return expr & cov_expr;
}


//////////////////////////////////////////////////////////////////////
// クラス ExprGen_Factor
//////////////////////////////////////////////////////////////////////

// @brief DetCond から Expr を作る．
Expr
ExprGen_Factor::conv(
  const DetCond::CondData& cond
)
{
  auto expr = lits_to_expr(cond.mand_cond);
  if ( cond.cube_list.empty() ) {
    return expr;
  }

  // cube_list に現れる変数のみを集めた辞書を作る．
  LocalMap local_map;
  auto cover = local_map.to_cover(cond.cube_list);

  // ファクタリングを行う．
  auto local_expr = cover.bool_factor();

  // local_expr 中の ID を元に ID に置き換える．
  auto cov_expr = local_map.remap_expr(local_expr);
  return expr & cov_expr;
}

END_NAMESPACE_DRUID
