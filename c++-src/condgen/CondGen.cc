
/// @file CondGen.cc
/// @brief CondGen の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "CondGen.h"
#include "types/TpgNetwork.h"
#include "types/TpgFFR.h"
#include "types/TpgNode.h"
#include "types/TpgFault.h"
#include "types/OpBase.h"
#include "dtpg/BdEngine.h"


#define DBG_OUT std::cerr

BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

int debug = 0;

// AssignList を std::vector<Literal> に変換する．
std::vector<Literal>
assign_to_literal(
  const AssignList& as_list
)
{
  std::vector<Literal> lits;
  lits.reserve(as_list.size());
  for ( auto as: as_list ) {
    auto node = as.node();
    auto time = as.time();
    auto val = as.val();
    auto id = node.id() * 2 + time;
    auto inv = val == 0;
    lits.push_back(Literal(id, inv));
  }
  return lits;
}

// CondData に変換する．
DetCond::CondData
to_cond(
  const AssignList& mand_cond,
  const std::vector<AssignList>& cube_list = {}
)
{
  auto mand_lits= assign_to_literal(mand_cond);
  std::vector<std::vector<Literal>> cov_lits;
  cov_lits.reserve(cube_list.size());
  for ( auto& cube: cube_list ) {
    auto cube_lits = assign_to_literal(cube);
    cov_lits.push_back(cube_lits);
  }
  auto data = DetCond::CondData{mand_lits, cov_lits};
  return data;
}

END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// クラス CondGen
//////////////////////////////////////////////////////////////////////

