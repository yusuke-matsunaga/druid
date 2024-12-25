
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

// @brief カバーをCNFに変換する．
SatLiteral
CnfGenBdd::cover_to_cnf(
  const vector<AssignList>& cube_list
)
{
  auto bdd_list = cover_to_bdd(cube_list);

  Bdd2Cnf conv{engine()};
  auto n = bdd_list.size();
  if ( n == 0 ) {
    abort();
  }
  if ( n == 1 ) {
    return conv.conv_to_cnf(bdd_list.front());
  }
  vector<SatLiteral> tmp_lits;
  tmp_lits.reserve(n + 1);
  auto lit = solver().new_variable(false);
  tmp_lits.push_back(~lit);
  for ( auto& bdd1: bdd_list ) {
    auto lit1 = conv.conv_to_cnf(bdd1);
    tmp_lits.push_back(lit);
  }
  solver().add_clause(tmp_lits);
  return lit;
}

// @brief カバーをCNFに変換した時の CNF のサイズを見積もる．
CnfSize
CnfGenBdd::calc_cover_size(
  const vector<AssignList>& cube_list
)
{
  auto bdd_list = cover_to_bdd(cube_list);

  Bdd2Cnf conv{engine()};
  auto n = bdd_list.size();
  if ( n == 0 ) {
    abort();
  }
  if ( n == 1 ) {
    return conv.calc_cnf_size(bdd_list.front());
  }
  auto size = CnfSize::zero();
  for ( auto& bdd1: bdd_list ) {
    auto size1 = conv.calc_cnf_size(bdd1);
    size += size1;
  }
  size += CnfSize{1, n + 1};
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
