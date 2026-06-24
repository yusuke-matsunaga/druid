
/// @file FaultAnalyze.cc
/// @brief FaultAnalyze の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "FaultAnalyze.h"
#include "types/TpgFaultList.h"
#include "FFRAnalyze.h"
#include "MFFCAnalyze.h"
#include "Filter.h"
#include "Reducer.h"
#include "EqDomCand.h"
#include "ym/Timer.h"
#include "ym/MtMgr.h"
#include "ym/IdPool.h"
#include "ym/ExLock.h"
#include <thread>


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

TpgFaultList
update_fault_list(
  const TpgFaultList& src_list,
  const FaultInfo& fault_info
)
{
  TpgFaultList dst_list;
  for ( auto fault: src_list ) {
    if ( fault_info.is_rep(fault) ) {
      dst_list.push_back(fault);
    }
  }
  return dst_list;
}

void
ffr_analyze(
  FaultInfo& fault_info,
  const ConfigParam& option
)
{
  Timer timer;
  timer.start();

  auto multi_thread = option.get_bool_elem("multi_thread", false);
  auto verbose = option.get_bool_elem("verbose", false);

  auto fault_list_array = fault_info.fault_list().ffr_split();
  auto network = fault_info.network();
  if ( multi_thread ) {
    SizeType thread_num = option.get_int_elem("thread_num", 0);
    IdPool id_pool(network.ffr_num());
    MtMgr::run(
      [&](){
	for ( ; ; ) {
	  SizeType id;
	  if ( !id_pool.get(id) ) {
	    // 終わり
	    break;
	  }
	  auto ffr = network.ffr(id);
	  auto fault_list1 = fault_list_array[id];
	  FFRAnalyze::run(ffr, fault_list1, fault_info, option);
	}
      }, thread_num
    );
  }
  else {
    for ( auto ffr: network.ffr_list() ) {
      auto fault_list1 = fault_list_array[ffr.id()];
      FFRAnalyze::run(ffr, fault_list1, fault_info, option);
    }
  }

  timer.stop();
  if ( verbose ) {
    auto nf = fault_info.rep_fault_list().size();
    std::cout << "FFR analyze end" << std::endl
	      << " # of faults:            " << nf << std::endl
	      << " CPU time:               " << timer.get_time()
	      << "ms" << std::endl;
  }
}

void
mffc_reduction(
  FaultInfo& fault_info,
  const ConfigParam& option
)
{
  Timer timer;
  timer.start();

  auto multi_thread = option.get_bool_elem("multi_thread", false);
  auto verbose = option.get_bool_elem("verbose", false);

  auto fault_list_array = fault_info.fault_list().mffc_split();
  auto network = fault_info.network();
  if ( multi_thread ) {
    SizeType thread_num = option.get_int_elem("thread_num", 0);
    IdPool id_pool(network.mffc_num());
    MtMgr::run(
      [&](){
	for ( ; ; ) {
	  SizeType id;
	  if ( !id_pool.get(id) ) {
	    // 終わり
	    break;
	  }
	  auto& p = fault_list_array[id];
	  auto ffr = p.first;
	  if ( ffr.is_valid() ) {
	    // 単一の FFR だった．
	    continue;
	  }
	  auto& fault_list = p.second;
	  auto mffc = network.mffc(id);
	  MFFCAnalyze::run(mffc, fault_list, fault_info, option);
	}
      }, thread_num
    );
  }
  else {
    for ( auto mffc: network.mffc_list() ) {
      auto& p = fault_list_array[mffc.id()];
      auto ffr = p.first;
      if ( ffr.is_valid() ) {
	// 単一の FFR だった．
	continue;
      }
      auto& fault_list = p.second;
      MFFCAnalyze::run(mffc, fault_list, fault_info, option);
    }
  }

  timer.stop();
  if ( verbose ) {
    auto nf = fault_info.rep_fault_list().size();
    std::cout << "MFFC reduction end" << std::endl
	      << " # of faults:            " << nf << std::endl
	      << " CPU time:               " << timer.get_time()
	      << "ms" << std::endl;
  }
}

void
global_reduction(
  FaultInfo& fault_info,
  const ConfigParam& option
)
{
  Timer timer;
  timer.start();

  auto multi_thread = option.get_bool_elem("multi_thread", false);
  auto verbose = option.get_bool_elem("verbose", false);

  auto reducer_option = option.get_param("reducer");
  Reducer::run(fault_info, reducer_option);

  timer.stop();
  if ( verbose ) {
    auto nf = fault_info.rep_fault_list().size();
    std::cout << "Phase3 end" << std::endl
	      << " # of faults:            " << nf << std::endl
	      << " CPU time:               " << timer.get_time() << "ms"
	      << std::endl;
  }
}

END_NONAMESPACE

// @brief 故障の解析を行う．
FaultInfo
FaultAnalyze::run(
  const TpgFaultList& fault_list,
  const ConfigParam& option
)
{
  auto do_mffc_reduction = option.get_bool_elem("mffc_reduction", true);
  auto do_global_reduction = option.get_bool_elem("global_reduction", true);

  auto fault_info = FaultInfo(fault_list);

  // phase1: FFR ごとに故障の検出状況を求める．
  ffr_analyze(fault_info, option);

  if ( do_mffc_reduction ) {
    // Phase2: MFFC ごとに故障の支配関係を調べる．
    mffc_reduction(fault_info, option);
  }

  if ( do_global_reduction ) {
    // Phase3: 異なる FFR の故障の間の支配関係を調べる．
    global_reduction(fault_info, option);
  }

  return fault_info;
}

END_NAMESPACE_DRUID