// @brief FFRの出力の故障伝搬条件を求める．
// @return 条件式を返す．
DetCond
CondGen::root_cond(
  const TpgNetwork& network,
  const TpgFFR& ffr,
  SizeType limit,
  const JsonValue& option
)
{
  Timer timer;
  timer.start();

  auto root = ffr.root();
  AssignList mand_cond;
  SizeType output_num;
  {
    BdEngine engine(network, root, option);
    engine.add_prev_node(root);
    output_num = engine.output_num();

    // FFR の出力の伝搬可能性を調べる．
    auto& solver = engine.solver();
    auto pvar = engine.prop_var();
    auto res = engine.solve({pvar});
    if ( res != SatBool3::True ) {
      // 検出可能ではなかった．
      return DetCond::undetected(ffr.id(), root);
    }

    // 最初の十分条件を求める．
    auto suff_cond = engine.extract_sufficient_condition();
    // 必要条件を求める．
    for ( auto as: suff_cond ) {
      auto lit = engine.conv_to_literal(as);
      if ( engine.solve({pvar, ~lit}) == SatBool3::False ) {
	mand_cond.add(as);
      }
    }
    suff_cond.diff(mand_cond);

    timer.stop();

    if ( debug > 1 ) {
      DBG_OUT << "PHASE1: " << (timer.get_time() / 1000.0)
	      << std::endl;
    }

    if ( suff_cond.size() == 0 ) {
      // 十分条件と必要条件が等しかった．
      return DetCond::detected(ffr.id(), root, to_cond(mand_cond));
    }

    std::vector<AssignList> cube_list;
    cube_list.push_back(suff_cond);

    bool found = false;

    // 別の十分条件を求める．
    timer.reset();
    timer.start();
    for ( SizeType loop_count = 1; loop_count < limit; ++ loop_count ) {
      Timer timer;
      timer.start();
      // suff_cond を否定した節を加える．
      std::vector<SatLiteral> tmp_lits;
      tmp_lits.reserve(suff_cond.size());
      for ( auto nv: suff_cond ) {
	auto lit = engine.conv_to_literal(nv);
	tmp_lits.push_back(~lit);
      }
      solver.add_clause(tmp_lits);
      auto tmp_assumptions = engine.conv_to_literal_list(mand_cond);
      tmp_assumptions.push_back(pvar);
      auto res = solver.solve(tmp_assumptions);
      timer.stop();
      if ( debug > 2 ) {
	DBG_OUT << "  " << (timer.get_time() / 1000.0)
		<< std::endl;
      }
      if ( res == SatBool3::False ) {
	// すべてのキューブを生成した．
	found = true;
	break;
      }
      if ( res == SatBool3::X ) {
	// 時間切れ．
	// どうする？
	break;
      }
      suff_cond = engine.extract_sufficient_condition();
      suff_cond.diff(mand_cond);
      if ( suff_cond.size() == 0 ) {
	// 最初に生成された suff_cond が冗長だった．
	// 結局 mand_cond が唯一の条件となる．
	cube_list.clear();
	cube_list.push_back(mand_cond);
	found = true;
	break;
      }
      cube_list.push_back(suff_cond);
    }
    timer.stop();

    if ( debug > 1 ) {
      DBG_OUT << "PHASE2: " << (timer.get_time() / 1000.0)
	      << std::endl;
    }

    // 生成された結果を論理式の形に変換する．
    if ( found ) {
      return DetCond::detected(ffr.id(), root, to_cond(mand_cond, cube_list));
    }
  }
  // 伝搬先の出力を一つに制限して同じ処理を繰り返す．
  TpgNodeList output_list;
  output_list.reserve(output_num);
  std::vector<DetCond::CondData> cond_list;
  for ( SizeType pos = 0; pos < output_num; ++ pos ) {
    BdEngine engine(network, root, option);
    engine.add_prev_node(root);
    auto output = engine.output(pos);
    auto& solver = engine.solver();
    auto pvar = engine.prop_var(pos);
    auto assumptions = engine.conv_to_literal_list(mand_cond);
    assumptions.push_back(pvar);
    auto res = solver.solve(assumptions);
    if ( res != SatBool3::True ) {
      // この出力では検出できなかった．
      continue;
    }
    // 最初の十分条件を求める．
    auto suff_cond = engine.extract_sufficient_condition(pos);
    suff_cond.diff(mand_cond);

    // 必要条件を求める．
    AssignList mand_cond1 = mand_cond;
    for ( auto as: suff_cond ) {
      auto lit = engine.conv_to_literal(as);
      auto assumptions1 = assumptions;
      assumptions1.push_back(~lit);
      if ( solver.solve(assumptions1) == SatBool3::False ) {
	mand_cond1.add(as);
      }
    }
    suff_cond.diff(mand_cond1);

    if ( suff_cond.size() == 0 ) {
      // 十分条件と必要条件が等しかった．
      cond_list.push_back(to_cond(mand_cond1));
      continue;
    }

    std::vector<AssignList> cube_list;
    cube_list.push_back(suff_cond);

    bool found = false;

    // 別の十分条件を求める．
    for ( SizeType loop_count = 1; loop_count < limit; ++ loop_count ) {
      // suff_cond を否定した節を加える．
      std::vector<SatLiteral> tmp_lits;
      tmp_lits.reserve(suff_cond.size());
      for ( auto as: suff_cond ) {
	auto lit = engine.conv_to_literal(as);
	tmp_lits.push_back(~lit);
      }
      solver.add_clause(tmp_lits);
      auto tmp_assumptions = engine.conv_to_literal_list(mand_cond1);
      tmp_assumptions.push_back(pvar);
      auto res = solver.solve(tmp_assumptions);
      if ( res == SatBool3::False ) {
	found = true;
	break;
      }
      if ( res == SatBool3::X ) {
	break;
      }
      suff_cond = engine.extract_sufficient_condition(pos);
      suff_cond.diff(mand_cond1);
      if ( suff_cond.size() == 0 ) {
	cube_list.clear();
	cube_list.push_back(mand_cond1);
	found = true;
	break;
      }
      cube_list.push_back(suff_cond);
    }
    if ( found ) {
      cond_list.push_back(to_cond(mand_cond1, cube_list));
    }
    else {
      output_list.push_back(output);
    }
  }
  if ( cond_list.empty() ) {
    return DetCond::overflow(ffr.id(), root, output_list);
  }
  else {
    return DetCond::partial_detected(ffr.id(), root, cond_list, output_list);
  }
}

END_NAMESPACE_DRUID
