
/// @file CnfGen.cc
/// @brief CnfGen の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "CnfGen.h"
#include "CnfGenImpl.h"
#include "CnfGenImpl2.h"
#include "CalcCnfImpl2.h"
#include "StructEngine.h"
#include "TpgNetwork.h"
#include "AssignExpr.h"
#include "ym/BddVar.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス CnfGen
//////////////////////////////////////////////////////////////////////

// @brief 論理式を CNF に変換する．
vector<SatLiteral>
CnfGen::make_cnf(
  StructEngine& engine,
  const AssignExpr& expr,
  const JsonValue& option
)
{
  CnfGenImpl gen{engine};
  vector<SatLiteral> assumptions;
  gen.make_cnf(expr.normalize(), assumptions);
  return assumptions;
}

// @brief 複数の論理式を CNF に変換する．
vector<vector<SatLiteral>>
CnfGen::make_cnf(
  StructEngine& engine,
  const vector<AssignExpr>& expr_list,
  const JsonValue& option
)
{
  vector<vector<SatLiteral>> assumptions_list;
  CnfGenImpl gen{engine};
  for ( auto expr: expr_list ) {
    vector<SatLiteral> assumptions;
    gen.make_cnf(expr.normalize(), assumptions);
    assumptions_list.push_back(assumptions);
  }
  return assumptions_list;
}

// @brief 論理式を CNF に変換した際の項数とリテラル数を数える．
CnfSize
CnfGen::calc_cnf_size(
  const AssignExpr& expr,
  const JsonValue& option
)
{
  if ( expr.expr().is_zero() ) {
    return CnfSize::zero();
  }
#if 1
  return CnfGenImpl::calc_cnf_size(expr.normalize());
#else
  CalcCnfImpl2 calc;
  calc.run(expr.normalize());
  return calc.cnf_size();
#endif
}

// @brief 複数の論理式を CNF に変換した際の項数とリテラル数を数える．
CnfSize
CnfGen::calc_cnf_size(
  const vector<AssignExpr>& expr_list,
  const JsonValue& option
)
{
  CnfSize size = CnfSize::zero();
  for ( auto expr: expr_list ) {
    size += CnfGenImpl::calc_cnf_size(expr.normalize());
  }
  return size;
}

END_NAMESPACE_DRUID
