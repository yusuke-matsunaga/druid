
/// @file Bdd2Cnf.cc
/// @brief Bdd2Cnf の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "Bdd2Cnf.h"
#include "TpgNetwork.h"
#include "ym/BddVar.h"


BEGIN_NAMESPACE_DRUID

// @brief BDD を CNF に変換する．
SatLiteral
Bdd2Cnf::conv_to_cnf(
  const Bdd& bdd
)
{
  if ( mResultDict.count(bdd) > 0 ) {
    return mResultDict.at(bdd);
  }

  SatLiteral lit;
  Bdd bdd0;
  Bdd bdd1;
  auto root_var = bdd.root_decomp(bdd0, bdd1);
  auto root_lit = conv_to_literal(root_var);
  if ( bdd0.is_zero() ) {
    if ( bdd1.is_one() ) {
      lit = root_lit;
    }
    else {
      lit = mEngine.solver().new_variable(false);
      auto lit1 = conv_to_cnf(bdd1);
      mEngine.solver().add_clause(~lit, root_lit);
      mEngine.solver().add_clause(~lit, lit1);
    }
  }
  else if ( bdd0.is_one() ) {
    if ( bdd1.is_zero() ) {
      lit = ~root_lit;
    }
    else {
      lit = mEngine.solver().new_variable(false);
      auto lit1 = conv_to_cnf(bdd1);
      mEngine.solver().add_clause(~lit, ~root_lit, lit1);
    }
  }
  else {
    if ( bdd1.is_zero() ) {
      // bdd0 は定数ではない．
      lit = mEngine.solver().new_variable(false);
      auto lit0 = conv_to_cnf(bdd0);
      mEngine.solver().add_clause(~lit, ~root_lit);
      mEngine.solver().add_clause(~lit, lit0);
    }
    else if ( bdd1.is_one() ) {
      lit = mEngine.solver().new_variable(false);
      auto lit0 = conv_to_cnf(bdd0);
      mEngine.solver().add_clause(~lit, root_lit, lit0);
    }
    else {
      lit = mEngine.solver().new_variable(false);
      auto lit0 = conv_to_cnf(bdd0);
      auto lit1 = conv_to_cnf(bdd1);
      mEngine.solver().add_clause(~lit,  root_lit, lit0);
      mEngine.solver().add_clause(~lit, ~root_lit, lit1);
    }
  }
  mResultDict.emplace(bdd, lit);
  return lit;
}

BEGIN_NONAMESPACE

void
dfs(
  const Bdd& bdd,
  vector<Bdd>& node_list,
  std::unordered_set<Bdd>& mark
)
{
  if ( bdd.is_zero() || bdd.is_one() ) {
    return;
  }
  if ( mark.count(bdd) > 0 ) {
    return;
  }
  mark.emplace(bdd);
  node_list.push_back(bdd);

  Bdd bdd0;
  Bdd bdd1;
  auto root_var = bdd.root_decomp(bdd0, bdd1);
  dfs(bdd0, node_list, mark);
  dfs(bdd1, node_list, mark);
}

END_NONAMESPACE

// @brief BDD を CNF に変換した時のサイズを見積もる．
CnfSize
Bdd2Cnf::calc_cnf_size(
  const Bdd& bdd
)
{
  std::unordered_set<Bdd> mark;
  vector<Bdd> node_list;
  dfs(bdd, node_list, mark);

  auto size = CnfSize::zero();
  for ( auto& bdd: node_list ) {
    Bdd bdd0;
    Bdd bdd1;
    auto root_var = bdd.root_decomp(bdd0, bdd1);

    if ( bdd0.is_zero() ) {
      if ( bdd1.is_one() ) {
	;
      }
      else {
	size += CnfSize{2, 2};
      }
    }
    else if ( bdd0.is_one() ) {
      if ( bdd1.is_zero() ) {
	;
      }
      else {
	size += CnfSize{1, 3};
      }
    }
    else if ( bdd1.is_zero() ) {
      size += CnfSize{2, 2};
    }
    else if ( bdd1.is_one() ) {
      size += CnfSize{1, 3};
    }
    else {
      size += CnfSize{2, 6};
    }
  }

  return size;
}

// @brief 論理式のリテラルを SAT ソルバのリテラルに変換する．
SatLiteral
Bdd2Cnf::conv_to_literal(
  const BddVar& var
) const
{
  // 回りくどい変換を行う．
  auto varid = var.id();
  auto node_id = varid / 2;
  auto node = mEngine.network().node(node_id);
  auto time = static_cast<int>(varid % 2);
  auto as = Assign{node, time, true};
  return mEngine.conv_to_literal(as);
}

END_NAMESPACE_DRUID
