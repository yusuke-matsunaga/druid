
/// @file Cover2Bdd.cc
/// @brief Cover2Bdd の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "Cover2Bdd.h"
#include "OpBase.h"


#define DBG_OUT cerr

BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
Cover2Bdd::Cover2Bdd(
  const JsonValue& option
) : mDebug{OpBase::get_debug(option)}
{
}

BEGIN_NONAMESPACE

// @brief AssignList に対応するBDDを返す．
Bdd
cube_to_bdd(
  BddMgr& mgr,
  const std::unordered_map<PtrIntType, BddVar>& varmap,
  const AssignList& cube
)
{
  auto bdd = mgr.one();
  for ( auto assign: cube ) {
    auto sig = assign.node_time();
    auto var = varmap.at(sig);
    if ( assign.val() ) {
      bdd &= var;
    }
    else {
      bdd &= ~var;
    }
  }
  return bdd;
}

Bdd
cover_to_bdd(
  BddMgr& mgr,
  const std::unordered_map<PtrIntType, BddVar>& varmap,
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
Cover2Bdd::make_bdd(
  BddMgr& mgr,
  const TestCover& cover
)
{
  // common_cube は最上位の変数となる．
  std::unordered_map<PtrIntType, BddVar> varmap;
  SizeType varbase = 0;
  for ( auto assign: cover.common_cube() ) {
    auto sig = assign.node_time();
    auto var = mgr.variable(varbase);
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
	auto var = mgr.variable(varbase);
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
