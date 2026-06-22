
/// @file Reducer.cc
/// @brief Reducer の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "Reducer.h"
#include "EqDomCandMgr.h"
#include "EqDomCand.h"
#include "PatGen.h"
#include "dtpg/NaiveDualEngine.h"
#include "fsim/Fsim.h"


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
  const ConfigParam& option
)
{
  // パラメータの取得
  SizeType NO_CHANGE_LIMIT = option.get_int_elem("no_change_limit", 1000);
  SizeType BATCH_SIZE = std::min(64, option.get_int_elem("batch_size", 64));
  SizeType TIME_LIMIT = option.get_int_elem("time_limit", 0);
  auto verbose = option.get_bool_elem("verbose", false);
  auto debug = option.get_int_elem("debug", 0);

  // 現時点での代表故障のリスト
  auto fault_list = fault_info.rep_fault_list();

  Timer timer;
  timer.start();

  // 故障シミュレータ
  auto fsim_option = option.get_param("fsim");
  Fsim fsim(fault_list, fsim_option);

  // パタンを作るオブジェクト
  auto patgen_option = option.get_param("patgen");
  auto patgen = PatGen::new_obj(fault_info, patgen_option);

  // 等価故障/支配故障の候補を管理するオブジェクト
  auto candmgr_option = option.get_param("candmgr");
  auto candmgr = EqDomCandMgr::new_obj(fault_list, candmgr_option);

  auto network = fault_info.network();
  auto max_size = network.max_fault_id();

  Timer fsim_timer;
  SizeType tv_count = 0;
  SizeType no_change = 0;
  while ( no_change < NO_CHANGE_LIMIT ) {
    std::vector<TestVector> tv_list(BATCH_SIZE);
    patgen->gen(BATCH_SIZE, tv_list);
    fsim_timer.start();
    auto res = fsim.run_multi(tv_list, true);
    fsim_timer.stop();
    tv_count += BATCH_SIZE;
    patgen->update(res);

    auto ntv = res.tv_num();
    std::vector<PackedVal> dpat_array(max_size, PV_ALL0);
    for ( SizeType i = 0; i < ntv; ++ i ) {
      PackedVal bit = 1ULL << i;
      for ( auto fault: res.fault_list(i) ) {
	auto fid = fault.id();
	dpat_array[fid] |= bit;
      }
    }
    auto change = candmgr->update(dpat_array);
    if ( change ) {
      no_change = 0;
    }
    else {
      no_change += BATCH_SIZE;
    }
  }

  auto cand = candmgr->end(false);

  if ( verbose ) {
    std::cout << "# of faults:            "
	      << std::setw(8) << std::right << fault_list.size() << std::endl
	      << "# of Groups:            "
	      << std::setw(8) << std::right << cand->group_num() << std::endl
	      << "Total # of patterns:    "
	      << std::setw(8) << std::right << tv_count << std::endl
	      << "No Change Limit:        "
	      << std::setw(8) << std::right << NO_CHANGE_LIMIT << std::endl
	      << "Filtering time:           " << time_str(timer) << std::endl
	      << " (Fsim time):             " << time_str(fsim_timer) << std::endl;
  }

  // 等価故障の検査を行う．
  SizeType eq_check_count = 0;
  SizeType eq_succ_count = 0;
  Timer eq_timer;
  eq_timer.start();
  auto ng = cand->group_num();
  for ( SizeType g = 0; g < ng; ++ g ) {
    auto& group = cand->group(g);
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
  for ( auto fault2: fault_list ) {
    if ( !fault_info.is_rep(fault2) ) {
      continue;
    }
    if ( debug > 1 ) {
      std::cout << "   " << fault_count << "/" << fault_list.size() << std::endl;
      ++ fault_count;
    }
    auto id2 = cand->group_id(fault2);
    auto& dom_list = cand->dom_list2(id2);
    bool done = false;
    for ( auto id1: dom_list ) {
      for ( auto fault1: cand->group(id1) ) {
	if ( !fault_info.is_rep(fault1) ) {
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
	  done = true;
	  break;
	}
      }
      if ( done ) {
	break;
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
