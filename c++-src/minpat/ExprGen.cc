
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


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
ExprGen::ExprGen(
  const TpgNetwork& network,
  const TpgNode* root,
  const JsonValue& option
) : mRoot{root},
    mEngine{network, option}
{
  mLimit = 1;
  if ( option.is_object() ) {
    if ( option.has_key("expr_per_fault") ) {
      mLimit = option.get("expr_per_fault").get_int();
    }
    if ( option.has_key("debug") ) {
      mDebug = option.get("debug").get_bool();
    }
  }
  mBdEnc = new BoolDiffEnc{mEngine, mRoot, option};
  mEngine.make_cnf({}, {mRoot});
}

// @brief デストラクタ
ExprGen::~ExprGen()
{
}

// @brief 対象のFFRの根のノードのブール微分を表す論理式を得る．
AssignExpr
ExprGen::run()
{
  // FFR の出力の伝搬可能性を調べる．
  Timer timer;
  timer.start();
  auto pvar = mBdEnc->prop_var();
  auto status = mEngine.solver().solve({pvar});
  if ( status == SatBool3::True ) {
    // 必要条件を求める．
    auto expr = mBdEnc->extract_sufficient_conditions();
    for ( auto nv: suff_cond ) {
      auto lit = mEngine.conv_to_literal(nv);
      if ( mEngine.solver().solve({pvar, ~lit}) == SatBool3::False ) {
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
  return TestCover{fault, expr};
}

END_NAMESPACE_DRUID
