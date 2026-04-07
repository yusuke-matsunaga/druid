
/// @file ffr_reduction.cc
/// @brief ffr_reduction の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "types/TpgFaultList.h"
#include "FFRDomChecker.h"
#include "ym/Timer.h"
#include "ym/MtMgr.h"
#include "ym/IdPool.h"
#include "ym/ExLock.h"
#include <thread>


BEGIN_NAMESPACE_DRUID

// @brief 対象の故障リストとテストパタンのリストを求める．
TpgFaultList
ffr_reduction(
  const TpgFaultList& fault_list,
  const ConfigParam& option
)
{
  Timer timer;
  timer.start();

  bool multi_thread = option.get_bool_elem("multi_thread", false);
  bool verbose = option.get_bool_elem("verbose", false);

  auto network = fault_list.network();

  auto ffr_fault_list_array = fault_list.ffr_split();

  // FFR ごとに支配関係を調べる．
  std::vector<bool> del_mark(network.max_fault_id(), false);
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
	  auto& fault_list = ffr_fault_list_array[id];
	  FFRDomChecker checker(ffr, fault_list, del_mark);
	  checker.run();
	}
      }, thread_num
    );
  }
  else {
    for ( auto ffr: network.ffr_list() ) {
      auto& fault_list = ffr_fault_list_array[ffr.id()];
      FFRDomChecker checker(ffr, fault_list, del_mark);
      checker.run();
    }
  }
  // 結果の故障リストを作る．
  TpgFaultList new_fault_list;
  for ( auto fault: fault_list ) {
    if ( del_mark[fault.id()] ) {
      continue;
    }
    new_fault_list.push_back(fault);
  }
  timer.stop();

  if ( verbose ) {
    std::cout << std::left << std::setw(20)
	      << "ffr_reduction end:" << timer.get_time() << std::endl
	      << std::left << std::setw(20)
	      << "# of reduced faults: " << new_fault_list.size() << std::endl;
  }
  return new_fault_list;
}

END_NAMESPACE_DRUID
