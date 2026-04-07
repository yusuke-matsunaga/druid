
/// @file DomChecker.cc
/// @brief DomChecker の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "DomChecker.h"
#include "types/TpgFaultList.h"
#include "dtpg/Bd2Engine.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// clit_list 中の pos 番目のみがアサートされたリテラルのリストを返す．
inline
std::vector<SatLiteral>
make_lits(
  const std::vector<SatLiteral>& clit_array,
  SizeType pos
)
{
  auto n = clit_array.size();
  std::vector<SatLiteral> lits(n);
  for ( SizeType i = 0; i < n; ++ i ) {
    if ( i == pos ) {
      lits[i] = clit_array[i];
    }
    else {
      lits[i] = ~clit_array[i];
    }
  }
  return lits;
}

END_NONAMESPACE

// @brief ２つのFFRの故障の間の支配関係を調べる．
DomChecker::Stats
DomChecker::run(
  const TpgFFR& ffr1,
  const TpgFFR& ffr2,
  const TpgFaultList& fault_list1,
  const TpgFaultList& fault_list2,
  FaultInfo& fault_info,
  const ConfigParam& option
)
{
  auto multi_thread = option.get_bool_elem("multi_thread", false);
  auto debug = option.get_bool_elem("debug", false);
  auto verbose = option.get_bool_elem("verbose", false);

  auto network = ffr1.network();

  // ２つの BoolDiffEnc を持つSATエンジン
  Bd2Engine engine(ffr1.root(), ffr2.root(), option);

  if ( debug ) {
    std::cout << "DomChecker FFR#" << ffr1.id()
	      << " and FFR#" << ffr2.id() << std::endl;
    std::cout << " fault_list1 =";
    for ( auto fault: fault_list1 ) {
      std::cout << " " << fault.str();
    }
    std::cout << std::endl;
    std::cout << " fault_list2 =";
    for ( auto fault: fault_list2 ) {
      std::cout << " " << fault.str();
    }
    std::cout << std::endl;
  }

  auto nf1 = fault_list1.size();
  // fault_list1 に含まれる故障の検出条件の配列
  // キーは fault_list1 中の位置
  std::vector<std::vector<SatLiteral>> dlits_array1(nf1);
  // fault_list1 に含まれる故障の非検出条件を表すリテラルの配列
  // キーは fault_list1 中の位置
  std::vector<SatLiteral> clit_array1(nf1);
  for ( SizeType i = 0; i < nf1; ++ i ) {
    auto fault = fault_list1[i];
    auto cond = fault.ffr_propagate_condition();
    auto lits = engine.conv_to_literal_list(cond);
    auto pvar = engine.prop_var1();
    lits.push_back(pvar);
    dlits_array1[i] = lits;
    auto clit = engine.solver().new_variable(true);
    clit_array1[i] = clit;
    std::vector<SatLiteral> tmp_lits;
    tmp_lits.reserve(lits.size() + 1);
    tmp_lits.push_back(~clit);
    for ( auto lit: lits ) {
      tmp_lits.push_back(~lit);
    }
    engine.solver().add_clause(tmp_lits);
  }

  auto nf2 = fault_list2.size();
  // fault_list1 に含まれる故障の検出条件の配列
  // キーは fault_list2 中の位置
  std::vector<std::vector<SatLiteral>> dlits_array2(nf2);
  // fault_list1 に含まれる故障の非検出条件を表すリテラルの配列
  // キーは fault_list2 中の位置
  std::vector<SatLiteral> clit_array2(nf2);
  for ( SizeType i = 0; i < nf2; ++ i ) {
    auto fault = fault_list2[i];
    auto cond = fault.ffr_propagate_condition();
    auto lits = engine.conv_to_literal_list(cond);
    auto pvar = engine.prop_var2();
    lits.push_back(pvar);
    dlits_array2[i] = lits;
    auto clit = engine.solver().new_variable(true);
    clit_array2[i] = clit;
    std::vector<SatLiteral> tmp_lits;
    tmp_lits.reserve(lits.size() + 1);
    tmp_lits.push_back(~clit);
    for ( auto lit: lits ) {
      tmp_lits.push_back(~lit);
    }
    engine.solver().add_clause(tmp_lits);
  }

  // ２つの故障の両立性を調べる．
  // 両立していた場合は支配関係も調べる．
  auto stats = Stats{0, 0, 0};
  for ( SizeType i1 = 0; i1 < nf1; ++ i1 ) {
    auto fault1 = fault_list1[i1];
    if ( !fault_info.is_rep(fault1) ) {
      continue;
    }
    if ( debug ) {
      std::cout << "fault1 = " << fault1.str()
		<< std::endl;
    }
    auto dlits1 = dlits_array1[i1];
    for ( SizeType i2 = 0; i2 < nf2; ++ i2 ) {
      auto fault2 = fault_list2[i2];
      if ( !fault_info.is_rep(fault2) ) {
	continue;
      }
      auto dlits2 = dlits_array2[i2];

      ++ stats.check_count;
      { // fault1 と fault2 を同時に検出できるか調べる．
	auto tmp_lits = concat_lits(dlits1, dlits2);
	auto res = engine.solver().solve(tmp_lits);
	if ( debug ) {
	  std::cout << "  fault2 = " << fault2.str() << std::endl
		    << "    res = " << res << std::endl;
	}
	if ( res != SatBool3::True ) {
	  continue;
	}
	fault_info.set_compatible(fault1, fault2);
      }

      { // fault1 を検出して fault2 を検出しない条件を調べる．
	auto nlits2 = make_lits(clit_array2, i2);
	auto tmp_lits = concat_lits(dlits1, nlits2);
	auto res1 = engine.solver().solve(tmp_lits);
	if ( res1 == SatBool3::False ) {
	  ++ stats.dom1_count;
	  // fault2 は支配されている．
	  fault_info.set_dominated(fault2);
	  if ( debug ) {
	    std::cout << fault2.str()
		      << " is dominated by "
		      << fault1.str()
		      << std::endl;
	  }
	  // fault2 は削除されたので逆は調べない．
	  continue;
	}
      }

      { // fault2 を検出して fault1 を検出しない条件を調べる．
	auto nlits1 = make_lits(clit_array1, i1);
	auto tmp_lits = concat_lits(nlits1, dlits2);
	auto res2 = engine.solver().solve(tmp_lits);
	if ( res2 == SatBool3::False ) {
	  ++ stats.dom2_count;
	  // fault1 は支配されている．
	  fault_info.set_dominated(fault1);
	  if ( debug ) {
	    std::cout << fault1.str()
		      << " is dominated by "
		      << fault2.str()
		      << std::endl;
	  }
	  // fault1 は削除されたので残りは調べない．
	  break;
	}
      }
    }
  }

  return stats;
}

END_NAMESPACE_DRUID
