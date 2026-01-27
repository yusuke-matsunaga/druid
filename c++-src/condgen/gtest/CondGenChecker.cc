
/// @file CondGenChecker.cc
/// @brief CondGenChecker の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "CondGenChecker.h"
#include "types/TpgNetwork.h"
#include "types/TpgFFR.h"
#include "dtpg/BdEngine.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

END_NONAMESPACE

// @brief コンストラクタ
CondGenChecker::CondGenChecker(
  const TpgNetwork& network,
  const DetCond& cond
) : mEngine(network, cond.root()),
    mCond{cond}
{
  mEngine.add_prev_node(cond.root());
  // cond の内容を表すCNFを作る．
  // ただし，cond が成り立たない場合も調べたいので両方の implication
  // を作る．
  mLit1 = make_cnf();
  mLit2 = mEngine.prop_var();
}

SatLiteral
CondGenChecker::conv_to_literal(
  Literal src_lit
)
{
  auto varid = src_lit.varid();
  auto node_id = varid / 2;
  auto time = varid % 2;
  auto node = mEngine.network().node(node_id);
  auto as = Assign(node, time, src_lit.is_positive());
  auto lit = mEngine.conv_to_literal(as);
  return lit;
}

std::vector<SatLiteral>
CondGenChecker::conv_to_literals(
  const std::vector<Literal>& src_lits
)
{
  std::vector<SatLiteral> lits;
  lits.reserve(src_lits.size());
  for ( auto src_lit: src_lits ) {
    auto lit = conv_to_literal(src_lit);
    lits.push_back(lit);
  }
  return lits;
}

SatLiteral
CondGenChecker::make_cnf(
  const DetCond::CondData& data
)
{
  auto tmp_lits = conv_to_literals(data.mand_cond);
  if ( !data.cube_list.empty() ) {
    std::vector<SatLiteral> tmp_lits2;
    tmp_lits2.reserve(data.cube_list.size());
    for ( auto& cube: data.cube_list ) {
      auto cube_lits = conv_to_literals(cube);
      auto and_lit = mEngine.solver().new_variable(false);
      mEngine.solver().add_andgate(and_lit, cube_lits);
      tmp_lits2.push_back(and_lit);
    }
    auto or_lit = mEngine.solver().new_variable(false);
    mEngine.solver().add_orgate(or_lit, tmp_lits2);
    tmp_lits.push_back(or_lit);
  }
  if ( tmp_lits.empty() ) {
    return SatLiteral::X;
  }
  auto olit = mEngine.solver().new_variable(false);
  mEngine.solver().add_andgate(olit, tmp_lits);
  return olit;
}

SatLiteral
CondGenChecker::make_cnf()
{
  if ( mCond.type() == DetCond::Detected ) {
    auto lit = make_cnf(mCond.cond());
    return lit;
  }
  if ( mCond.type() == DetCond::PartialDetected ) {
    std::vector<SatLiteral> tmp_lits;
    tmp_lits.reserve(mCond.cond_list().size());
    for ( auto& data: mCond.cond_list() ) {
      auto lit2 = make_cnf(data);
      tmp_lits.push_back(lit2);
    }
    auto olit = mEngine.solver().new_variable(false);
    mEngine.solver().add_orgate(olit, tmp_lits);
    auto lit1 = make_cnf(mCond.cond());
    if ( lit1 == SatLiteral::X ) {
      auto tmp_lit = olit;
      olit = mEngine.solver().new_variable(false);
      mEngine.solver().add_andgate(olit, tmp_lit, lit1);
    }
    if ( mCond.output_list().empty() ) {
      if ( olit == SatLiteral::X ) {
	std::cout << "olit == SatLiteral::X"
		  << std::endl;
	mCond.print(std::cout);
	abort();
      }
      return olit;
    }
    auto bd_enc = new BoolDiffEnc(mCond.root(), mCond.output_list());
    mEngine.add_subenc(std::unique_ptr<SubEnc>{bd_enc});
    mEngine.add_prev_node(mCond.root());
    auto tmp_lit = olit;
    olit = mEngine.new_variable(false);
    mEngine.solver().add_orgate(olit, tmp_lit, bd_enc->prop_var());
    if ( olit == SatLiteral::X ) {
      std::cout << "olit == SatLiteral::X"
		<< std::endl;
      mCond.print(std::cout);
      abort();
    }
    return olit;
  }
  if ( mCond.type() == DetCond::Overflow ) {
    auto bd_enc = new BoolDiffEnc(mCond.root(), mCond.output_list());
    //auto bd_enc = new BoolDiffEnc(mCond.root());
    mEngine.add_subenc(std::unique_ptr<SubEnc>{bd_enc});
    mEngine.add_prev_node(mCond.root());
    auto lit = bd_enc->prop_var();
    return lit;
  }
  throw std::logic_error{"cond.type() == Undetected"};
  return SatLiteral::X;
}

END_NAMESPACE_DRUID
