
/// @file CondGenChecker.cc
/// @brief CondGenChecker の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "CondGenChecker.h"
#include "TpgFFR.h"
#include "TpgFault.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
CondGenChecker::CondGenChecker(
  const TpgNetwork& network,
  const TpgFFR* ffr,
  const AssignExpr& cond,
  const JsonValue& option
) : mEngine{network, option}
{
  mBdEnc = new BoolDiffEnc{mEngine, ffr->root(), option};
  mEngine.make_cnf({}, {ffr->root()});
  make_cnf(cond.expr(), cond.assign_map());
}

// @brief コンストラクタ
CondGenChecker::CondGenChecker(
  const TpgNetwork& network,
  const TpgFFR* ffr,
  const TpgFault* fault,
  const AssignExpr& cond,
  const JsonValue& option
) : mEngine{network, option},
    mExtraCond{fault->ffr_propagate_condition()},
    mCond{cond}
{
  mBdEnc = new BoolDiffEnc{mEngine, ffr->root(), option};
  mEngine.make_cnf({}, {ffr->root()});
  make_cnf(mCond.expr(), mCond.assign_map());
}

// @breif 結果の検証を行う．
bool
CondGenChecker::check(
)
{
  auto pvar = mBdEnc->prop_var();
  auto extra_cond = mEngine.conv_to_literal_list(mExtraCond);
  extra_cond.push_back(~pvar);
  auto res = mEngine.solver().solve(extra_cond);
  if ( res != SatBool3::False ) {
    cout << mCond.expr() << endl;
  }
  return res == SatBool3::False;
}

// @brief cond の条件を CNF式に変換する．
void
CondGenChecker::make_cnf(
  const Expr& expr,
  const AssignMap& assign_map
)
{
  if ( expr.is_one() ) {
    return;
  }
  if ( expr.is_zero() ) {
    abort();
  }
  if ( expr.is_literal() ) {
    auto as = assign_map.assign(expr.literal());
    auto lit = mEngine.conv_to_literal(as);
    mEngine.solver().add_clause(lit);
  }
  if ( expr.is_and() ) {
    auto n = expr.operand_num();
    for ( SizeType i = 0; i < n; ++ i ) {
      auto opr = expr.operand(i);
      make_cnf(opr, assign_map);
    }
  }
  if ( expr.is_or() ) {
    auto n = expr.operand_num();
    vector<SatLiteral> tmp_lits(n);
    for ( SizeType i = 0; i < n; ++ i ) {
      auto opr = expr.operand(i);
      auto lit = make_cnf2(opr, assign_map);
      tmp_lits[i] = lit;
    }
    mEngine.solver().add_clause(tmp_lits);
  }
}

// @brief cond の条件を CNF式に変換する．
SatLiteral
CondGenChecker::make_cnf2(
  const Expr& expr,
  const AssignMap& assign_map
)
{
  if ( expr.is_constant() ) {
    abort();
  }
  if ( expr.is_literal() ) {
    auto as = assign_map.assign(expr.literal());
    auto lit = mEngine.conv_to_literal(as);
    return lit;
  }
  auto lit = mEngine.solver().new_variable();
  if ( expr.is_and() ) {
    auto n = expr.operand_num();
    for ( SizeType i = 0; i < n; ++ i ) {
      auto opr = expr.operand(i);
      auto lit1 = make_cnf2(opr, assign_map);
      mEngine.solver().add_clause(~lit, lit1);
    }
  }
  if ( expr.is_or() ) {
    auto n = expr.operand_num();
    vector<SatLiteral> tmp_lits(n + 1);
    tmp_lits[0] = ~lit;
    for ( SizeType i = 0; i < n; ++ i ) {
      auto opr = expr.operand(i);
      tmp_lits[i + 1] = make_cnf2(opr, assign_map);
    }
    mEngine.solver().add_clause(tmp_lits);
  }
  return lit;
}

END_NAMESPACE_DRUID
