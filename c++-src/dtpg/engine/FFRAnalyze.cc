
/// @file FFRAnalyze.cc
/// @brief FFRAnalyze の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "FFRAnalyze.h"
#include "types/TpgFaultList.h"
#include "dtpg/FFREngine.h"


BEGIN_NAMESPACE_DRUID

// @brief 処理を行うクラスメソッド
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
  auto engine_option = option;
  engine_option.add("has_ulit", ffr_reduction);
  FFREngine engine(ffr, fault_list, engine_option);

  // 個々の故障の検出状況を調べる．
  TpgFaultList det_list;
  for ( auto fault: fault_list ) {
    auto lits = engine.dlits(fault);
    auto res = engine.solver().solve(lits);
    if ( res == SatBool3::True ) {
      if ( debug ) {
	std::cout << fault.str() << " is detected" << std::endl;
      }
      auto model = engine.solver().model();
      auto suff_cond = engine.extract_sufficient_condition(fault, model);
      auto pi_assign = engine.justify(suff_cond, model);
      auto tv = TestVector(pi_assign);
      auto det_cond = suff_cond.main_cond();
      fault_info.set_detected(fault, det_cond, tv);
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
      det_list.push_back(fault);
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

  // ２つの故障の両立性を調べる．
  // 両立していた場合は支配関係も調べる．
  for ( SizeType i1 = 0; i1 < ndet - 1; ++ i1 ) {
    auto fault1 = det_list[i1];
    if ( !fault_info.is_rep(fault1) ) {
      continue;
    }
    if ( debug ) {
      std::cout << "fault1 = " << fault1.str() << std::endl;
    }
    auto& dlits1 = engine.dlits(fault1);
    for ( SizeType i2 = i1 + 1; i2 < ndet; ++ i2 ) {
      auto fault2 = det_list[i2];
      if ( !fault_info.is_rep(fault2) ) {
	continue;
      }
      if ( debug ) {
	std::cout << "  fault2 = " << fault2.str() << std::endl;
      }
      auto& dlits2 = engine.dlits(fault2);
      auto tmp_lits = concat_lits(dlits1, dlits2);
      auto res = engine.solver().solve(tmp_lits);
      if  ( res != SatBool3::True ) {
	continue;
      }
      if ( ffr_reduction ) {
	// fault1 を検出して fault2 を検出しない条件を調べる．
	auto tmp_lits1 = dlits1;
	auto ulit2 = engine.ulit(fault2);
	tmp_lits1.push_back(ulit2);
	auto res1 = engine.solver().solve(tmp_lits1);
	auto dom1 = res1 == SatBool3::False;
	if ( dom1 ) {
	  if ( debug ) {
	    std::cout << "    " << fault2.str()
		      << " is dominated by "
		      << fault1.str()
		      << std::endl;
	  }
	  // fault2 は支配されている．
	  fault_info.set_dominator(fault2, fault1);
	  // fault2 は削除されたので逆は調べない．
	  continue;
	}
	// fault2 を検出して fault1 を検出しない条件を調べる．
	auto tmp_lits2 = dlits2;
	auto ulit1 = engine.ulit(fault1);
	tmp_lits2.push_back(ulit1);
	auto res2 = engine.solver().solve(tmp_lits2);
	auto dom2 = res2 == SatBool3::False;
	if ( dom1 ) {
	  // fault1 が fault2 を支配している．
	  if ( dom2 ) {
	    if ( debug ) {
	      std::cout << "    " << fault1.str()
			<< " and "
			<< fault2.str()
			<< " are equivalent"
			<< std::endl;
	    }
	    // fault2 が fault1 を支配している．
	    // -> fault1 と fault2 は等価
	    if ( fault1.id() < fault2.id() ) {
	      fault_info.set_rep(fault2, fault1);
	    }
	    else {
	      fault_info.set_rep(fault1, fault2);
	      break;
	    }
	  }
	  else {
	    if ( debug ) {
	      std::cout << "    " << fault2.str()
			<< " is dominated by "
			<< fault1.str()
			<< std::endl;
	    }
	    fault_info.set_dominator(fault2, fault1);
	  }
	}
	else if ( dom2 ) {
	  if ( debug ) {
	    std::cout << "    " << fault1.str()
		      << " is dominated by "
		      << fault2.str()
		      << std::endl;
	  }
	  // fault2 が fault1 を支配している．
	  fault_info.set_dominator(fault1, fault2);
	  break;
	}
      }
    }
  }
}

END_NAMESPACE_DRUID
