
/// @file CondGen2.cc
/// @brief CondGen2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "CondGen2.h"
#include "TpgNetwork.h"
#include "TpgFFR.h"
#include "TpgNode.h"
#include "TpgFault.h"
#include "AssignMgr.h"
#include "OpBase.h"


#define DBG_OUT cerr

BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

Expr
conv2expr(
  const AssignList& cube,
  AssignMgr& assign_mgr
)
{
  vector<Expr> opr_list;
  opr_list.reserve(cube.size());
  for ( auto nv: cube ) {
    auto var = assign_mgr.get_varid(nv);
    bool inv = !nv.val();
    auto lit = Expr::literal(var, inv);
    opr_list.push_back(lit);
  }
  return Expr::and_op(opr_list);
}

string
print_cube(
  const AssignList& cube,
  const AssignMap& assign_map
)
{
  std::unordered_map<Assign, SizeType> var_map;
  auto n = assign_map.variable_num();
  for ( SizeType i = 0; i < n; ++ i ) {
    auto assign = assign_map.assign(i);
    var_map.emplace(assign, i);
  }
  ostringstream s;
  const char* sep = "";
  for ( auto nv: cube ) {
    auto nv0 = nv.val() ? nv : ~nv;
    auto var = var_map.at(nv0);
    s << sep;
    sep = " & ";
    if ( !nv.val() ) {
      s << "~";
    }
    s << var;
  }
  return s.str();
}

END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// クラス CondGen2
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
CondGen2::CondGen2(
  const TpgNetwork& network,
  const TpgFFR* ffr,
  const JsonValue& option
) : mFFR{ffr},
    mDebug{OpBase::get_debug(option)}
{
  mBdEnc = new BoolDiffEnc(ffr->root(), option);
  StructEngine::Builder builder;
  builder.add_subenc(mBdEnc);
  builder.add_extra_prev_node(ffr->root());
  mEngine = builder.new_obj(network, option);

  // FFR の出力の伝搬可能性を調べる．
  Timer timer;
  timer.start();
  auto pvar = mBdEnc->prop_var();
  mRootStatus = mEngine->solver().solve({pvar});
  if ( mRootStatus == SatBool3::True ) {
    // 必要条件を求める．
    auto suff_cond = mBdEnc->extract_sufficient_condition();
    for ( auto nv: suff_cond ) {
      auto lit = mEngine->conv_to_literal(nv);
      if ( mEngine->solver().solve({pvar, ~lit}) == SatBool3::False ) {
	mRootMandCond.add(nv);
      }
    }
  }
  timer.stop();

  if ( mDebug > 1 ) {
    DBG_OUT << "FFR#" << ffr->id()
	    << ": " << mRootMandCond.size()
	    << ": " << (timer.get_time() / 1000.0) << endl;
  }
}

// @brief コンストラクタ
CondGen2::CondGen2(
  const TpgNetwork& network,
  const TpgFFR* ffr,
  const AssignList& root_cond,
  const JsonValue& option
) : mFFR{ffr},
    mBdEnc{new BoolDiffEnc(ffr->root(), option)},
    mRootMandCond{root_cond},
    mDebug{OpBase::get_debug(option)}
{
  StructEngine::Builder builder;
  builder.add_subenc(mBdEnc);
  builder.add_extra_prev_node(ffr->root());
  mEngine = builder.new_obj(network, option);

  if ( mDebug > 1 ) {
    DBG_OUT << "FFR#" << ffr->id()
	    << ": " << mRootMandCond.size() << endl;
  }
}

// @brief デストラクタ
CondGen2::~CondGen2()
{
}

// @brief FFRの出力の故障伝搬条件を求める．
// @return 条件式を返す．
AssignExpr
CondGen2::root_cond(
  SizeType limit,
  SizeType& loop_count
)
{
  return gen_cond({}, limit, loop_count);
}

// @brief 与えられた故障を検出するテストキューブを生成する．
AssignExpr
CondGen2::fault_cond(
  const TpgFault* fault,
  SizeType limit,
  SizeType& loop_count
)
{
  if ( fault->ffr_root() != mFFR->root() ) {
    ostringstream buf;
    buf << fault->str() << " is not in the FFR";
    throw std::invalid_argument{buf.str()};
  }
  auto ffr_cond = fault->ffr_propagate_condition();
  return gen_cond(ffr_cond, limit, loop_count);
}

