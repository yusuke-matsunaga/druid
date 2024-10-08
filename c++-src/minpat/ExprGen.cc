
/// @file ExprGen.cc
/// @brief ExprGen の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "ExprGen.h"
#include "TpgNetwork.h"
#include "TpgFFR.h"
#include "TpgNode.h"
#include "TpgFault.h"
#include "StructEngine.h"
#include "BoolDiffEnc.h"
#include "TestVector.h"
#include "ym/JsonValue.h"


#define DBG_OUT cerr

BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
ExprGen::ExprGen(
  const TpgNetwork& network,
  const TpgFFR* ffr,
  const JsonValue& option
) : mFFR{ffr},
    mEngine{network, option}
{
  mLimit = 100;
  if ( option.is_object() ) {
    if ( option.has_key("limit") ) {
      mLimit = option.get("limit").get_int();
    }
    if ( option.has_key("debug") ) {
      mDebug = option.get("debug").get_int();
    }
  }
  auto root = mFFR->root();
  mBdEnc = new BoolDiffEnc{mEngine, root, option};
  mEngine.make_cnf({}, {root});
}

// @brief デストラクタ
ExprGen::~ExprGen()
{
}

// @brief 対象のFFRの根のノードのブール微分を表す論理式を得る．
SizeType
ExprGen::run(
  const TpgFault* fault
)
{
  if ( fault->ffr_root() != mFFR->root() ) {
    ostringstream buf;
    buf << fault->str() << " is not in the FFR";
    throw std::invalid_argument{buf.str()};
  }

  // FFR の出力の伝搬可能性を調べる．
  Timer timer;
  timer.start();
  vector<AssignExpr> tmp_list;
  auto pvar = mBdEnc->prop_var();
  auto ffr_cond = fault->ffr_propagate_condition();
  auto assumptions = mEngine.conv_to_literal_list(ffr_cond);
  assumptions.push_back(pvar);

  // 制御用の変数を用意する．
  auto clit = mEngine.solver().new_variable(false);
  assumptions.push_back(clit);
  SizeType count = 0;
  for ( ; count < mLimit; ++ count ) {
    auto status = mEngine.solver().solve(assumptions);
    if ( status != SatBool3::True ) {
      break;
    }

    // 十分条件を取り出す．
    auto expr = mBdEnc->extract_sufficient_conditions();
#if 0
    {
      cout << endl;
      cout << "expr = " << expr << endl;
    }
#endif
    tmp_list.push_back(expr);
    // expr の否定を加える．
    add_negation(clit, expr);
  }
  timer.stop();

  auto expr = AssignExpr::make_or(tmp_list);
  if ( mDebug > 1 ) {
    DBG_OUT << "Fault#" << std::left << std::setw(5) << fault->id()
	    << ": " << setw(7) << count
	    << ": " << (timer.get_time() / 1000.0) << endl;
  }
  return count;
}

// @brief expr を否定した節を加える．
void
ExprGen::add_negation(
  SatLiteral clit,
  const AssignExpr& expr
)
{
  if ( expr.is_literal() ) {
    auto nv = expr.literal();
    auto lit = mEngine.conv_to_literal(nv);
    mEngine.solver().add_clause(~clit, ~lit);
    return;
  }

  auto& opr_list = expr.opr_list();
  if ( expr.is_and() ) {
    vector<SatLiteral> lit_list;
    SizeType n = opr_list.size();
    lit_list.reserve(n + 1);
    lit_list.push_back(~clit);
    for ( auto& opr: opr_list ) {
      auto lit = add_negation_sub(opr);
      lit_list.push_back(lit);
    }
    mEngine.solver().add_clause(lit_list);
  }
  else { // expr.is_or()
    for ( auto& opr: opr_list ) {
      add_negation(clit, opr);
      if ( !mEngine.solver().sane() ) {
	break;
      }
    }
  }
}

// @brief expr を否定した節を加える．
SatLiteral
ExprGen::add_negation_sub(
  const AssignExpr& expr
)
{
  if ( expr.is_literal() ) {
    auto nv = expr.literal();
    auto lit = mEngine.conv_to_literal(nv);
    return ~lit;
  }

  auto& opr_list = expr.opr_list();
  SizeType n = opr_list.size();
  auto lit0 = mEngine.solver().new_variable();
  if ( expr.is_and() ) {
    vector<SatLiteral> lit_list;
    lit_list.reserve(n + 1);
    lit_list.push_back(~lit0);
    for ( auto& opr: opr_list ) {
      auto lit = add_negation_sub(opr);
      if ( !mEngine.solver().sane() ) {
	break;
      }
      lit_list.push_back(lit);
    }
    mEngine.solver().add_clause(lit_list);
  }
  else { // expr.is_or()
    for ( auto& opr: opr_list ) {
      auto lit = add_negation_sub(opr);
      mEngine.solver().add_clause(~lit0, lit);
      if ( !mEngine.solver().sane() ) {
	break;
      }
    }
  }
  return lit0;
}

END_NAMESPACE_DRUID
