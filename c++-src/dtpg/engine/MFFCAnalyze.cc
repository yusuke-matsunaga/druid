
/// @file MFFCAnalyze.cc
/// @brief MFFCAnalyze の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "MFFCAnalyze.h"
#include "types/TpgFaultList.h"
#include "dtpg/BdEngine.h"
#include "BoolDiffEnc.h"


BEGIN_NAMESPACE_DRUID

// @brief 処理を行う関数
void
MFFCAnalyze::run(
  const TpgMFFC& mffc,
  const TpgFaultList& fault_list,
  FaultInfo& fault_info,
  const ConfigParam& option
)
{
  auto debug = option.get_bool_elem("debug", false);

  if ( debug ) {
    std::cout << "MFFCAnalyze(MFFC#" << mffc.id()
	      << ")" << std::endl;
  }

  // FFR番号をキーにして各FFRごとの故障リストを格納した配列
  auto fault_list_array = fault_list.ffr_split();
  auto nffr = mffc.ffr_num();
  for ( SizeType i1 = 0; i1 < nffr - 1; ++ i1 ) {
    auto ffr1 = mffc.ffr(i1);
    if ( debug ) {
      std::cout << "FFR#" << ffr1.id() << std::endl;
    }
    for ( SizeType i2 = i1 + 1; i2 < nffr; ++ i2 ) {
      auto ffr2 = mffc.ffr(i2);
      if ( debug ) {
	std::cout << "  FFR#" << ffr2.id() << std::endl;
      }
      // mffc の根を起点とする伝搬条件を求めるエンジン
      BdEngine engine(mffc.root(), option);
      auto pvar = engine.prop_var();
      engine.solver().add_clause(pvar);
      // ffr1 の根から mffc の根までの伝搬条件を表すエンコーダ
      auto enc1 = new BoolDiffEnc(ffr1.root(), mffc.root(), option);
      // ffr2 の根から mffc の根までの伝搬条件を表すエンコーダ
      auto enc2 = new BoolDiffEnc(ffr2.root(), mffc.root(), option);
      engine.add_subenc(std::unique_ptr<SubEnc>{enc1});
      engine.add_subenc(std::unique_ptr<SubEnc>{enc2});
      // ffr1 の故障リスト
      auto& fault_list1 = fault_list_array[ffr1.id()];
      auto nf1 = fault_list1.size();
      // fault_list1 の故障の検出条件
      std::vector<std::vector<SatLiteral>> dlits1_array;
      dlits1_array.reserve(nf1);
      // fault_list1 の故障の非検出条件
      std::vector<SatLiteral> clit1_array;
      clit1_array.reserve(nf1);
      auto pvar1 = enc1->prop_var();
      for ( auto fault: fault_list1 ) {
	auto cond = fault.ffr_propagate_condition();
	auto lits = engine.conv_to_literal_list(cond);
	lits.push_back(pvar1);
	dlits1_array.push_back(lits);
	auto clit = engine.solver().new_variable(true);
	clit1_array.push_back(clit);
	std::vector<SatLiteral> tmp_lits;
	tmp_lits.reserve(lits.size() + 1);
	tmp_lits.push_back(~clit);
	for ( auto lit: lits ) {
	  tmp_lits.push_back(~lit);
	}
	engine.solver().add_clause(tmp_lits);
      }

      // ffr2 の故障リスト
      auto& fault_list2 = fault_list_array[ffr2.id()];
      auto nf2 = fault_list2.size();
      // fault_list2 の故障の検出条件
      std::vector<std::vector<SatLiteral>> dlits2_array;
      dlits2_array.reserve(nf2);
      // fault_list2 の故障の非検出条件
      std::vector<SatLiteral> clit2_array;
      clit2_array.reserve(nf2);
      auto pvar2 = enc2->prop_var();
      for ( auto fault: fault_list2 ) {
	auto cond = fault.ffr_propagate_condition();
	auto lits = engine.conv_to_literal_list(cond);
	lits.push_back(pvar2);
	dlits2_array.push_back(lits);
	auto clit = engine.solver().new_variable(true);
	clit2_array.push_back(clit);
	std::vector<SatLiteral> tmp_lits;
	tmp_lits.reserve(lits.size() + 1);
	tmp_lits.push_back(~clit);
	for ( auto lit: lits ) {
	  tmp_lits.push_back(~lit);
	}
	engine.solver().add_clause(tmp_lits);
      }

      for ( SizeType j1 = 0; j1 < nf1; ++ j1 ) {
	auto fault1 = fault_list1[j1];
	auto dlits1 = dlits1_array[j1];
	auto clit1 = clit1_array[j1];
	if ( debug ) {
	  std::cout << "    " << fault1.str() << std::endl;
	}
	for ( SizeType j2 = 0; j2 < nf2; ++ j2 ) {
	  auto fault2 = fault_list2[j2];
	  auto dlits2 = dlits2_array[j2];
	  auto clit2 = clit2_array[j2];
	  if ( debug ) {
	    std::cout << "      " << fault2.str() << std::endl;
	  }
	  // fault1 と fault2 を同時に検出できるか調べる．
	  auto tmp_lits = concat_lits(dlits1, dlits2);
	  auto res = engine.solve(tmp_lits);
	  if ( res != SatBool3::True ) {
	    continue;
	  }
	  // fault1 を検出して fault2 を検出しない条件を調べる．
	  auto tmp_lits1 = dlits1;
	  tmp_lits1.push_back(clit2);
	  auto res1 = engine.solve(tmp_lits1);
	  if ( res1 == SatBool3::False ) {
	    // fault2 は fault1 に支配されている．
	    fault_info.set_dominator(fault2, fault1);
	    continue;
	  }
	  // fault2 を検出して fault1 を検出しない条件を調べる．
	  auto tmp_lits2 = dlits2;
	  tmp_lits2.push_back(clit1);
	  auto res2 = engine.solve(tmp_lits2);
	  if ( res2 == SatBool3::False ) {
	    // fault1 は fault2 に支配されている．
	    fault_info.set_dominator(fault1, fault2);
	    break;
	  }
	}
      }
    }
  }
}

END_NAMESPACE_DRUID