BEGIN_NONAMESPACE

Expr
remake(
  const Expr& expr,
  const AssignMap& old_map,
  AssignMgr& assign_mgr
)
{
  if ( expr.is_literal() ) {
    auto lit = expr.literal();
    auto as = old_map.assign(lit);
    auto new_var = assign_mgr.get_varid(as);
    return Expr::literal(new_var, lit.is_negative());
  }
  auto n = expr.operand_num();
  vector<Expr> opr_list(n);
  for ( SizeType i = 0; i < n; ++ i ) {
    auto opr = expr.operand(i);
    opr_list[i] = remake(opr, old_map, assign_mgr);
  }
  if ( expr.is_and() ) {
    return Expr::and_op(opr_list);
  }
  if ( expr.is_or() ) {
    return Expr::or_op(opr_list);
  }
  if ( expr.is_xor() ) {
    return Expr::xor_op(opr_list);
  }
  ASSERT_NOT_REACHED;
  return Expr::zero();
}

// 共通なキューブを求める．
AssignList
common_cube(
  const AssignExpr& src_expr
)
{
  auto expr = src_expr.expr();
  if ( expr.is_literal() ) {
    auto as = src_expr.assign(0);
    if ( expr.is_nega_literal() ) {
      as = ~as;
    }
    return AssignList{vector<Assign>{as}};
  }
  if ( expr.is_and() ) {
    auto n = expr.operand_num();
    vector<Assign> tmp_list;
    tmp_list.reserve(n);
    for ( SizeType i = 0; i < n; ++ i ) {
      auto opr = expr.operand(i);
      if ( opr.is_literal() ) {
	auto lit = opr.literal();
	auto as = src_expr.assign(lit);
	tmp_list.push_back(as);
      }
    }
    return AssignList{tmp_list};
  }
  // それ以外はなし
  return AssignList{};
}

Expr
cofactor_sub(
  const Expr& expr,
  const std::unordered_set<Assign>& cube_hash,
  const AssignMap& assign_map
)
{
  if ( expr.is_literal() ) {
    auto as = assign_map.assign(expr.literal());
    if ( cube_hash.count(as) > 0 ) {
      return Expr::one();
    }
    auto as_inv = ~as;
    if ( cube_hash.count(as_inv) > 0 ) {
      return Expr::zero();
    }
    return expr;
  }
  auto n = expr.operand_num();
  vector<Expr> new_opr_list(n);
  for ( SizeType i = 0; i < n; ++ i ) {
    auto opr = expr.operand(i);
    new_opr_list[i] = cofactor_sub(opr, cube_hash, assign_map);
  }
  if ( expr.is_and() ) {
    return Expr::and_op(new_opr_list);
  }
  if ( expr.is_or() ) {
    return Expr::or_op(new_opr_list);
  }
  if ( expr.is_xor() ) {
    return Expr::xor_op(new_opr_list);
  }
  ASSERT_NOT_REACHED;
  return Expr::zero();
}

// cube によるコファクターを計算する．
// ただし cube 中のリテラルが assign_map に存在するとは限らない．
AssignExpr
cofactor(
  const AssignExpr& expr,
  const AssignList& cube
)
{
  if ( cube.size() == 0 ) {
    return expr;
  }
  std::unordered_set<Assign> cube_hash;
  for ( auto as: cube ) {
    cube_hash.emplace(as);
  }
  auto& assign_map = expr.assign_map();
  auto new_expr = cofactor_sub(expr.expr(), cube_hash, assign_map);
  return AssignExpr{new_expr, assign_map.assign_list()};
}

