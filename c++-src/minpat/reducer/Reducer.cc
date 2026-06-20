
/// @file Reducer.cc
/// @brief Reducer の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "Reducer.h"
#include "EqDomCand.h"
#include "dtpg/NaiveDualEngine.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

std::string
time_str(
  Timer& timer
)
{
  std::ostringstream buf;
  buf << std::setw(11)
      << std::fixed
      << std::setprecision(2)
      << timer.get_time()
      << "ms";
  return buf.str();
}

END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// クラス Reducer
//////////////////////////////////////////////////////////////////////

// @brief 故障グループの細分化を行ってから支配関係を調べる．
void
Reducer::run(
  FaultInfo& fault_info,
  const EqDomCand& cand,
  const ConfigParam& option
)
{
  SizeType TIME_LIMIT = option.get_int_elem("time_limit", 0);
  auto verbose = option.get_bool_elem("verbose", false);
  auto debug = option.get_int_elem("debug", 0);

  // 故障シミュレーションを用いて候補を絞る．

  // 現時点での代表故障のリスト
  auto fault_list = fault_info.rep_fault_list();

  Timer timer;
  timer.start();

  // 等価故障の検査を行う．
  SizeType eq_check_count = 0;
  SizeType eq_succ_count = 0;
  Timer eq_timer;
  eq_timer.start();
  auto ng = cand.group_num();
  for ( SizeType g = 0; g < ng; ++ g ) {
    auto& group = cand.group(g);
    auto nf = group.size();
    if ( nf == 1 ) {
      continue;
    }
    if ( debug > 2 ) {
      std::cout << "Group#" << g << ":";
      for ( auto fault: group ) {
	std::cout << " " << fault.str();
      }
      std::cout << std::endl;
    }
    // とりあえずリファレンス用に単純なアルゴリズムを用いる．
    for ( SizeType i1 = 0; i1 < nf - 1; ++ i1 ) {
      auto fault1 = group[i1];
      for ( SizeType i2 = i1 + 1; i2 < nf; ++ i2 ) {
	auto fault2 = group[i2];
	if ( !fault_info.is_rep(fault2) ) {
	  continue;
	}
	NaiveDualEngine engine(fault1, fault2, option);
	++ eq_check_count;
	auto res01 = engine.solve(false, true, TIME_LIMIT);
	auto res10 = engine.solve(true, false, TIME_LIMIT);
	if ( res01 == SatBool3::False ) {
	  if ( res10 == SatBool3::False ) {
	    // fault1 と fault2 は等価故障
	    fault_info.set_rep(fault2, fault1);
	    ++ eq_succ_count;
	    if ( debug > 0 ) {
	      std::cout << fault1.str() << " and " << fault2.str()
			<< " are equivalent" << std::endl;
	    }
	    continue;
	  }
	  else if ( res10 == SatBool3::True ) {
	    // fault1 は fault2 に支配されている．
	    fault_info.set_dominator(fault1, fault2);
	    ++ eq_succ_count;
	    if ( debug > 0 ) {
	      std::cout << fault1.str() << " is dominated by " << fault2.str()
			<< std::endl;
	    }
	    break;
	  }
	}
	else if ( res01 == SatBool3::True ) {
	  if ( res10 == SatBool3::False ) {
	    // fault2 は fault1 に支配されている．
	    fault_info.set_dominator(fault2, fault1);
	    ++ eq_succ_count;
	    if ( debug > 0 ) {
	      std::cout << fault2.str() << " is dominated by "
			<< fault1.str() << std::endl;
	    }
	    continue;
	  }
	}
      }
    }
  }
  eq_timer.stop();

  if ( verbose ) {
    auto rep_num = fault_info.rep_fault_list().size();
    std::cout << std::endl
	      << "Equivalence check end:    " << std::endl
	      << "Total checks:             " << std::setw(8) << std::right
	      << eq_check_count << std::endl
	      << "Total succeeds:           " << std::setw(8) << std::right
	      << eq_succ_count << std::endl
	      << "# of faults:              " << std::setw(8) << std::right
	      << rep_num << std::endl;
  }

  // 支配故障の検査を行う．
  SizeType dom_check_count = 0;
  SizeType dom_succ_count = 0;
  Timer dom_timer;
  dom_timer.start();
  SizeType fault_count = 0;
  for ( auto fault1: fault_list ) {
    if ( !fault_info.is_rep(fault1) ) {
      continue;
    }
    if ( debug > 1 ) {
      std::cout << "   " << fault_count << "/" << fault_list.size() << std::endl;
      ++ fault_count;
    }
    auto id1 = cand.group_id(fault1);
    auto& dom_list = cand.dom_list(id1);
    for ( auto id2: dom_list ) {
      for ( auto fault2: cand.group(id2) ) {
	if ( !fault_info.is_rep(fault2) ) {
	  continue;
	}
	NaiveDualEngine engine(fault1, fault2, option);
	auto res = engine.solve(true, false, TIME_LIMIT);
	++ dom_check_count;
	if ( res == SatBool3::False ) {
	  fault_info.set_dominator(fault2, fault1);
	  ++ dom_succ_count;
	  if ( debug > 0 ) {
	    std::cout << "    "
		      << fault2.str() << " is dominated by "
		      << fault1.str() << std::endl;
	  }
	  continue;
	}
      }
    }
  }
  dom_timer.stop();

  timer.stop();
  if ( verbose ) {
    std::cout << std::endl
	      << "Dominance check end:      " << std::endl
	      << "Total checks:             " << std::setw(8) << std::right
	      << dom_check_count << std::endl
	      << "Total succeeds:           " << std::setw(8) << std::right
	      << dom_succ_count << std::endl
	      << "Total CPU TIme:           " << time_str(timer) << std::endl;
  }
}

END_NAMESPACE_DRUID
