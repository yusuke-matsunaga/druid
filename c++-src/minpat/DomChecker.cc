
/// @file DomChecker.cc
/// @brief DomChecker の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "DomChecker.h"
#include "types/TpgFaultList.h"
#include "dtpg/FFR2Engine.h"


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
    mRepList1(fault_list1.size()),
    mDomList1(fault_list1.size()),
    mRepList2(fault_list2.size()),
    mDomList2(fault_list2.size())
{
  auto multi_thread = option.get_bool_elem("multi_thread", false);
  auto debug = option.get_int_elem("debug", 0);
  auto verbose = option.get_bool_elem("verbose", false);

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
  auto nf2 = fault_list2.size();
  mPosList1.reserve(nf1);
  mPosList2.reserve(nf2);

  auto network = ffr1.network();

  FFR2Engine engine(ffr1, ffr2, fault_list1, fault_list2, option);

  // ２つの故障の両立性を調べる．
  // 両立していた場合は支配関係も調べる．
  mCheckCount = 0;
  for ( SizeType i1 = 0; i1 < nf1; ++ i1 ) {
    if ( mDomList1[i1].is_valid() ) {
      // すでに支配されている．
      continue;
    }
    auto fault1 = fault_list1[i1];
    if ( debug ) {
      std::cout << "fault1 = " << fault1.str()
		<< std::endl;
    }
    for ( SizeType i2 = 0; i2 < nf2; ++ i2 ) {
      if ( mDomList2[i2].is_valid() ) {
	// すでに支配されている．
	continue;
      }
      ++ mCheckCount;
      auto fault2 = fault_list2[i2];
      if ( debug ) {
	std::cout << "  fault2 = " << fault2.str() << std::endl;
      }
      { // fault1 と fault2 を同時に検出できるか調べる．
	auto res = engine.solve(fault1, fault2, true, true);
	if ( debug ) {
	  std::cout << "    res = " << res << std::endl;
	}
	if ( res == SatBool3::False ) {
	  continue;
	}
	if ( res == SatBool3::X ) {
	  continue;
	}
      }

      // fault1 を検出して fault2 を検出しない条件を調べる．
      bool dom1 = engine.solve(fault1, fault2, true, false) == SatBool3::False;

      // fault2 を検出して fault1 を検出しない条件を調べる．
      bool dom2 = engine.solve(fault1, fault2, false, true) == SatBool3::False;

      if ( dom1 ) {
	if ( dom2 ) {
	  if ( debug ) {
	    std::cout << fault1.str()
		      << " and "
		      << fault2.str()
		      << " are equivallent"
		      << std::endl;
	  }
	  // fault1 と fault2 は等価だった．
	  // 番号の若い方を代表とする．
	  if ( fault1.id() < fault2.id() ) {
	    set_rep2(i2, fault1);
	  }
	  else {
	    set_rep1(i1, fault2);
	    break;
	  }
	}
	else {
	  if ( debug ) {
	    std::cout << fault2.str()
		      << " is dominated by "
		      << fault1.str()
		      << std::endl;
	  }
	  set_dominator2(i2, fault1);
	}
      }
      else if ( dom2 ) {
	// fault1 は fault2 に支配されている．
	set_dominator1(i1, fault2);
	if ( debug ) {
	  std::cout << fault1.str()
		    << " is dominated by "
		    << fault2.str()
		    << std::endl;
	}
	break;
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
    auto fault = mFaultList1[pos];
    auto rep_fault = mRepList1[pos];
    if ( rep_fault.is_valid() ) {
      fault_info.set_rep(fault, rep_fault);
    }
    auto dom_fault = mDomList1[pos];
    if ( dom_fault.is_valid() ) {
      fault_info.set_dominator(fault, dom_fault);
    }
  }
  for ( auto pos: mPosList2 ) {
    auto fault = mFaultList2[pos];
    auto rep_fault = mRepList2[pos];
    if ( rep_fault.is_valid() ) {
      fault_info.set_rep(fault, rep_fault);
    }
    auto dom_fault = mDomList2[pos];
    if ( dom_fault.is_valid() ) {
      fault_info.set_dominator(fault, dom_fault);
    }
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