AssignExpr
cube_diff(
  const AssignExpr& src_expr,
  const AssignList& cube
)
{
  if ( cube.size() == 0 ) {
    return src_expr;
  }
  std::unordered_set<Assign> cube_hash;
  for ( auto as: cube ) {
    cube_hash.emplace(as);
  }
  auto expr = src_expr.expr();
  if ( expr.is_literal() ) {
    auto as = src_expr.assign(expr.literal());
    if ( cube_hash.count(as) > 0 ) {
      return AssignExpr{Expr::one(), {}};
    }
    return src_expr;
  }
  if ( expr.is_and() ) {
    auto n = expr.operand_num();
    vector<Expr> new_opr_list;
    new_opr_list.reserve(n);
    for ( SizeType i = 0; i < n; ++ i ) {
      auto opr = expr.operand(i);
      if ( opr.is_literal() ) {
	auto as = src_expr.assign(opr.literal());
	if ( cube_hash.count(as) > 0 ) {
	  continue;
	}
      }
      new_opr_list.push_back(opr);
    }
    if ( new_opr_list.size() == 0 ) {
      return AssignExpr{Expr::one(), {}};
    }
    return AssignExpr{Expr::and_op(new_opr_list),
		      src_expr.assign_map().assign_list()};
  }
  if ( expr.is_or() ) {
    auto n = expr.operand_num();
    vector<Expr> new_opr_list;
    new_opr_list.reserve(n);
    bool found = false;
    for ( SizeType i = 0; i < n; ++ i ) {
      auto opr = expr.operand(i);
      if ( opr.is_literal() ) {
	auto as = src_expr.assign(opr.literal());
	if ( cube_hash.count(as) > 0 ) {
	  found = true;
	  break;
	}
      }
      new_opr_list.push_back(opr);
    }
    if ( new_opr_list.size() == 0 ) {
      return AssignExpr{Expr::one(), {}};
    }
    return AssignExpr{Expr::and_op(new_opr_list),
		      src_expr.assign_map().assign_list()};
  }
  {
    cout << "cube_diff() abort" << endl
	 << " expr: " << expr << endl
	 << " cube: " << print_cube(cube, src_expr.assign_map()) << endl;
  }
  ASSERT_NOT_REACHED;
  return AssignExpr{};
}

END_NONAMESPACE

