
/// @file Dichotomy.cc
/// @brief Dichotomy の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "Dichotomy.h"
#include "DiGroupMgr.h"
#include "NaiveMgr.h"
#include "types/TpgNetwork.h"
#include "types/TestVector.h"
#include "types/PackedVal.h"
#include "fsim/Fsim.h"
#include "dtpg/NaiveDualEngine.h"
#include "ym/HeapTree.h"


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

class FaultComp
{
public:

  FaultComp(
    SizeType size
  ) : count_array(size, 0)
  {
  }

  int
  operator()(
    SizeType id1,
    SizeType id2
  ) const
  {
    auto c1 = count_array[id1];
    auto c2 = count_array[id2];
    if ( c1 < c2 ) {
      return -1;
    }
    if ( c1 > c2 ) {
      return 1;
    }
    return 0;
  }

  void
  inc_count(
    SizeType id
  )
  {
    ++ count_array[id];
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  std::vector<SizeType> count_array;

};

inline
TpgFaultList
rep_fault_list(
  const TpgFaultList& src_list,
  const FaultInfo& fault_info
)
{
  TpgFaultList fault_list;
  fault_list.reserve(src_list.size());
  for ( auto fault: src_list ) {
    if ( fault_info.is_rep(fault) ) {
      fault_list.push_back(fault);
    }
  }
  return fault_list;
}

void
check_list(
  const std::string& label,
  const TpgFaultList& fault_list1,
  const TpgFaultList& fault_list2
)
{
  auto n1 = fault_list1.size();
  auto n2 = fault_list2.size();
  SizeType i1 = 0;
  SizeType i2 = 0;
  while ( i1 < n1 && i2 < n2 ) {
    auto fault1 = fault_list1[i1];
    auto fault2 = fault_list2[i2];
    if ( fault1.id() < fault2.id() ) {
      break;
    }
    if ( fault1.id() > fault2.id() ) {
      break;
    }
    ++ i1;
    ++ i2;
  }
  if ( i1 < n1 || i2 < n2 ) {
    // 内容が異なっていた．
    std::cout << label << std::endl;
    SizeType i1 = 0;
    SizeType i2 = 0;
    while ( i1 < n1 && i2 < n2 ) {
      auto fault1 = fault_list1[i1];
      auto fault2 = fault_list2[i2];
      if ( fault1.id() < fault2.id() ) {
	std::cout << "1: " << fault1.str() << std::endl;
	++ i1;
      }
      else if ( fault1.id() > fault2.id() ) {
	std::cout << "2: " << fault2.str() << std::endl;
	++ i2;
      }
      else {
	++ i1;
	++ i2;
      }
    }
    for ( ; i1 < n1; ++ i1 ) {
      auto fault1 = fault_list1[i1];
      std::cout << "1: " << fault1.str() << std::endl;
    }
    for ( ; i2 < n2; ++ i2 ) {
      auto fault2 = fault_list2[i2];
      std::cout << "2: " << fault2.str() << std::endl;
    }
  }
}

void
check(
  const TpgFaultList& fault_list,
  const DiGroupMgr& dgmgr,
  const NaiveMgr& naivemgr
)
{
  SizeType max_id = 0;
  for ( auto fault: fault_list ) {
    max_id = std::max(max_id, fault.id());
  }
  ++ max_id;
  std::vector<TpgFaultList> domcand_list_array1(max_id);
  std::vector<TpgFaultList> eqcand_list_array1(max_id);
  std::vector<TpgFaultList> domcand_list_array2(max_id);
  std::vector<TpgFaultList> eqcand_list_array2(max_id);

  for ( auto group: dgmgr.group_list() ) {
    for ( auto fault: group->fault_list() ) {
      auto& domcand_list = domcand_list_array1[fault.id()];
      for ( auto dom_group: group->dominance_list() ) {
	for ( auto fault1: dom_group->fault_list() ) {
	  if ( fault1 != fault ) {
	    domcand_list.push_back(fault1);
	  }
	}
      }
      auto& eqcand_list = eqcand_list_array1[fault.id()];
      for ( auto fault1: group->fault_list() ) {
	if ( fault1 == fault ) {
	  continue;
	}
	eqcand_list.push_back(fault1);
      }
    }
  }

  for ( auto fault: fault_list ) {
    auto& domcand_list = domcand_list_array2[fault.id()];
    for ( auto fault1: naivemgr.domcand_list(fault) ) {
      domcand_list.push_back(fault1);
    }
    auto& eqcand_list = eqcand_list_array2[fault.id()];
    for ( auto fault1: naivemgr.eqcand_list(fault) ) {
      eqcand_list.push_back(fault1);
    }
  }

  for ( auto fault: fault_list ) {
    auto& eqcand_list1 = eqcand_list_array1[fault.id()];
    eqcand_list1.sort();
    auto& eqcand_list2 = eqcand_list_array2[fault.id()];
    eqcand_list2.sort();
    std::ostringstream buf1;
    buf1 << "EQ CAND for " << fault.str();
    check_list(buf1.str(), eqcand_list1, eqcand_list2);

    auto& domcand_list1 = domcand_list_array1[fault.id()];
    domcand_list1.sort();
    auto& domcand_list2 = domcand_list_array2[fault.id()];
    domcand_list2.sort();
    std::ostringstream buf2;
    buf2 << "DOM CAND for " << fault.str();
    check_list(buf2.str(), domcand_list1, domcand_list2);
  }
}

END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// クラス Dichotomy
//////////////////////////////////////////////////////////////////////

// @brief 故障グループの細分化を行ってから支配関係を調べる．
void
Dichotomy::run(
  FaultInfo& fault_info,
  const ConfigParam& option
)
{
  SizeType NO_CHANGE_LIMIT = option.get_int_elem("no_change_limit", 1000);
  SizeType BATCH_SIZE = std::min(64, option.get_int_elem("batch_size", 64));
  SizeType TIME_LIMIT = option.get_int_elem("time_limit", 0);
  auto verbose = option.get_bool_elem("verbose", false);
  auto debug = option.get_int_elem("debug", 0);

  // 現時点での代表故障のリスト
  auto fault_list = fault_info.rep_fault_list();
  auto fault_num = fault_list.size();

  // 対象のネットワーク
  auto network = fault_list.network();

  // 故障シミュレータ
  auto fsim_option = option.get_param("fsim");
  Fsim fsim(fault_list, fsim_option);

  // 初期グループは全ての故障を含んだ一つのグループ．
  auto mgr = DiGroupMgr(fault_list);

  // 故障番号をキーにして検出回数で比較する関数オブジェクト
  FaultComp comp(network.max_fault_id());
  // 故障番号を入れるヒープ木
  auto heap = HeapTree<SizeType, FaultComp>(comp);
  for ( auto fault: fault_list ) {
    heap.put_item(fault.id());
  }

  Timer scr_timer;
  scr_timer.start();

  Timer fsim_timer;
  Timer dicho_timer;
  Timer naive_timer;
  SizeType tv_count = 0;
  std::mt19937 randgen;
  SizeType no_change = 0;
  NaiveMgr naivemgr(fault_list, network.max_fault_id());
  // 未検出の故障がある場合，必ず検出されるパタンが選ばれるので
  // no_change とはならない．
  // よってループ中止の条件に「ヒープ内に未検出の故障がない」は不要
  while ( mgr.group_num() < fault_num &&
	  no_change < NO_CHANGE_LIMIT ) {
    std::vector<TestVector> tv_list(BATCH_SIZE);
    for ( SizeType base = 0; base < BATCH_SIZE; ++ base ) {
      if ( heap.empty() ) {
	auto tv1 = TestVector(network);
	tv1.set_from_random(randgen);
	tv_list[base] = tv1;
      }
      else {
	// 検出回数が最小の故障を一つ選ぶ．
	auto fid = heap.get_min();
	auto min_fault = network.fault(fid);
	// この故障を検出するテストベクタを用いて故障シミュレーションを行う．
	auto tv1 = fault_info.testvector(min_fault);
	tv1.fix_x_from_random(randgen);
	tv_list[base] = tv1;
      }
    }
    fsim_timer.start();
    auto res = fsim.run_multi(tv_list, true);
    fsim_timer.stop();
    tv_count += BATCH_SIZE;

    // 検出回数を更新する．
    auto ntv = res.tv_num();
    for ( SizeType i = 0; i < ntv; ++ i ) {
      for ( auto fault: res.fault_list(i) ) {
	auto fid = fault.id();
	if ( heap.is_in(fid) ) {
	  comp.inc_count(fid);
	  heap.update(fid);
	}
      }
    }

    // シミュレーション結果に基づいて細分化を行う．
    dicho_timer.start();
    auto new_mgr = DiGroupMgr::dichotomy(mgr, res, option);
    dicho_timer.stop();

    naive_timer.start();
    naivemgr.add(res);
    naive_timer.stop();

    {
#if 0
      auto ntv = res.tv_num();
      for ( SizeType i = 0; i < ntv; ++ i ) {
	std::cout << "#" << i << std::endl;
	for ( auto fault: res.fault_list(i) ) {
	  std::cout << " " << fault.str();
	}
	std::cout << std::endl;
      }
#endif
      check(fault_list, new_mgr, naivemgr);
      //std::cout << std::endl;
    }
    if ( new_mgr != mgr ) {
      // 細分化できたら更新する．
      std::swap(mgr, new_mgr);
      if ( debug > 1 ) {
	std::cout << "#" << tv_count << ": "
		  << mgr.group_num()
		  << std::endl;
      }
      no_change = 0;
    }
    else {
      no_change += BATCH_SIZE;
    }
  }
  scr_timer.stop();

  if ( verbose ) {
    std::cout << "# of faults:            " << std::setw(8) << std::right << fault_list.size() << std::endl
	      << "# of Groups:            " << std::setw(8) << std::right << mgr.group_num() << std::endl
	      << "Total # of patterns:    " << std::setw(8) << std::right << tv_count << std::endl
	      << "No Change Limit:        " << std::setw(8) << std::right << NO_CHANGE_LIMIT << std::endl
	      << "Screening time:           " << time_str(scr_timer) << std::endl
	      << " (Fsim time):             " << time_str(fsim_timer) << std::endl
	      << " (Dichotomy time):        " << time_str(dicho_timer) << std::endl
	      << " (NaiveMgr Time):         " << time_str(naive_timer) << std::endl;
  }

  auto naive_fault_info = fault_info;

  Timer timer;
  timer.start();

  // DiGroup 内の故障が等価故障かどうか調べる．
  SizeType check_count = 0;
  SizeType succ_count = 0;
  Timer timer_phase1;
  timer_phase1.start();
  for ( auto group: mgr.group_list() ) {
    auto& fault_list = group->fault_list();
    auto nf = fault_list.size();
    if ( nf == 1 ) {
      continue;
    }
    if ( debug > 2 ) {
      std::cout << "Group" << group->id() << ":";
      for ( auto fault: fault_list ) {
	std::cout << " " << fault.str();
      }
      std::cout << std::endl;
    }
    // とりあえずリファレンス用に単純なアルゴリズムを用いる．
    for ( SizeType i1 = 0; i1 < nf - 1; ++ i1 ) {
      auto fault1 = fault_list[i1];
      for ( SizeType i2 = i1 + 1; i2 < nf; ++ i2 ) {
	auto fault2 = fault_list[i2];
	if ( !fault_info.is_rep(fault2) ) {
	  continue;
	}
	NaiveDualEngine engine(fault1, fault2, option);
	++ check_count;
	auto res01 = engine.solve(false, true, TIME_LIMIT);
	auto res10 = engine.solve(true, false, TIME_LIMIT);
	if ( res01 == SatBool3::False ) {
	  if ( res10 == SatBool3::False ) {
	    // fault1 と fault2 は等価故障
	    fault_info.set_rep(fault2, fault1);
	    ++ succ_count;
	    if ( debug > 0 ) {
	      std::cout << fault1.str() << " and " << fault2.str()
			<< " are equivalent" << std::endl;
	    }
	    continue;
	  }
	  else if ( res10 == SatBool3::True ) {
	    // fault1 は fault2 に支配されている．
	    fault_info.set_dominator(fault1, fault2);
	    ++ succ_count;
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
	    ++ succ_count;
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
  timer_phase1.stop();

  if ( verbose ) {
    auto rep_num = fault_info.rep_fault_list().size();
    std::cout << std::endl
	      << "Equivalence check end:    " << std::endl
	      << "Total checks:             " << std::setw(8) << std::right << check_count << std::endl
	      << "Total succeeds:           " << std::setw(8) << std::right << succ_count << std::endl
	      << "# of faults:              " << std::setw(8) << std::right << rep_num << std::endl;
  }

  // 残ったグループ内の故障は無関係となる．
  check_count = 0;
  succ_count = 0;
  Timer timer_phase2;
  timer_phase2.start();
  for ( auto group1: mgr.group_list() ) {
    auto fault_list1 = rep_fault_list(group1->fault_list(), fault_info);
    if ( fault_list1.empty() ) {
      continue;
    }
    if ( debug > 2 ) {
      std::cout << "Group#" << group1->id() << ":";
      for ( auto fault: fault_list1 ) {
	std::cout << " " << fault.str();
      }
      std::cout << std::endl;
    }
    auto nf1 = fault_list1.size();
    for ( auto group2: group1->dominance_list() ) {
      auto fault_list2 = rep_fault_list(group2->fault_list(), fault_info);
      if ( fault_list2.empty() ) {
	continue;
      }
      if ( debug > 2 ) {
	std::cout << "  Group#" << group2->id() << ":";
	for ( auto fault: fault_list2 ) {
	  std::cout << " " << fault.str();
	}
	std::cout << std::endl;
      }
      auto nf2 = fault_list2.size();
      for ( SizeType i1 = 0; i1 < nf1; ++ i1 ) {
	auto fault1 = fault_list1[i1];
	for ( SizeType i2 = 0; i2 < nf2; ++ i2 ) {
	  auto fault2 = fault_list2[i2];
	  if ( fault1 == fault2 ) {
	    continue;
	  }
	  if ( !fault_info.is_rep(fault2) ) {
	    continue;
	  }
	  NaiveDualEngine engine(fault1, fault2, option);
	  auto res = engine.solve(true, false, TIME_LIMIT);
	  ++ check_count;
	  if ( res == SatBool3::False ) {
	    fault_info.set_dominator(fault2, fault1);
	    ++ succ_count;
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
  }
  timer_phase2.stop();

  timer.stop();
  if ( verbose ) {
    std::cout << std::endl
	      << "Dominance check end:      " << std::endl
	      << "Total checks:             " << std::setw(8) << std::right << check_count << std::endl
	      << "Total succeeds:           " << std::setw(8) << std::right << succ_count << std::endl
	      << "---------------------------------------------------------------"
	      << std::endl
	      << "Phase1 Time:              " << time_str(timer_phase1) << std::endl
	      << "Phase2 Time:              " << time_str(timer_phase2) << std::endl
	      << "Total CPU TIme:           " << time_str(timer) << std::endl;
  }

  {
    Timer timer;
    timer.start();
    Timer timer_phase1;
    timer_phase1.start();
    SizeType check_count = 0;
    SizeType succ_count = 0;
    for ( auto fault1: fault_list ) {
      if ( !naive_fault_info.is_rep(fault1) ) {
	continue;
      }
      for ( auto fault2: naivemgr.eqcand_list(fault1) ) {
	if ( !naive_fault_info.is_rep(fault2) ) {
	  continue;
	}
	NaiveDualEngine engine(fault1, fault2, option);
	++ check_count;
	auto res10 = engine.solve(true, false, TIME_LIMIT);
	auto res01 = engine.solve(false, true, TIME_LIMIT);
	if ( res10 == SatBool3::False && res01 == SatBool3::False ) {
	  // fault1 と fault2 は等価
	  naive_fault_info.set_rep(fault2, fault1);
	  ++ succ_count;
	}
      }
    }
    timer_phase1.stop();
    if ( verbose ) {
      std::cout << std::endl
		<< "Equivalence check end:    " << std::endl
		<< "Total checks:             " << std::setw(8) << std::right << check_count << std::endl
		<< "Total succeeds:           " << std::setw(8) << std::right << succ_count << std::endl;
    }

    Timer timer_phase2;
    timer_phase2.start();
    check_count = 0;
    succ_count = 0;
    for ( auto fault1: fault_list ) {
      if ( !naive_fault_info.is_rep(fault1) ) {
	continue;
      }
      for ( auto fault2: naivemgr.domcand_list(fault1) ) {
	if ( !naive_fault_info.is_rep(fault2) ) {
	  continue;
	}
	NaiveDualEngine engine(fault1, fault2, option);
	++ check_count;
	auto res10 = engine.solve(true, false, TIME_LIMIT);
	if ( res10 == SatBool3::False ) {
	  // fault2 は fault1 に支配されている．
	  naive_fault_info.set_dominator(fault2, fault1);
	  ++ succ_count;
	}
      }
    }
    timer_phase2.stop();

    timer.stop();
    if ( verbose ) {
      std::cout << std::endl
		<< "Dominance check end:      " << std::endl
		<< "Total checks:             " << std::setw(8) << std::right << check_count << std::endl
		<< "Total succeeds:           " << std::setw(8) << std::right << succ_count << std::endl
		<< "-------------------------------------------------------" << std::endl
		<< "Phase1 Time:              " << time_str(timer_phase1) << std::endl
		<< "Phase2 Time:              " << time_str(timer_phase2) << std::endl
		<< "Total CPU Time:           " << time_str(timer) << std::endl;
    }
  }
}

END_NAMESPACE_DRUID
