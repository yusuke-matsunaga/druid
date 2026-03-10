
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
  const JsonValue& option
)
{
  Timer timer;
  timer.start();

  bool multi_thread = get_bool(option, "multi_thread", false);

  auto network = fault_list.network();

  std::unordered_map<SizeType, TpgFaultList> fault_list_dict;
  for ( auto fault: fault_list ) {
    auto root_id = fault.ffr_root().id();
    if ( fault_list_dict.count(root_id) == 0 ) {
      fault_list_dict.emplace(root_id, TpgFaultList());
    }
    fault_list_dict.at(root_id).push_back(fault);
  }

  // FFR ごとに支配関係を調べる．
  std::vector<bool> del_mark(network.max_fault_id(), false);
  if ( multi_thread ) {
    SizeType thread_num = get_int(option, "thread_num", 0);
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
	  auto root_id = ffr.root().id();
	  if ( fault_list_dict.count(root_id) > 0 ) {
	    auto& fault_list = fault_list_dict.at(root_id);
	    FFRDomChecker checker(ffr, fault_list, del_mark);
	    checker.run();
	  }
	}
      }, thread_num
    );
  }
  else {
    for ( auto ffr: network.ffr_list() ) {
      auto root_id = ffr.root().id();
      if ( fault_list_dict.count(root_id) > 0 ) {
	auto& fault_list = fault_list_dict.at(root_id);
	FFRDomChecker checker(ffr, fault_list, del_mark);
	checker.run();
      }
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

  {
    std::cout << std::left << std::setw(20)
	      << "ffr_reduction end:" << timer.get_time() << std::endl
	      << std::left << std::setw(20)
	      << "# of reduced faults: " << new_fault_list.size() << std::endl;
  }
  return new_fault_list;
}

END_NAMESPACE_DRUID
