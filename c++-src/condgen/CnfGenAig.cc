
/// @file CnfGenAig.cc
/// @brief CnfGenAig の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "CnfGenAig.h"
#include "ym/SopCover.h"
#include "ym/Expr.h"
#include "ym/AigMgr.h"
#include "TpgNetwork.h"
#include "Expr2Aig.h"



BEGIN_NAMESPACE_DRUID

// @brief Expr のリストから CNF を作る．
vector<vector<SatLiteral>>
CnfGenAig::expr_to_cnf(
  StructEngine& engine,
  const vector<Expr>& expr_list
)
{
  AigMgr mgr;
  Expr2Aig expr2aig(mgr);
  auto aig_list = expr2aig.conv_to_aig(expr_list);

  // AigHandle をキーにして SatLiteral を記憶する辞書
  std::unordered_map<AigHandle, vector<SatLiteral>> aig_map;

  vector<vector<SatLiteral>> lits_list;
  lits_list.reserve(expr_list.size());
  for ( auto& aig: aig_list ) {
    auto lits = aig_to_cnf(engine, aig, aig_map);
    lits_list.push_back(lits);
  }
  return lits_list;
}

// @brief Expr のリストから CNF サイズを見積もる．
CnfSize
CnfGenAig::expr_cnf_size(
  const vector<Expr>& expr_list
)
{
  AigMgr mgr;
  Expr2Aig expr2aig(mgr);
  auto aig_list = expr2aig.conv_to_aig(expr_list);

  std::unordered_map<AigHandle, SizeType> aig_map;

  auto size = CnfSize::zero();
  for ( auto& aig: aig_list ) {
    aig_cnf_size(aig, size, aig_map);
  }
  return size;
}

// @brief AigHandle に対応する CNF を作る．
vector<SatLiteral>
CnfGenAig::aig_to_cnf(
  StructEngine& engine,
  const AigHandle& aig,
  std::unordered_map<AigHandle, vector<SatLiteral>>& aig_map
)
{
  if ( aig.is_zero() ) {
    throw std::logic_error{"aig is zero"};
  }
  if ( aig.is_one() ) {
    return {};
  }
  if ( aig.is_input() ) {
    auto input_id = aig.input_id();
    auto node_id = input_id / 2;
    auto time = node_id % 2;
    auto node = engine.network().node(node_id);
    auto as = Assign(node, time, true);
    auto lit = engine.conv_to_literal(as);
    return {lit};
  }

  if ( aig_map.count(aig) > 0 ) {
    auto lits = aig_map.at(aig);
    return lits;
  }
  auto iaig = ~aig;
  if ( aig_map.count(iaig) > 0 ) {
    auto ilits = aig_map.at(iaig);
    auto lit = invert(engine, ilits);
    auto lits = vector<SatLiteral>{lit};
    aig_map.emplace(aig, lits);
    return lits;
  }
  // aig.is_and()
  auto h0 = aig.fanin0();
  auto h1 = aig.fanin1();
  auto lits0 = aig_to_cnf(engine, h0, aig_map);
  auto lits1 = aig_to_cnf(engine, h1, aig_map);
  vector<SatLiteral> lits;
  lits.reserve(lits0.size() + lits1.size());
  for ( auto lit: lits0 ) {
    lits.push_back(lit);
  }
  for ( auto lit: lits1 ) {
    lits.push_back(lit);
  }
  auto paig = aig.positive_handle();
  aig_map.emplace(paig, lits);

  if ( aig == paig ) {
    return lits;
  }

  auto ilit = invert(engine, lits);
  auto ilits = vector<SatLiteral>{ilit};
  aig_map.emplace(aig, ilits);
  return ilits;
}

// @brief 反転したCNFを作る．
SatLiteral
CnfGenAig::invert(
  StructEngine& engine,
  const vector<SatLiteral>& lits
)
{
  auto lit = engine.solver().new_variable(false);
  vector<SatLiteral> tmp_lits;
  tmp_lits.reserve(lits.size() + 1);
  tmp_lits.push_back(~lit);
  for ( auto lit: lits ) {
    tmp_lits.push_back(~lit);
  }
  engine.solver().add_clause(tmp_lits);
  return lit;
}

// @brief AigHandle に対応する CNF のサイズを見積もる．
SizeType
CnfGenAig::aig_cnf_size(
  const AigHandle& aig,
  CnfSize& size,
  std::unordered_map<AigHandle, SizeType>& aig_map
)
{
  if ( aig.is_const() ) {
    return 0;
  }
  if ( aig.is_input() ) {
    return 1;
  }
  if ( aig_map.count(aig) > 0 ) {
    return aig_map.at(aig);
  }
  auto iaig = ~aig;
  if ( aig_map.count(iaig) > 0 ) {
    auto n = aig_map.at(iaig);
    size += CnfSize{1, n + 1};
    aig_map.emplace(aig, 1);
    return 1;
  }
  auto h0 = aig.fanin0();
  auto h1 = aig.fanin1();
  auto n0 = aig_cnf_size(h0, size, aig_map);
  auto n1 = aig_cnf_size(h1, size, aig_map);
  auto n = n0 + n1;
  auto paig = aig.positive_handle();
  aig_map.emplace(paig, n);

  if ( aig == paig ) {
    return n;
  }

  size += CnfSize{1, n + 1};
  aig_map.emplace(aig, 1);
  return 1;
}

END_NAMESPACE_DRUID
