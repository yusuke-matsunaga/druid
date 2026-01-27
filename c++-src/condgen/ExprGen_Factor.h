#ifndef EXPRGEN_FACTOR_H
#define EXPRGEN_FACTOR_H

/// @file ExprGen_Factor.h
/// @brief ExprGen_Factor のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ExprGen.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class ExprGen_Factor ExprGen_Factor.h "ExprGen_Factor.h"
/// @brief 積和形論理式に変換した跡でファクタリングを行う ExprGen
//////////////////////////////////////////////////////////////////////
class ExprGen_Factor :
  public ExprGen
{
public:

  /// @brief コンストラクタ
  ExprGen_Factor() = default;

  /// @brief デストラクタ
  ~ExprGen_Factor() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // ExprGen の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief DetCond::CondData から Expr を作る．
  Expr
  conv(
    const DetCond::CondData& cond
  ) override;

};

END_NAMESPACE_DRUID

#endif // EXPRGEN_FACTOR_H
