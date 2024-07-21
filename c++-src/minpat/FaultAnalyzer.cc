
/// @file FaultAnalyzer.cc
/// @brief FaultAnalyzer の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "FaultAnalyzer.h"
#include "TpgFFR.h"
#include "TpgFault.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
FaultAnalyzer::FaultAnalyzer(
  const TpgNetwork& network,
  const TpgFFR* ffr,
  const JsonValue& option
) : mBaseEnc{network, option}
{
  auto root = ffr->root();
  mBdEnc = new BoolDiffEnc{mBaseEnc, root, option};
  mBaseEnc.make_cnf({}, {root});
}

// @brief 故障検出の十分条件と必要条件を求める．
bool
FaultAnalyzer::extract_condition(
  const TpgFault* fault,
  NodeTimeValList& sufficient_condition,
  NodeTimeValList& mandatory_condition
)
{
  auto ffr_cond = fault->ffr_propagate_condition();
  auto assumptions = mBaseEnc.conv_to_literal_list(ffr_cond);
  assumptions.push_back(mBdEnc->prop_var());
  auto ffr_mand_cond = root_mandatory_condition();
  auto ffr_assumptions = mBaseEnc.conv_to_literal_list(ffr_mand_cond);
  assumptions.insert(assumptions.end(),
		     ffr_assumptions.begin(), ffr_assumptions.end());
  auto res = mBaseEnc.solver().solve(assumptions);
  ASSERT_COND( res == SatBool3::True );
  sufficient_condition = mBdEnc->extract_sufficient_condition();
  auto tmp_cond{sufficient_condition};
  tmp_cond.diff(ffr_mand_cond);
  auto assumptions1{assumptions};
  assumptions1.push_back(SatLiteral::X);
  bool trivial = true;
  for ( auto nv: tmp_cond ) {
    auto lit = mBaseEnc.conv_to_literal(nv);
    assumptions1.back() = ~lit;
    auto res = mBaseEnc.solver().solve(assumptions1);
    if ( res == SatBool3::False ) {
      mandatory_condition.add(nv);
    }
    else {
      trivial = false;
    }
  }
  sufficient_condition.merge(ffr_cond);
  sufficient_condition.merge(ffr_mand_cond);
  mandatory_condition.merge(ffr_cond);
  mandatory_condition.merge(ffr_mand_cond);
  return trivial;
}

// @brief FFR の根の故障伝搬の必要条件を求める．
const NodeTimeValList&
FaultAnalyzer::root_mandatory_condition()
{
  if ( !mDone ) {
    auto pvar = mBdEnc->prop_var();
    auto res = mBaseEnc.solver().solve({pvar});
    if ( res == SatBool3::True ) {
      auto suff_cond = mBdEnc->extract_sufficient_condition();
      for ( auto nv: suff_cond ) {
	auto lit = mBaseEnc.conv_to_literal(nv);
	if ( mBaseEnc.solver().solve({pvar, ~lit}) == SatBool3::False ) {
	  mRootMandCond.add(nv);
	}
      }
    }
    mDone = true;
  }
  return mRootMandCond;
}

END_NAMESPACE_DRUID