// @brief root_cond(), fault_cond() の共通な下請け関数
AssignExpr
CondGen2::gen_cond(
  const AssignList& extra_cond,
  SizeType limit,
  SizeType& loop_count
)
{
  Timer timer;
  timer.start();
  auto plit = mBdEnc->prop_var();
  auto assumptions = mEngine->conv_to_literal_list(extra_cond);
  assumptions.push_back(plit);
  auto res = mEngine->solver().solve(assumptions);
  timer.stop();
  if ( mDebug > 1 ) {
    DBG_OUT << "DTPG: " << (timer.get_time() / 1000.0) << endl;
  }
  if ( res != SatBool3::True ) {
    // 検出可能ではなかった．
    loop_count = 0;
    return {};
  }
  timer.reset();
  timer.start();
  // 最初の十分条件を取り出す．
  auto suff_cond = mBdEnc->extract_sufficient_conditions();
  if ( 0 ) {
    cout << "suff_cond0: " << suff_cond.expr() << endl;
  }
  // suff_cond のなかの必要条件を求める．
  auto tmp_cond = common_cube(suff_cond);
  if ( 0 ) {
    cout << "common_cube: " << tmp_cond << endl;
  }
  tmp_cond.diff(mRootMandCond);
  AssignList mand_cond;
  auto assumptions1 = assumptions;
  assumptions1.push_back(SatLiteral::X);
  for ( auto nv: tmp_cond ) {
    auto lit = mEngine->conv_to_literal(nv);
    assumptions1.back() = ~lit;
    if ( mEngine->solver().solve(assumptions1) == SatBool3::False ) {
      mand_cond.add(nv);
    }
  }
  suff_cond = cofactor(suff_cond, mand_cond);
  mand_cond.merge(extra_cond);
  mand_cond.merge(mRootMandCond);
  timer.stop();

  if ( mDebug > 1 ) {
    DBG_OUT << "PHASE1: " << (timer.get_time() / 1000.0) << endl;
  }

  loop_count = 1;

  if ( suff_cond.expr().is_one() ) {
    // 十分条件と必要条件が等しかった．
    AssignMgr assign_mgr;
    auto expr = conv2expr(mand_cond, assign_mgr);
    return AssignExpr{expr, assign_mgr.assign_list()};
  }

  timer.reset();
  timer.start();
  // suff_cond を 新しい assign_mgr の元で作り直す．
  AssignMgr assign_mgr;
  auto expr = remake(suff_cond.expr(),
		     suff_cond.assign_map(),
		     assign_mgr);
  if ( 0 ) {
    cout << expr << endl;
  }
  auto or_expr = expr;

  // 制御用の変数を用意する．
  auto clit = mEngine->solver().new_variable(false);
  for ( ; loop_count < limit; ++ loop_count ) {
    Timer timer;
    timer.start();
    // expr を否定した節を加える．
    // ただし他の故障の処理のときには無効化したいので
    // 制御変数をつけておく．
    add_negate(expr, assign_mgr.assign_map(), clit);
    auto tmp_assumptions = mEngine->conv_to_literal_list(mand_cond);
    tmp_assumptions.push_back(plit);
    tmp_assumptions.push_back(clit);
    auto res = mEngine->solver().solve(tmp_assumptions);
    timer.stop();
    if ( mDebug > 2 ) {
      DBG_OUT << "  " << (timer.get_time() / 1000.0) << endl;
    }
    if ( res != SatBool3::True ) {
      // すべてのキューブを生成した．p
      break;
    }
    suff_cond = mBdEnc->extract_sufficient_conditions();
    if ( 0 ) {
      cout << "suff_cond1: " << suff_cond.expr() << endl;
    }
    suff_cond = cofactor(suff_cond, mand_cond);
    if ( suff_cond.expr().is_one() ) {
      // 最初に生成された suff_cond が冗長だった．
      // 結局 mand_cond が唯一の条件となる．
      break;
    }
    expr = remake(suff_cond.expr(),
		  suff_cond.assign_map(),
		  assign_mgr);
    or_expr |= expr;
    if ( 0 ) {
      cout << expr << endl;
    }
  }
  timer.stop();

  if ( mDebug > 1 ) {
    DBG_OUT << "PHASE2: " << (timer.get_time() / 1000.0) << endl;
  }

  auto mand_cond_expr = conv2expr(mand_cond, assign_mgr);
  auto and_expr = mand_cond_expr & or_expr;
  return AssignExpr{and_expr, assign_mgr.assign_list()};
}

// expr を否定した項を追加する．
void
CondGen2::add_negate(
  const Expr& expr,
  const AssignMap& assign_map,
  SatLiteral clit
)
{
  if ( expr.is_constant() ) {
    abort();
  }
  if ( expr.is_literal() ) {
    auto as = assign_map.assign(expr.literal());
    auto lit = mEngine->conv_to_literal(as);
    mEngine->solver().add_clause(~clit, ~lit);
    return;
  }
  auto n = expr.operand_num();
  if ( expr.is_and() ) {
    vector<SatLiteral> tmp_list;
    tmp_list.reserve(n + 1);
    tmp_list.push_back(~clit);
    for ( SizeType i = 0; i < n; ++ i ) {
      auto opr = expr.operand(i);
      if ( opr.is_literal() ) {
	auto as = assign_map.assign(opr.literal());
	auto lit = mEngine->conv_to_literal(as);
	tmp_list.push_back(~lit);
      }
      else {
	auto lit = mEngine->solver().new_variable();
	add_negate(opr, assign_map, ~lit);
	tmp_list.push_back(~lit);
      }
    }
    mEngine->solver().add_clause(tmp_list);
    return;
  }
  if ( expr.is_or() ) {
    for ( SizeType i = 0; i < n; ++ i ) {
      auto opr = expr.operand(i);
      if ( opr.is_literal() ) {
	auto as = assign_map.assign(opr.literal());
	auto lit = mEngine->conv_to_literal(as);
	mEngine->solver().add_clause(~clit, ~lit);
      }
      else {
	auto lit = mEngine->solver().new_variable();
	add_negate(opr, assign_map, ~lit);
	mEngine->solver().add_clause(~clit, ~lit);
      }
    }
    return;
  }
  ASSERT_NOT_REACHED;
}

END_NAMESPACE_DRUID
