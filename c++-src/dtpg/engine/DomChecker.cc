
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

//////////////////////////////////////////////////////////////////////
// クラス DomChecker
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
DomChecker::DomChecker(
  const TpgFFR& ffr1,
  const TpgFFR& ffr2,
  const TpgFaultList& fault_list1,
  const TpgFaultList& fault_list2,
  const ConfigParam& option
) : mFaultList1{fault_list1},
    mFaultList2{fault_list2},
    mDomList1(fault_list1.size()),
    mDomList2(fault_list2.size())
{
  mPosList1.reserve(fault_list1.size());
  mPosList2.reserve(fault_list2.size());

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
  mCheckCount = 0;
  for ( SizeType i1 = 0; i1 < nf1; ++ i1 ) {
    auto fault1 = fault_list1[i1];
    if ( debug ) {
      std::cout << "fault1 = " << fault1.str()
		<< std::endl;
    }
    auto dlits1 = dlits_array1[i1];
    for ( SizeType i2 = 0; i2 < nf2; ++ i2 ) {
      if ( mDomList2[i2].is_valid() ) {
	// すでに支配されている．
	continue;
      }
      auto fault2 = fault_list2[i2];
      auto dlits2 = dlits_array2[i2];

      ++ mCheckCount;
      { // fault1 と fault2 を同時に検出できるか調べる．
	auto tmp_lits = concat_lits(dlits1, dlits2);
	auto res = engine.solver().solve(tmp_lits);
	if ( debug ) {
	  std::cout << "  fault2 = " << fault2.str() << std::endl
		    << "    res = " << res << std::endl;
	}
	if ( res == SatBool3::False ) {
	  continue;
	}
	if ( res == SatBool3::X ) {
	  continue;
	}
      }

      { // fault1 を検出して fault2 を検出しない条件を調べる．
	auto tmp_lits = dlits1;
	tmp_lits.push_back(clit_array2[i2]);
	auto res1 = engine.solver().solve(tmp_lits);
	if ( res1 == SatBool3::False ) {
	  // fault2 は支配されている．
	  mPosList2.push_back(i2);
	  mDomList2[i2] = fault1;
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
	auto tmp_lits = dlits2;
	tmp_lits.push_back(clit_array1[i1]);
	auto res2 = engine.solver().solve(tmp_lits);
	if ( res2 == SatBool3::False ) {
	  // fault1 は支配されている．
	  mPosList1.push_back(i1);
	  mDomList1[i1] = fault2;
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
}

// @brief 故障の情報を fault_info にコピーする．
void
DomChecker::copy(
  FaultInfo& fault_info
) const
{
  for ( auto pos: mPosList1 ) {
    auto fault1 = mFaultList1[pos];
    auto fault2 = mDomList1[pos];
    fault_info.set_dominator(fault1, fault2);
  }
  for ( auto pos: mPosList2 ) {
    auto fault1 = mFaultList2[pos];
    auto fault2 = mDomList2[pos];
    fault_info.set_dominator(fault1, fault2);
  }
}

// @brief dominator1() 中の有効な故障の数を返す．
SizeType
DomChecker::dom1_count() const
{
  return mPosList1.size();
}

// @brief dominator2() 中の有効な故障の数を返す．
SizeType
DomChecker::dom2_count() const
{
  return mPosList2.size();
}

END_NAMESPACE_DRUID
