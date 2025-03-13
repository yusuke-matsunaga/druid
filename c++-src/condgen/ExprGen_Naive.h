#ifndef EXPRGEN_NAIVE_H
#define EXPRGEN_NAIVE_H

/// @file ExprGen_Naive.h
/// @brief ExprGen_Naive のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ExprGen.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class ExprGen_Naive ExprGen_Naive.h "ExprGen_Naive.h"
/// @brief 単純に積和形論理式に変換する ExprGen
//////////////////////////////////////////////////////////////////////
class ExprGen_Naive :
  public ExprGen
{
public:

  /// @brief コンストラクタ
  ExprGen_Naive() = default;

  /// @brief デストラクタ
  ~ExprGen_Naive() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // ExprGen の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief DetCond::CondData を Expr に変換する．
  Expr
  conv(
    const DetCond::CondData& cond ///< [in] 条件を表すデータ
  ) override;

};

END_NAMESPACE_DRUID

#endif // EXPRGEN_NAIVE_H
