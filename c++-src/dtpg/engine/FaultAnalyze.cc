
/// @file FaultAnalyze.cc
/// @brief FaultAnalyze の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "dtpg/FaultAnalyze.h"
#include "types/TpgFaultList.h"
#include "dtpg/BdEngine.h"
#include "FFRAnalyze.h"
#include "DomChecker.h"
#include "ym/MtMgr.h"
#include "ym/IdPool.h"
#include "ym/ExLock.h"
#include <thread>


BEGIN_NAMESPACE_DRUID

// @brief 故障の解析を行う．
FaultInfo
FaultAnalyze::run(
  const TpgFaultList& fault_list,
  const ConfigParam& option
)
{
  Timer timer;

  auto multi_thread = option.get_bool_elem("multi_thread", false);
  auto global_reduction = option.get_bool_elem("global_reduction", true);
  auto verbose = option.get_bool_elem("verbose", false);

  auto network = fault_list.network();
  auto fault_info = FaultInfo(network.max_fault_id());

  // phase1: FFR ごとに故障の検出状況を求める．
  timer.start();
  auto ffr_fault_list_array = fault_list.ffr_split();
  if ( multi_thread ) {
    SizeType thread_num = option.get_int_elem("thread_num", 0);
    IdPool id_pool(network.ffr_num());
    ExLock r_lock;
    MtMgr::run(
      [&](){
	for ( ; ; ) {
	  SizeType id;
	  if ( !id_pool.get(id) ) {
	    // 終わり
	    break;
	  }
	  auto ffr = network.ffr(id);
	  auto fault_list1 = ffr_fault_list_array[ffr.id()];
	  FFRAnalyze::run(ffr, fault_list1, fault_info, option);
	}
      }, thread_num
    );
  }
  else {
    for ( auto ffr: network.ffr_list() ) {
      auto fault_list1 = ffr_fault_list_array[ffr.id()];
      FFRAnalyze::run(ffr, fault_list1, fault_info, option);
    }
  }

  // 支配されていない故障のリストを作る．
  auto nffr = network.ffr_num();
  std::vector<TpgFaultList> rep_fault_list_array(nffr);
  for ( SizeType i = 0; i < nffr; ++ i ) {
    auto& fault_list1 = ffr_fault_list_array[i];
    auto& rep_fault_list = rep_fault_list_array[i];
    for ( auto fault: fault_list1 ) {
      if ( fault_info.is_rep(fault) ) {
	rep_fault_list.push_back(fault);
      }
    }
  }

  timer.stop();
  if ( verbose ) {
    SizeType nf = 0;
    for ( auto fault: fault_list ) {
      if ( fault_info.is_rep(fault) ) {
	++ nf;
      }
    }
    std::cout << "Phase1 end" << std::endl
	      << " # of faults:            " << nf << std::endl
	      << " CPU time:               " << timer.get_time()
	      << "ms" << std::endl;
  }

  // Phase2: 異なる FFR の故障の間の支配関係を調べる．
  if ( global_reduction ) {
    timer.reset();
    timer.start();
    auto stats = DomChecker::Stats{0, 0, 0};
    for ( SizeType i1 = 0; i1 < nffr - 1; ++ i1 ) {
      auto ffr1 = network.ffr(i1);
      auto& fault_list1 = rep_fault_list_array[i1];
      for ( SizeType i2 = i1 + 1; i2 < nffr; ++ i2 ) {
	auto ffr2 = network.ffr(i2);
	auto& fault_list2 = rep_fault_list_array[i2];
	auto stats1 = DomChecker::run(ffr1, ffr2, fault_list1, fault_list2,
				      fault_info, option);
	stats += stats1;
      }
    }
    timer.stop();
    if ( verbose ) {
      SizeType nf = 0;
      for ( auto fault: fault_list ) {
	if ( fault_info.is_rep(fault) ) {
	  ++ nf;
	}
      }
      std::cout << "Phase2 end" << std::endl
		<< " # of faults:            " << nf << std::endl
		<< " CPU time:               " << timer.get_time() << "ms" << std::endl
		<< " # of compatible checks: " << stats.check_count << std::endl
		<< " # of dominance type1:   " << stats.dom1_count << std::endl
		<< " # of dominance type2:   " << stats.dom2_count << std::endl;
    }
  }

  // Phase3: 残った故障に対して必須条件を求める．
  timer.reset();
  timer.start();
  if ( multi_thread ) {
    SizeType thread_num = option.get_int_elem("thread_num", 0);
    IdPool id_pool(network.ffr_num());
    ExLock r_lock;
    MtMgr::run(
      [&](){
	for ( ; ; ) {
	  SizeType id;
	  if ( !id_pool.get(id) ) {
	    // 終わり
	    break;
	  }
	  auto ffr = network.ffr(id);
	  auto fault_list1 = ffr_fault_list_array[ffr.id()];
	  FFRAnalyze::get_mandatory_condition(ffr, fault_list1, fault_info, option);
	}
      }, thread_num
    );
  }
  else {
    for ( auto ffr: network.ffr_list() ) {
      auto fault_list1 = ffr_fault_list_array[ffr.id()];
      FFRAnalyze::get_mandatory_condition(ffr, fault_list1, fault_info, option);
    }
  }
  timer.stop();
  if ( verbose ) {
    std::cout << "Phase3 end" << std::endl
	      << " CPU time:               " << timer.get_time()
	      << "ms" << std::endl;
  }

  return fault_info;
}

END_NAMESPACE_DRUID
