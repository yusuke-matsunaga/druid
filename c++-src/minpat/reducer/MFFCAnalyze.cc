
/// @file MFFCAnalyze.cc
/// @brief MFFCAnalyze の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "MFFCAnalyze.h"
#include "dtpg/FFR2Engine.h"


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
  auto debug = option.get_int_elem("debug", 0);

  if ( debug ) {
    std::cout << "MFFCAnalyze(MFFC#" << mffc.id()
	      << ")" << std::endl;
  }

  // FFR番号をキーにして各FFRごとの故障リストを格納した配列
  auto fault_list_array = fault_list.ffr_split();
  auto nffr = mffc.ffr_num();
  for ( SizeType i1 = 0; i1 < nffr - 1; ++ i1 ) {
    auto ffr1 = mffc.ffr(i1);
    // ffr1 の故障リスト
    auto& fault_list1 = fault_list_array[ffr1.id()];
    if ( debug ) {
      std::cout << "FFR#" << ffr1.id() << std::endl;
    }
    for ( SizeType i2 = i1 + 1; i2 < nffr; ++ i2 ) {
      auto ffr2 = mffc.ffr(i2);
      if ( debug ) {
	std::cout << "  FFR#" << ffr2.id() << std::endl;
      }
      // ffr2 の故障リスト
      auto& fault_list2 = fault_list_array[ffr2.id()];

      // mffc の根を起点とする伝搬条件を求めるエンジン
      FFR2Engine engine(ffr1, ffr2, fault_list1, fault_list2, option);

      for ( auto fault1: fault_list1 ) {
	if ( debug ) {
	  std::cout << "    " << fault1.str() << std::endl;
	}
	for ( auto fault2: fault_list2 ) {
	  if ( debug ) {
	    std::cout << "      " << fault2.str() << std::endl;
	  }
	  // fault1 と fault2 を同時に検出できるか調べる．
	  auto res = engine.solve(fault1, fault2, true, true);
	  if ( res != SatBool3::True ) {
	    continue;
	  }
	  // fault1 を検出して fault2 を検出しない条件を調べる．
	  auto dom1 = engine.solve(fault1, fault2, true, false) == SatBool3::False;
	  // fault2 を検出して fault1 を検出しない条件を調べる．
	  auto dom2 = engine.solve(fault1, fault2, false, true) == SatBool3::False;
	  if ( dom1 ) {
	    // fault2 は fault1 に支配されている．
	    if ( dom2 ) {
	      // fault1 と fault2 は等価故障だった．
	      if ( fault1.id() < fault2.id() ) {
		fault_info.set_rep(fault2, fault1);
	      }
	      else {
		fault_info.set_rep(fault1, fault2);
		break;
	      }
	    }
	    else {
	      fault_info.set_dominator(fault2, fault1);
	    }
	  }
	  else if ( dom2 ) {
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
