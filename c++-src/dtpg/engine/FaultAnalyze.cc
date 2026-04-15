
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
#include "MFFCAnalyze.h"
#include "DomChecker.h"
#include "ym/MtMgr.h"
#include "ym/IdPool.h"
#include "ym/ExLock.h"
#include <thread>


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

void
dfs(
  const TpgNode& node,
  std::vector<SizeType>& support,
  std::vector<bool>& mark
)
{
  if ( mark[node.id()] ) {
    return;
  }
  mark[node.id()] = true;

  if ( node.is_ppi() ) {
    support.push_back(node.input_id());
  }
  else {
    for ( auto inode: node.fanin_list() ) {
      dfs(inode, support, mark);
    }
  }
}

bool
check_intersect(
  const std::vector<SizeType>& sup1,
  const std::vector<SizeType>& sup2
)
{
  auto p1 = sup1.begin();
  auto e1 = sup1.end();
  auto p2 = sup2.begin();
  auto e2 = sup2.end();
  while ( p1 != e1 && p2 != e2 ) {
    auto v1 = *p1;
    auto v2 = *p2;
    if ( v1 < v2 ) {
      ++ p1;
    }
    else if ( v1 > v2 ) {
      ++ p2;
    }
    else { // v1 == v2
      return true;
    }
  }
  return false;
}

SizeType
count_intersect(
  const std::vector<SizeType>& sup1,
  const std::vector<SizeType>& sup2
)
{
  SizeType count = 0;
  auto p1 = sup1.begin();
  auto e1 = sup1.end();
  auto p2 = sup2.begin();
  auto e2 = sup2.end();
  while ( p1 != e1 && p2 != e2 ) {
    auto v1 = *p1;
    auto v2 = *p2;
    if ( v1 < v2 ) {
      ++ p1;
    }
    else if ( v1 > v2 ) {
      ++ p2;
    }
    else { // v1 == v2
      ++ count;
      ++ p1;
      ++ p2;
    }
  }
  return count;
}

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
phase1(
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
    std::cout << "Phase1 end" << std::endl
	      << " # of faults:            " << nf << std::endl
	      << " CPU time:               " << timer.get_time()
	      << "ms" << std::endl;
  }
}

void
phase2(
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
    std::cout << "Phase2 end" << std::endl
	      << " # of faults:            " << nf << std::endl
	      << " CPU time:               " << timer.get_time()
	      << "ms" << std::endl;
  }
}

void
phase3(
  FaultInfo& fault_info,
  const ConfigParam& option
)
{
  Timer timer;
  timer.start();

  auto multi_thread = option.get_bool_elem("multi_thread", false);
  auto verbose = option.get_bool_elem("verbose", false);

  auto fault_list_array = fault_info.rep_fault_list().ffr_split();
  auto network = fault_info.network();

  SizeType check_count = 0;
  SizeType dom1_count = 0;
  SizeType dom2_count = 0;

  struct FFR_Pair {
    TpgFFR ffr1;
    TpgFFR ffr2;
    SizeType intersect{0};
    SizeType dom1_count{0};
    SizeType dom2_count{0};
  };

  // FFR のペアのリストを求める．
  std::vector<FFR_Pair> pair_list;
  auto nffr = network.ffr_num();
  pair_list.reserve( nffr * (nffr - 1) / 2 );
  for ( SizeType i1 = 0; i1 < nffr - 1; ++ i1 ) {
    auto ffr1 = network.ffr(i1);
    for ( SizeType i2 = i1 + 1; i2 < nffr; ++ i2 ) {
      auto ffr2 = network.ffr(i2);
      pair_list.push_back({ffr1, ffr2});
    }
  }
  SizeType npairs = pair_list.size();

  if ( multi_thread ) {
    SizeType thread_num = option.get_int_elem("thread_num", 0);
    IdPool id_pool(npairs);
    ExLock all_lock;
    MtMgr::run(
      [&](){
	for ( ; ; ) {
	  SizeType id;
	  if ( !id_pool.get(id) ) {
	    // 終わり
	    break;
	  }
	  auto& p = pair_list[id];
	  auto ffr1 = p.ffr1;
	  auto id1 = ffr1.id();
	  auto fault_list1 = update_fault_list(fault_list_array[id1],
					       fault_info);
	  auto ffr2 = p.ffr2;
	  auto id2 = ffr2.id();
	  auto fault_list2 = update_fault_list(fault_list_array[id2],
					       fault_info);
	  auto checker = DomChecker(ffr1, ffr2,
				    fault_list1, fault_list2,
				    option);
	  all_lock.run([&]() {
	    check_count += checker.check_count();
	    dom1_count += checker.dom1_count();
	    dom2_count += checker.dom2_count();
	    checker.copy(fault_info);
	    p.dom1_count = checker.dom1_count();
	    p.dom2_count = checker.dom2_count();
	  });
	}
      }, thread_num
    );
  }
  else {
    for ( auto& p: pair_list ) {
      auto ffr1 = p.ffr1;
      auto id1 = ffr1.id();
      auto fault_list1 = update_fault_list(fault_list_array[id1],
					   fault_info);
      auto ffr2 = p.ffr2;
      auto id2 = ffr2.id();
      auto fault_list2 = update_fault_list(fault_list_array[id2],
					   fault_info);
      auto checker = DomChecker(ffr1, ffr2,
				fault_list1, fault_list2,
				option);
      check_count += checker.check_count();
      dom1_count += checker.dom1_count();
      dom2_count += checker.dom2_count();
      checker.copy(fault_info);
      p.dom1_count = checker.dom1_count();
      p.dom2_count = checker.dom2_count();
    }
  }
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
  auto mffc_reduction = option.get_bool_elem("mffc_reduction", true);
  auto global_reduction = option.get_bool_elem("global_reduction", true);

  auto fault_info = FaultInfo(fault_list);

  // phase1: FFR ごとに故障の検出状況を求める．
  phase1(fault_info, option);

  if ( mffc_reduction ) {
    // Phase2: MFFC ごとに故障の支配関係を調べる．
    phase2(fault_info, option);
  }

  if ( global_reduction ) {
    // Phase3: 異なる FFR の故障の間の支配関係を調べる．
    phase3(fault_info, option);
  }

  return fault_info;
}

END_NAMESPACE_DRUID
