#ifndef CNFGENNAIVE_H
#define CNFGENNAIVE_H

/// @file CnfGenNaive.h
/// @brief CnfGenNaive のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "CnfGen.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class CnfGenNaive CnfGenNaive.h "CnfGenNaive.h"
/// @brief カバーをCNFに変換する
//////////////////////////////////////////////////////////////////////
class CnfGenNaive :
  public CnfGen
{
public:

  /// @brief コンストラクタ
  CnfGenNaive() = default;

  /// @brief デストラクタ
  ~CnfGenNaive() = default;


protected:
  //////////////////////////////////////////////////////////////////////
  // CnfGen の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief DetCond::CondData から Expr を作る．
  Expr
  cond_to_expr(
    const DetCond::CondData& cond
  ) override;

  /// @brief Expr のリストから CNF を作る．
  vector<vector<SatLiteral>>
  expr_to_cnf(
    StructEngine& engine,
    const vector<Expr>& expr_list
  ) override;

  /// @brief Expr のリストから CNF サイズを見積もる．
  CnfSize
  expr_cnf_size(
    const vector<Expr>& expr_list
  ) override;

};

END_NAMESPACE_DRUID

#endif // CNFGENNAIVE_H
