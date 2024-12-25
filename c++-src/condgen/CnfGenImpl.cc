
/// @file CnfGenImpl.cc
/// @brief CnfGenImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "CnfGenImpl.h"
//#include "TpgNetwork.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス CnfGenImpl
//////////////////////////////////////////////////////////////////////

// @brief 式を CNF に変換する．
vector<SatLiteral>
CnfGenImpl::make_cnf(
  const DetCond& cond
)
{
  // 必要条件はそのまま assumptions に入れる．
  auto assumptions = cube_to_literals(cond.mandatory_condition());

  // 残りは継承クラスの仮想関数に任せる．
  auto lit = cover_to_cnf(cond.cube_list());
  assumptions.push_back(lit);

  return assumptions;
}

// @brief 条件を CNF に変換した際の項数とリテラル数を計算する．
CnfSize
CnfGenImpl::calc_cnf_size(
  const DetCond& cond
)
{
  // 必要条件は項を生成しない．

  // 継承クラスの仮想関数に任せる．
  return calc_cover_size(cond.cube_list());
}

// @brief キューブを表すリテラルのリストを返す．
vector<SatLiteral>
CnfGenImpl::cube_to_literals(
  const AssignList& cube
)
{
  auto n = cube.size();
  vector<SatLiteral> lits;
  lits.reserve(n);
  for ( auto as: cube ) {
    auto lit = mEngine.conv_to_literal(as);
    lits.push_back(lit);
  }
  return lits;
}

END_NAMESPACE_DRUID
