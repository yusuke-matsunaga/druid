
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
#include "StructEngine.h"
#include "BoolDiffEnc.h"
#include "TestVector.h"
#include "OpBase.h"
#include "ym/JsonValue.h"


#define DBG_OUT cerr

BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
ExCubeGen::ExCubeGen(
  const TpgNetwork& network,
  const TpgFFR* ffr,
  const JsonValue& option
) : mFFR{ffr},
    mEngine{network, option},
    mDebug{OpBase::get_debug(option)}
{
  mLimit = 1;
  if ( option.is_object() ) {
    if ( option.has_key("limit") ) {
      mLimit = option.get("limit").get_int();
    }
  }
  mBdEnc = new BoolDiffEnc{mEngine, ffr->root(), option};
  mEngine.make_cnf({}, {ffr->root()});

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
ExCubeGen::ExCubeGen(
  const TpgNetwork& network,
  const TpgFFR* ffr,
  const AssignList& root_cond,
  const JsonValue& option
) : mFFR{ffr},
    mEngine{network, option},
    mRootMandCond{root_cond},
    mDebug{OpBase::get_debug(option)}
{
  mLimit = 1;
  if ( option.is_object() ) {
    if ( option.has_key("limit") ) {
      mLimit = option.get("limit").get_int();
    }
  }
  mBdEnc = new BoolDiffEnc{mEngine, ffr->root(), option};
  mEngine.make_cnf({}, {ffr->root()});

  if ( mDebug > 1 ) {
    DBG_OUT << "FFR#" << ffr->id()
	    << ": " << mRootMandCond.size() << endl;
  }
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
  Timer timer;
  timer.start();
  auto plit = mBdEnc->prop_var();
  auto ffr_cond = fault->ffr_propagate_condition();
  auto assumptions = mEngine.conv_to_literal_list(ffr_cond);
  assumptions.push_back(plit);
  auto res = mEngine.solver().solve(assumptions);
  timer.stop();
  if ( mDebug > 1 ) {
    DBG_OUT << "DTPG: " << (timer.get_time() / 1000.0) << endl;
  }
  if ( res != SatBool3::True ) {
    // fault が検出可能ではなかった．
    ostringstream buf;
    buf << fault->str() << " is untestable";
    throw std::invalid_argument{buf.str()};
  }
  timer.reset();
  timer.start();
  // 最初の十分条件を取り出す．
  auto suff_cond = mBdEnc->extract_sufficient_condition();
  // suff_cond のなかの必要条件を求める．
  auto tmp_cond{suff_cond};
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
  mand_cond.merge(ffr_cond);
  mand_cond.merge(mRootMandCond);
  timer.stop();

  if ( mDebug > 1 ) {
    DBG_OUT << "PHASE1: " << (timer.get_time() / 1000.0) << endl;
  }

  timer.reset();
  timer.start();
  vector<AssignList> cube_list;
  cube_list.push_back(suff_cond);
  if ( suff_cond.size() == 0 ) {
    // 十分条件と必要条件が等しかった．
    return TestCover{fault, mand_cond, cube_list};
  }

  // 制御用の変数を用意する．
  auto clit = mEngine.solver().new_variable(false);
  while ( cube_list.size() < mLimit ) {
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
    auto assumptions = mEngine.conv_to_literal_list(mand_cond);
    assumptions.push_back(plit);
    assumptions.push_back(clit);
    auto res = mEngine.solver().solve(assumptions);
    timer.stop();
    if ( mDebug > 2 ) {
      DBG_OUT << "  " << (timer.get_time() / 1000.0) << endl;
    }
    if ( res != SatBool3::True ) {
      // すべてのキューブを生成した．
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

  return TestCover{fault, mand_cond, cube_list};
}

// @brief TestCover に対応するBDDを返す．
Bdd
ExCubeGen::make_bdd(
  const TestCover& cover
)
{
  auto common_cube = make_bdd(cover.common_cube());
  Bdd cover_bdd = mBddMgr.zero();
  for ( auto cube: cover.cube_list() ) {
    auto cube_bdd = make_bdd(cube);
    cover_bdd |= cube_bdd;
  }
  auto ans = common_cube & cover_bdd;
  return ans;
}

// @brief AssignList に対応するBDDを返す．
Bdd
ExCubeGen::make_bdd(
  const AssignList& cube
)
{
  auto cube_bdd = mBddMgr.one();
  for ( auto nv: cube ) {
    auto lit_bdd = make_bdd(nv);
    cube_bdd &= lit_bdd;
  }
  return cube_bdd;
}

// @brief Assign に対応するBDDを返す．
Bdd
ExCubeGen::make_bdd(
  const Assign& assign
)
{
  auto sig = assign.node_time();
  Bdd bdd;
  if ( mBddMap.count(sig) > 0 ) {
    bdd= mBddMap.at(sig);
  }
  else {
    bdd = mBddMgr.posi_literal(mBddVarBase);
    ++ mBddVarBase;
    mBddMap.emplace(sig, bdd);
  }
  if ( !assign.val() ) {
    bdd = ~bdd;
  }
  return bdd;
}

END_NAMESPACE_DRUID
