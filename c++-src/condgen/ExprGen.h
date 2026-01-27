#ifndef EXPRGEN_H
#define EXPRGEN_H

/// @file ExprGen.h
/// @brief ExprGen のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "condgen/DetCond.h"
#include "ym/Expr.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class Exprgen Exprgen.h "Exprgen.h"
/// @brief DetCond を Expr に変換する処理を行うクラス
///
/// このクラスは仮想関数を定義するだけの純粋仮想基底クラス
//////////////////////////////////////////////////////////////////////
class ExprGen
{
public:

  /// @brief 継承クラスを生成するクラスメソッド
  static
  std::unique_ptr<ExprGen>
  new_obj(
    const JsonValue& option ///< [in] オプション
  );

  /// @brief デストラクタ
  virtual
  ~ExprGen() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief DetCond を Expr に変換する．
  Expr
  cond_to_expr(
    const DetCond& cond ///< [in] 条件
  );


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief DetCond::CondData を Expr に変換する．
  virtual
  Expr
  conv(
    const DetCond::CondData& data ///< [in] 条件を表すデータ
  ) = 0;

};

END_NAMESPACE_DRUID

#endif // EXPRGEN_H
