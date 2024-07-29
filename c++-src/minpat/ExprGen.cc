
/// @file ExCubeGen.cc
/// @brief ExCubeGen の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "ExCubeGen.h"
#include "TpgNetwork.h"
#include "TpgFFR.h"
#include "TpgNode.h"
#include "TpgFault.h"
#include "BaseEnc.h"
#include "BoolDiffEnc.h"
#include "TestVector.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
ExCubeGen::ExCubeGen(
  const TpgNetwork& network,
  const TpgFFR* ffr,
  const JsonValue& option
) : mFFR{ffr},
    mBaseEnc{network, option}
{
  mLimit = 1;
  if ( option.is_object() ) {
    if ( option.has_key("cube_per_fault") ) {
      mLimit = option.get("cube_per_fault").get_int();
    }
    if ( option.has_key("debug") ) {
      mDebug = option.get("debug").get_bool();
    }
  }
  mBdEnc = new BoolDiffEnc{mBaseEnc, ffr->root(), option};
  mBaseEnc.make_cnf({}, {ffr->root()});
}

// @brief デストラクタ
ExCubeGen::~ExCubeGen()
{
}

// @brief 与えられた故障を検出するテストキューブを生成する．
TestCover
ExCubeGen::run(
  const TpgFault* fault
)
{
  if ( fault->ffr_root() != mFFR->root() ) {
    ostringstream buf;
    buf << fault->str() << " is not in the FFR";
    throw std::invalid_argument{buf.str()};
  }
#if 0
  auto plit = mBdEnc->prop_var();
  auto clit = mBaseEnc.solver().new_variable(false);
  vector<AssignList> cube_list;
  auto new_cond = suff_cond;
  for ( ; ; ) {
    new_cond.diff(mand_cond);
    if ( new_cond.size() == 0 ) {
      // 最初に生成された suff_cond が冗長だった．
      // 結局 mand_cond が唯一の条件となる．
      cube_list.clear();
      cube_list.push_back(new_cond);
      break;
    }
    cube_list.push_back(new_cond);
    if ( cube_list.size() >= mLimit ) {
      break;
    }
    // new_cond を否定した節を加える．
    // ただし他の故障の処理のときには無効化したいので
    // 制御変数をつけておく．
    vector<SatLiteral> tmp_lits;
    tmp_lits.reserve(new_cond.size() + 1);
    tmp_lits.push_back(~clit);
    for ( auto nv: new_cond ) {
      auto lit = mBaseEnc.conv_to_literal(nv);
      tmp_lits.push_back(~lit);
    }
    mBaseEnc.solver().add_clause(tmp_lits);
    auto assumptions = mBaseEnc.conv_to_literal_list(mand_cond);
    assumptions.push_back(plit);
    assumptions.push_back(clit);
    auto res = mBaseEnc.solver().solve(assumptions);
    if ( res != SatBool3::True ) {
      // すべてのキューブを生成した．
      break;
    }
    new_cond = mBdEnc->extract_sufficient_condition();
    new_cond.merge(mand_cond);
  }
#else
#warning "TODO: 十分条件の列挙"
  auto plit = mBdEnc->prop_var();
  auto clit = mBaseEnc.solver().new_variable(false);
  AssignExpr expr;
  AssignExpr new_expr = suff_cond;
#if 0
  for ( ; ; ) {
    // new_expr = new_expr & ~mand_cond
    // new_expr が空なら終わる．
    // この場合，mand_cond のみが条件となる．
    if ( new_cond.size() == 0 ) {
      // 最初に生成された suff_cond が冗長だった．
      // 結局 mand_cond が唯一の条件となる．
      cube_list.clear();
      cube_list.push_back(new_cond);
      break;
    }
    expr |= new_expr;
    // new_cond を否定した節を加える．
    // ただし他の故障の処理のときには無効化したいので
    // 制御変数をつけておく．
    add_negate(new_expr);
    auto assumptions = mBaseEnc.conv_to_literal_list(mand_cond);
    assumptions.push_back(plit);
    assumptions.push_back(clit);
    auto res = mBaseEnc.solver().solve(assumptions);
    if ( res != SatBool3::True ) {
      // すべてのキューブを生成した．
      break;
    }
    new_expr = mBdEnc->extract_sufficient_conditions();
  }
#endif
#endif
  return TestCover{fault, expr};
}

END_NAMESPACE_DRUID
