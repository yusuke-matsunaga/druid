
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

BEGIN_NONAMESPACE

// @brief AssignList に対応するBDDを返す．
Bdd
cube_to_bdd(
  BddMgr& mgr,
  const std::unordered_map<PtrIntType, SizeType>& varmap,
  const AssignList& cube
)
{
  auto bdd = mgr.one();
  for ( auto assign: cube ) {
    auto sig = assign.node_time();
    auto var = varmap.at(sig);
    if ( assign.val() ) {
      bdd &= mgr.posi_literal(var);
    }
    else {
      bdd &= mgr.nega_literal(var);
    }
  }
  return bdd;
}

Bdd
cover_to_bdd(
  BddMgr& mgr,
  const std::unordered_map<PtrIntType, SizeType>& varmap,
  const vector<PtrIntType>& sig_list,
  SizeType pos,
  const vector<AssignList>& cube_list
)
{
  auto bdd = mgr.zero();
  for ( auto& cube: cube_list ) {
    auto cube_bdd = cube_to_bdd(mgr, varmap, cube);
    bdd |= cube_bdd;
  }
  return bdd;
#if 0
#if 1
  {
    cout << "cover_to_bdd(" << cube_list.size() << "),"
	 << " pos = " << pos << endl;
  }
#endif
  if ( cube_list.empty() ) {
    return mgr.zero();
  }
  if ( cube_list.size() == 1 ) {
    return cube_to_bdd(mgr, varmap, cube_list[0]);
  }
  auto sig = sig_list[pos];
  vector<AssignList> cube0_list;
  vector<AssignList> cube1_list;
  cube0_list.reserve(cube_list.size());
  cube1_list.reserve(cube_list.size());
  bool cube0_one = false;
  bool cube1_one = false;
  for ( auto& cube: cube_list ) {
    bool found = false;
    for ( auto assign: cube ) {
      auto sig1 = assign.node_time();
      if ( sig1 == sig ) {
	auto cube1 = cube - assign;
	if ( assign.val() ) {
	  if ( cube1.size() == 0 ) {
	    cube1_one = true;
	  }
	  else {
	    cube1_list.push_back(cube1);
	  }
	}
	else {
	  if ( cube1.size() == 0 ) {
	    cube0_one = true;
	  }
	  else {
	    cube0_list.push_back(cube1);
	  }
	}
	found = true;
	break;
      }
    }
    if ( !found ) {
      cube0_list.push_back(cube);
      cube1_list.push_back(cube);
    }
  }
  Bdd bdd0;
  if ( cube0_one ) {
    bdd0 = mgr.one();
  }
  else {
    bdd0 = cover_to_bdd(mgr, varmap, sig_list, pos + 1, cube0_list);
  }
  Bdd bdd1;
  if ( cube1_one ) {
    bdd1 = mgr.one();
  }
  else {
    bdd1 = cover_to_bdd(mgr, varmap, sig_list, pos + 1, cube1_list);
  }
  auto var = varmap.at(sig);
  auto lit = mgr.posi_literal(var);
  auto bdd = mgr.ite(lit, bdd1, bdd0);
  return bdd;
#endif
}

END_NONAMESPACE

// @brief TestCover に対応するBDDを返す．
Bdd
ExCubeGen::make_bdd(
  BddMgr& mgr,
  const TestCover& cover
)
{
  // common_cube は最上位の変数となる．
  std::unordered_map<PtrIntType, SizeType> varmap;
  SizeType varbase = 0;
  for ( auto assign: cover.common_cube() ) {
    auto sig = assign.node_time();
    auto var = varbase;
    ++ varbase;
    varmap.emplace(sig, var);
  }
  auto common_cube = cube_to_bdd(mgr, varmap, cover.common_cube());
  // 残りは most binate variable の順に選ぶ．
#if 0
  vector<PtrIntType> sig_list;
  std::unordered_map<PtrIntType, std::pair<SizeType, SizeType>> sig_map;
  for ( auto cube: cover.cube_list() ) {
    for ( auto assign: cube ) {
      auto sig = assign.node_time();
      if ( sig_map.count(sig) == 0 ) {
	sig_list.push_back(sig);
	sig_map.emplace(sig, std::pair<SizeType, SizeType>{0, 0});
      }
    }
  }
  // 各変数ごとに肯定のキューブ数と否定のキューブ数の
  // 最小値の降順に並べる．
  for ( auto cube: cover.cube_list() ) {
    for ( auto assign: cube ) {
      auto sig = assign.node_time();
      auto& p = sig_map.at(sig);
      if ( assign.val() ) {
	++ p.second;
      }
      else {
	++ p.first;
      }
    }
  }
  sort(sig_list.begin(), sig_list.end(),
       [&](PtrIntType a,
	   PtrIntType b)
       {
	 auto& pa = sig_map.at(a);
	 auto& pb = sig_map.at(b);
	 return std::min(pa.first, pa.second) > std::min(pb.first, pb.second);
       });
  for ( auto sig: sig_list ) {
    auto var = varbase;
    ++ varbase;
    varmap.emplace(sig, var);
  }
#else
  std::unordered_set<PtrIntType> sig_set;
  for ( auto cube: cover.cube_list() ) {
    for ( auto assign: cube ) {
      auto sig = assign.node_time();
      if ( sig_set.count(sig) == 0 ) {
	sig_set.emplace(sig);
	auto var = varbase;
	++ varbase;
	varmap.emplace(sig, var);
      }
    }
  }
#endif
  vector<PtrIntType> sig_list;
  auto cover_bdd = cover_to_bdd(mgr, varmap, sig_list, 0, cover.cube_list());
  if ( cover_bdd.size() == 0 ) {
    cout << "cover_bdd.size() == 0" << endl;
    for ( auto cube: cover.cube_list() ) {
      cout << cube << endl;
    }
    cout << endl;
  }
  auto ans = common_cube & cover_bdd;
  return ans;
}

END_NAMESPACE_DRUID
