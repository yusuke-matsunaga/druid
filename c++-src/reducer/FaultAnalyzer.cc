
/// @file FaultAnalyzer.cc
/// @brief FaultAnalyzer の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "FaultAnalyzer.h"
#include "TpgFFR.h"
#include "TpgFault.h"
#include "FaultInfo.h"
#include "OpBase.h"


#define DBG_OUT cerr

BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
FaultAnalyzer::FaultAnalyzer(
  const TpgNetwork& network,
  const TpgFFR* ffr,
  const JsonValue& option
) : mEngine(network, option),
    mDebug{OpBase::get_debug(option)}
{
  auto root = ffr->root();
  mBdEnc = new BoolDiffEnc(root, option);
  mEngine.add_subenc(std::unique_ptr<SubEnc>{mBdEnc});
  mEngine.add_prev_node(root);

  // FFR の出力の伝搬可能性を調べる．
  if ( mDebug > 1 ) {
    DBG_OUT << "FFR#" << ffr->id() << ": ";
    DBG_OUT.flush();
  }
  Timer timer;
  timer.start();
  auto pvar = mBdEnc->prop_var();
  mRootStatus = mEngine.solve({pvar});
  if ( mRootStatus == SatBool3::True ) {
    // 必要条件を求める．
    auto suff_cond = mBdEnc->extract_sufficient_condition();
    for ( auto nv: suff_cond ) {
      auto lit = mEngine.conv_to_literal(nv);
      if ( mEngine.solve({pvar, ~lit}) == SatBool3::False ) {
	mRootMandCond.add(nv);
      }
    }
  }
  timer.stop();
  if ( mDebug > 1 ) {
    DBG_OUT << mRootMandCond.size() << ": "
	    << (timer.get_time() / 1000.0) << endl;
  }
}

// @brief 故障検出の十分条件と必要条件を求める．
bool
FaultAnalyzer::run(
  FaultInfo& finfo
)
{
  auto fault = finfo.fault();
  if ( mRootStatus == SatBool3::True ) {
    Timer timer;
    timer.start();
    auto ffr_cond = fault->ffr_propagate_condition();
    auto root_cond = mRootMandCond;
    ffr_cond.merge(root_cond);
    auto assumptions = mEngine.conv_to_literal_list(ffr_cond);
    assumptions.push_back(mBdEnc->prop_var());
    auto res = mEngine.solve(assumptions);
    timer.stop();
    if ( mDebug > 1 ) {
      DBG_OUT << "  DTPG: " << fault->str() << ": "
	      << (timer.get_time() / 1000.0) << endl;
    }
    if ( res == SatBool3::True ) {
      timer.reset();
      timer.start();
      auto suff_cond = mBdEnc->extract_sufficient_condition();
      // あとで必要条件を求めるためにコピーしておく．
      auto tmp_cond{suff_cond};
      suff_cond.merge(ffr_cond);
      auto pi_assign = mEngine.justify(suff_cond);
      finfo.set_sufficient_condition(suff_cond, pi_assign);
      tmp_cond.diff(root_cond);
      auto assumptions1{assumptions};
      assumptions1.push_back(SatLiteral::X);
      AssignList mand_cond;
      for ( auto nv: tmp_cond ) {
	auto lit = mEngine.conv_to_literal(nv);
	assumptions1.back() = ~lit;
	auto res = mEngine.solve(assumptions1);
	if ( res == SatBool3::False ) {
	  mand_cond.add(nv);
	}
      }
      mand_cond.merge(ffr_cond);
      finfo.set_mandatory_condition(mand_cond);
      timer.stop();
      if ( mDebug > 1 ) {
	DBG_OUT << "  PHASE1: " << suff_cond.size() << " | "
		<< mand_cond.size() << ": "
		<< (timer.get_time() / 1000.0) << endl;
      }
      return true;
    }
    if ( res == SatBool3::False ) {
      finfo.set_untestable();
    }
  }
  if ( mRootStatus == SatBool3::False ) {
    finfo.set_untestable();
  }
  // mRootStatus == SatBool3::X の時はなにもしない．
  return false;
}

END_NAMESPACE_DRUID
