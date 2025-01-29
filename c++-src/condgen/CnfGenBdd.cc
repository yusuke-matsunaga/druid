
/// @file CnfGenBdd.cc
/// @brief CnfGenBdd の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "CnfGenBdd.h"
#include "BddHeap.h"
#include "Bdd2Cnf.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス CnfGenBdd
//////////////////////////////////////////////////////////////////////

// @brief 条件を CNF に変換する．
vector<vector<SatLiteral>>
CnfGenBdd::make_cnf(
  StructEngine& engine,
  const vector<DetCond>& cond_list
)
{
  vector<vector<SatLiteral>> ans_list;
  ans_list.reserve(cond_list.size());
  for ( auto& cond: cond_list ) {
    vector<SatLiteral> assumptions;
    assumptions.reserve(cond.mandatory_condition().size() + 1);
    for ( auto as: cond.mandatory_condition() ) {
      auto lit = engine.conv_to_literal(as);
      assumptions.push_back(lit);
    }
    auto lit = cover_to_cnf(engine, cond.cube_list());
    assumptions.push_back(lit);
    ans_list.push_back(assumptions);
  }
  return ans_list;
}

// @brief カバーをCNFに変換する．
SatLiteral
CnfGenBdd::cover_to_cnf(
  StructEngine& engine,
  const vector<AssignList>& cube_list
)
{
  Bdd2Cnf conv{engine};

  auto bdd_list = cover_to_bdd(cube_list);

  auto n = bdd_list.size();
  if ( n == 0 ) {
    abort();
  }
  if ( n == 1 ) {
    return conv.conv_to_cnf(bdd_list.front());
  }
  vector<SatLiteral> tmp_lits;
  tmp_lits.reserve(n + 1);
  auto lit = engine.solver().new_variable(false);
  tmp_lits.push_back(~lit);
  for ( auto& bdd1: bdd_list ) {
    auto lit1 = conv.conv_to_cnf(bdd1);
    tmp_lits.push_back(lit);
  }
  engine.solver().add_clause(tmp_lits);
  return lit;
}

// @brief カバーをCNFに変換した時の CNF のサイズを見積もる．
CnfSize
CnfGenBdd::calc_cnf_size(
  const vector<DetCond>& cond_list
)
{
  auto size = CnfSize::zero();
  for ( auto& cond: cond_list ) {
    auto& cube_list = cond.cube_list();
    if ( cube_list.empty() ) {
      continue;
    }
    auto bdd_list = cover_to_bdd(cube_list);

    auto n = bdd_list.size();
    if ( n == 0 ) {
      cout << endl;
      for ( auto& cube: cube_list ) {
	cout << cube << endl;
      }
      abort();
    }
    auto size1 = CnfSize::zero();
    if ( n == 1 ) {
      size1 = Bdd2Cnf::calc_cnf_size(bdd_list.front());
    }
    else {
      for ( auto& bdd1: bdd_list ) {
	auto tmp_size = Bdd2Cnf::calc_cnf_size(bdd1);
	size1 += tmp_size;
      }
      size1 += CnfSize{1, n + 1};
    }

    auto size0 = CnfSize::zero();
    for ( auto& cube: cube_list ) {
      auto n = cube.size();
      // 1つのキューブにつき
      // n 項， n * 2 リテラル
      size0 += CnfSize{n, n * 2};
    }
    // 最後にキューブ数+1の項を追加
    size0 += CnfSize{1, cube_list.size() + 1};

    if ( size0.literal_num < size1.literal_num ) {
      size += size0;
    }
    else {
      size += size1;
    }
  }

  return size;
}

// @brief カバー（キューブのリスト）をBDDのリストに変換する．
vector<Bdd>
CnfGenBdd::cover_to_bdd(
  const vector<AssignList>& cube_list
)
{
  // 個々のキューブをBddに変換してヒープ木に入れる．
  BddHeap bdd_heap;
  for ( auto& cube: cube_list ) {
    auto bdd1 = cube_to_bdd(cube);
    bdd_heap.put(bdd1);
  }
  // サイズオーバーとなった Bdd を入れておくリスト
  vector<Bdd> bdd_list;
  bdd_list.reserve(bdd_heap.size());
  // ヒープ木からサイズの小さい順に取り出してマージする．
  while ( bdd_heap.size() > 1 ) {
    auto bdd1 = bdd_heap.get_min();
    auto bdd2 = bdd_heap.get_min();
    auto bdd3 = bdd1 | bdd2;
    if ( bdd3.size() > mSizeLimit ) {
      // サイズが上限を超えたら bdd_list に退避させる．
      bdd_list.push_back(bdd3);
    }
    else {
      bdd_heap.put(bdd3);
    }
  }
  if ( bdd_heap.size() == 1 ) {
    // 最後に残った Bdd を bdd_list に入れる．
    auto bdd1 = bdd_heap.get_min();
    bdd_list.push_back(bdd1);
  }
  return bdd_list;
}

// @brief キューブを BDD に変換する．
Bdd
CnfGenBdd::cube_to_bdd(
  const AssignList& cube
)
{
  auto ans = mBddMgr.one();
  for ( auto as: cube ) {
    auto node = as.node();
    auto time = as.time();
    auto varid = node->id() * 2 + time;
    auto bdd1 = static_cast<Bdd>(mBddMgr.variable(varid));
    if ( !as.val() ) {
      bdd1 = ~bdd1;
    }
    ans &= bdd1;
  }
  return ans;
}

END_NAMESPACE_DRUID
