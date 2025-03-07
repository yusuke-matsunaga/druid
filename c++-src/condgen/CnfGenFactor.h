#ifndef CNFGENFACTOR_H
#define CNFGENFACTOR_H

/// @file CnfGenFactor.h
/// @brief CnfGenFactor のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "CnfGenNaive.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class CnfGenFactor CnfGenFactor.h "CnfGenFactor.h"
/// @brief カバーをCNFに変換する
//////////////////////////////////////////////////////////////////////
class CnfGenFactor :
  public CnfGenNaive
{
public:

  /// @brief コンストラクタ
  CnfGenFactor() = default;

  /// @brief デストラクタ
  ~CnfGenFactor() = default;


protected:
  //////////////////////////////////////////////////////////////////////
  // CnfGen の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief DetCond::CondData から Expr を作る．
  Expr
  cond_to_expr(
    const DetCond::CondData& cond
  ) override;

};

END_NAMESPACE_DRUID

#endif // CNFGENFACTOR_H
