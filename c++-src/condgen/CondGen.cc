
/// @file CondGen.cc
/// @brief CondGen の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "CondGen.h"
#include "TpgNetwork.h"
#include "TpgFFR.h"
#include "TpgNode.h"
#include "TpgFault.h"
#include "OpBase.h"


#define DBG_OUT cerr

BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス CondGen
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
CondGen::CondGen(
  const TpgNetwork& network,
  const TpgFFR* ffr,
  const JsonValue& option
) : CondGen{network, ffr, {}, option}
{
  // FFR の出力の伝搬可能性を調べる．
  Timer timer;
  timer.start();
  auto pvar = mBdEnc->prop_var();
  mRootStatus = mEngine.solver().solve({pvar});
  if ( mRootStatus == SatBool3::True ) {
    // 必要条件を求める．
    auto suff_cond = mBdEnc->extract_sufficient_condition();
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
}

// @brief コンストラクタ
CondGen::CondGen(
  const TpgNetwork& network,
  const TpgFFR* ffr,
  const AssignList& root_cond,
  const JsonValue& option
) : mFFR{ffr},
    mEngine{network, option},
    mRootMandCond{root_cond},
    mDebug{OpBase::get_debug(option)}
{
  mBdEnc = new BoolDiffEnc{mEngine, ffr->root(), option};
  mEngine.make_cnf({}, {ffr->root()});
}

// @brief デストラクタ
CondGen::~CondGen()
{
}

// @brief FFRの出力の故障伝搬条件を求める．
// @return 条件式を返す．
DetCond
CondGen::root_cond(
  SizeType limit
)
{
  return gen_cond({}, limit);
}

// @brief 与えられた故障を検出するテストキューブを生成する．
DetCond
CondGen::fault_cond(
  const TpgFault* fault,
  SizeType limit
)
{
  if ( fault->ffr_root() != mFFR->root() ) {
    ostringstream buf;
    buf << fault->str() << " is not in the FFR";
    throw std::invalid_argument{buf.str()};
  }
  auto ffr_cond = fault->ffr_propagate_condition();
  return gen_cond(ffr_cond, limit);
}

// @brief root_cond(), fault_cond() の共通な下請け関数
DetCond
CondGen::gen_cond(
  const AssignList& extra_cond,
  SizeType limit
)
{
  Timer timer;
  timer.start();
  auto plit = mBdEnc->prop_var();
  AssignList precond{extra_cond};
  precond.merge(mRootMandCond);
  auto assumptions = mEngine.conv_to_literal_list(precond);
  assumptions.push_back(plit);
  auto res = mEngine.solver().solve(assumptions);
  timer.stop();
  if ( mDebug > 1 ) {
    DBG_OUT << "DTPG: " << (timer.get_time() / 1000.0) << endl;
  }
  if ( res != SatBool3::True ) {
    // 検出可能ではなかった．
    return DetCond::undetected();
  }
  timer.reset();
  timer.start();
  // 最初の十分条件を取り出す．
  auto suff_cond = mBdEnc->extract_sufficient_condition();
  // suff_cond のなかの必要条件を求める．
  auto tmp_cond = suff_cond;
  tmp_cond.diff(mRootMandCond);
  AssignList mand_cond;
  auto assumptions1 = assumptions;
  assumptions1.push_back(SatLiteral::X);
  for ( auto nv: tmp_cond ) {
    auto lit = mEngine.conv_to_literal(nv);
    assumptions1.back() = ~lit;
    if ( mEngine.solver().solve(assumptions1) == SatBool3::False ) {
      mand_cond.add(nv);
    }
  }
  suff_cond.diff(mand_cond);
  mand_cond.merge(precond);
  timer.stop();

  if ( mDebug > 1 ) {
    DBG_OUT << "PHASE1: " << (timer.get_time() / 1000.0) << endl;
  }

  SizeType loop_count = 1;

  if ( suff_cond.size() == 0 ) {
    // 十分条件と必要条件が等しかった．
    return DetCond{mand_cond};
  }

  timer.reset();
  timer.start();
  vector<AssignList> cube_list;
  cube_list.push_back(suff_cond);

  bool found = false;

  // 制御用の変数を用意する．
  auto clit = mEngine.solver().new_variable(false);
  for ( ; loop_count < limit; ++ loop_count ) {
    Timer timer;
    timer.start();
    // suff_cond を否定した節を加える．
    // ただし他の故障の処理のときには無効化したいので
    // 制御変数をつけておく．
    vector<SatLiteral> tmp_lits;
    tmp_lits.reserve(suff_cond.size() + 1);
    tmp_lits.push_back(~clit);
    for ( auto nv: suff_cond ) {
      auto lit = mEngine.conv_to_literal(nv);
      tmp_lits.push_back(~lit);
    }
    mEngine.solver().add_clause(tmp_lits);
    auto tmp_assumptions = mEngine.conv_to_literal_list(mand_cond);
    tmp_assumptions.push_back(plit);
    tmp_assumptions.push_back(clit);
    auto res = mEngine.solver().solve(tmp_assumptions);
    timer.stop();
    if ( mDebug > 2 ) {
      DBG_OUT << "  " << (timer.get_time() / 1000.0) << endl;
    }
    if ( res != SatBool3::True ) {
      // すべてのキューブを生成した．
      found = true;
      break;
    }
    suff_cond = mBdEnc->extract_sufficient_condition();
    suff_cond.diff(mand_cond);
    if ( suff_cond.size() == 0 ) {
      // 最初に生成された suff_cond が冗長だった．
      // 結局 mand_cond が唯一の条件となる．
      cube_list.clear();
      cube_list.push_back(suff_cond);
      break;
    }
    cube_list.push_back(suff_cond);
  }
  timer.stop();

  if ( mDebug > 1 ) {
    DBG_OUT << "PHASE2: " << (timer.get_time() / 1000.0) << endl;
  }

  // 生成された結果を論理式の形に変換する．
  if ( found ) {
    return DetCond(mand_cond, cube_list);
  }
  return DetCond::overflow();
}

END_NAMESPACE_DRUID
