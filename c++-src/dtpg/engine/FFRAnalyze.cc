
/// @file FFRAnalyze.cc
/// @brief FFRAnalyze の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "FFRAnalyze.h"
#include "types/TpgFaultList.h"
#include "dtpg/BdEngine.h"


BEGIN_NAMESPACE_DRUID

// @brief FFR内の故障の支配関係を調べる．
void
FFRAnalyze::run(
  const TpgFFR& ffr,
  const TpgFaultList& fault_list,
  FaultInfo& fault_info,
  const ConfigParam& option
)
{
  if ( fault_list.empty() ) {
    return;
  }

  auto ffr_reduction = option.get_bool_elem("ffr_reduction", true);
  auto debug = option.get_bool_elem("debug", false);

  if ( debug ) {
    std::cout << "FFRAnalyze(FFR#" << ffr.id() << ")" << std::endl
	      << " fault_list:";
    for ( auto fault: fault_list ) {
      std::cout << " " << fault.str();
    }
    std::cout << std::endl;
  }

  // fault_list の FFR に対する故障伝搬条件を表すCNF式を持つSATソルバを作る．
  BdEngine engine(ffr.root(), option);
  auto pvar = engine.prop_var();
  engine.solver().add_clause(pvar);

  // 個々の故障の検出状況を調べる．
  TpgFaultList det_list;
  std::vector<std::vector<SatLiteral>> dlits_array;
  dlits_array.reserve(fault_list.size());
  for ( auto fault: fault_list ) {
    auto cond = fault.ffr_propagate_condition();
    auto lits = engine.conv_to_literal_list(cond);
    auto res = engine.solver().solve(lits);
    if ( res == SatBool3::True ) {
      if ( debug ) {
	std::cout << fault.str() << " is detected" << std::endl;
      }
      auto model = engine.solver().model();
      auto suff_cond = engine.extract_sufficient_condition(model);
      auto det_cond = suff_cond.main_cond();
      det_cond.merge(fault.ffr_propagate_condition());
      auto aux_cond = suff_cond.aux_cond();
      for ( auto node: fault.ffr_aux_side_inputs() ) {
	aux_cond.add(engine.assign(node, 1, model));
      }
      auto suff_cond1 = SuffCond(det_cond, aux_cond);
      auto pi_assign = engine.justify(suff_cond1, model);
      auto tv = TestVector(pi_assign);
      fault_info.set_detected(fault, det_cond, tv);
      det_list.push_back(fault);
      dlits_array.push_back(lits);
    }
    else if ( res == SatBool3::False ) {
      fault_info.set_untestable(fault);
    }
    else { // res == SatBool3::X ) {
      ;
    }
  }

  // 各故障が検出できない条件を表すリテラルを作る．
  // ただし，もともと検出できなかった故障はスキップする．
  auto ndet = det_list.size();
  std::vector<SatLiteral> clit_array(ndet, SatLiteral::X);
  for ( SizeType i = 0; i < ndet; ++ i ) {
    auto fault = det_list[i];
    auto dlits = dlits_array[i];
    auto clit = engine.solver().new_variable(true);
    clit_array[i] = clit;
    std::vector<SatLiteral> tmp_lits;
    tmp_lits.reserve(dlits.size() + 1);
    tmp_lits.push_back(~clit);
    for ( auto lit: dlits ) {
      tmp_lits.push_back(~lit);
    }
    engine.solver().add_clause(tmp_lits);
  }

  // ２つの故障の両立性を調べる．
  // 両立していた場合は支配関係も調べる．
  for ( SizeType i1 = 0; i1 < ndet - 1; ++ i1 ) {
    auto fault1 = det_list[i1];
    if ( fault_info.is_dominated(fault1) ) {
      continue;
    }
    if ( debug ) {
      std::cout << "fault1 = " << fault1.str() << std::endl;
    }
    auto& dlits1 = dlits_array[i1];
    for ( SizeType i2 = i1 + 1; i2 < ndet; ++ i2 ) {
      auto fault2 = det_list[i2];
      if ( fault_info.is_dominated(fault2) ) {
	continue;
      }
      if ( debug ) {
	std::cout << "  fault2 = " << fault2.str() << std::endl;
      }
      auto dlits2 = dlits_array[i2];
      auto tmp_lits = concat_lits(dlits1, dlits2);
      auto res = engine.solver().solve(tmp_lits);
      if ( res != SatBool3::True ) {
	continue;
      }
      fault_info.set_compatible(fault1, fault2);
      if ( ffr_reduction ) {
	// fault1 を検出して fault2 を検出しない条件を調べる．
	auto tmp_lits1 = dlits1;
	tmp_lits1.push_back(clit_array[i2]);
	auto res1 = engine.solver().solve(tmp_lits1);
	if ( res1 == SatBool3::False ) {
	  if ( debug ) {
	    std::cout << "    " << fault2.str()
		      << " is dominated by "
		      << fault1.str()
		      << std::endl;
	  }
	  // fault2 は支配されている．
	  fault_info.set_dominated(fault2);
	  // fault2 は削除されたので逆は調べない．
	  continue;
	}
	// fault2 を検出して fault1 を検出しない条件を調べる．
	auto tmp_lits2 = dlits2;
	tmp_lits2.push_back(clit_array[i1]);
	auto res2 = engine.solver().solve(tmp_lits2);
	if ( res2 == SatBool3::False ) {
	  if ( debug ) {
	    std::cout << "    " << fault1.str()
		      << " is dominated by "
		      << fault2.str()
		      << std::endl;
	  }
	  // fault1 は支配されている．
	  fault_info.set_dominated(fault1);
	  // fault1 は削除されたので残りは調べない．
	  break;
	}
      }
    }
  }
}

// @brief 代表故障に対して必須条件を求める．
void
FFRAnalyze::get_mandatory_condition(
  const TpgFFR& ffr,
  const TpgFaultList& fault_list,
  FaultInfo& fault_info,
  const ConfigParam& option
)
{
  if ( fault_list.empty() ) {
    return;
  }

  auto debug = option.get_bool_elem("debug", false);

  if ( debug ) {
    std::cout << "FFRAnalyze(FFR#" << ffr.id() << ")" << std::endl
	      << " fault_list:";
    for ( auto fault: fault_list ) {
      std::cout << " " << fault.str();
    }
    std::cout << std::endl;
  }

  // fault_list の FFR に対する故障伝搬条件を表すCNF式を持つSATソルバを作る．
  BdEngine engine(ffr.root(), option);
  auto pvar = engine.prop_var();
  engine.solver().add_clause(pvar);

  for ( auto fault: fault_list ) {
    if ( fault_info.is_rep(fault) ) {
      auto cond = fault.ffr_propagate_condition();
      auto lits = engine.conv_to_literal_list(cond);
      auto& det_cond = fault_info.detect_cond(fault);
      AssignList mand_cond;
      for ( auto as: det_cond ) {
	auto lit = engine.conv_to_literal(as);
	auto tmp_lits = lits;
	tmp_lits.push_back(~lit);
	auto res = engine.solver().solve(tmp_lits);
	if ( res == SatBool3::False ) {
	  // lit(as) を否定したら充足できなくなった．
	  // -> as は必須条件
	  mand_cond.add(as);
	}
      }
      fault_info.set_mandatory_condition(fault, mand_cond);
    }
  }
}

END_NAMESPACE_DRUID
