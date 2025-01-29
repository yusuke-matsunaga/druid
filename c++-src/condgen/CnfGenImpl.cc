
/// @file CnfGenImpl.cc
/// @brief CnfGenImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "CnfGenImpl.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス CnfGenImpl
//////////////////////////////////////////////////////////////////////

// @brief 式を CNF に変換する．
vector<vector<SatLiteral>>
CnfGenImpl::make_cnf(
  StructEngine& engine,
  const vector<DetCond>& cond_list
)
{
  vector<SatLiteral> assumptions;
  assumptions.reserve(cond.mandatory_condition().size() + 1);

  // 必要条件はそのまま assumptions に入れる．
  for ( auto& as: cond.mandatory_condition() ) {
    auto lit = mEngine.conv_to_literal(as);
    assumptions.push_back(lit);
  }

  // 残りは継承クラスの仮想関数に任せる．
  auto lit = cover_to_cnf(cond.cube_list());
  assumptions.push_back(lit);

  return assumptions;
}

// @brief キューブを表すリテラルを返す．
SatLiteral
CnfGenImpl::cube_to_cnf(
  StructEngine& engine,
  const AssignList& cube
)

END_NAMESPACE_DRUID
