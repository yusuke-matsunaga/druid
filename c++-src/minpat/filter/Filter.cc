
/// @file Filter.cc
/// @brief Filter の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "Filter.h"
#include "PatGen.h"
#include "CandMgr.h"
#include "EqDomCand.h"
#include "types/TpgNetwork.h"
#include "types/TestVector.h"
#include "fsim/Fsim.h"
#include "ym/Timer.h"


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
// クラス Filter
//////////////////////////////////////////////////////////////////////

// @brief fault_info に対して支配故障候補を求める．
std::unique_ptr<EqDomCand>
Filter::run(
  const FaultInfo& fault_info,
  const ConfigParam& option
)
{
  // パラメータの取得
  SizeType NO_CHANGE_LIMIT = option.get_int_elem("no_change_limit", 1000);
  SizeType BATCH_SIZE = std::min(64, option.get_int_elem("batch_size", 64));
  auto verbose = option.get_bool_elem("verbose", false);
  auto debug = option.get_int_elem("debug", 0);

  Timer timer;
  timer.start();

  auto fault_list = fault_info.rep_fault_list();

  // 故障シミュレータ
  auto fsim_option = option.get_param("fsim");
  Fsim fsim(fault_list, fsim_option);

  // パタンを作るオブジェクト
  auto patgen_option = option.get_param("patgen");
  auto patgen = PatGen::new_obj(fault_info, patgen_option);

  // 等価故障/支配故障の候補を管理するオブジェクト
  auto candmgr_option = option.get_param("candmgr");
  auto candmgr = CandMgr::new_obj(fault_list, candmgr_option);

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

  // 結果の EqDomCand を作る．
  auto cand = candmgr->end(true);
  timer.stop();

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

  return cand;
}

// @brief 等価故障と支配故障の候補を求める．
std::unique_ptr<EqDomCand>
Filter::run2(
  const FaultInfo& fault_info,
  const ConfigParam& option
)
{
  // パラメータの取得
  SizeType NO_CHANGE_LIMIT = option.get_int_elem("no_change_limit", 1000);
  SizeType BATCH_SIZE = std::min(64, option.get_int_elem("batch_size", 64));
  auto verbose = option.get_bool_elem("verbose", false);
  auto debug = option.get_int_elem("debug", 0);

  Timer timer;
  timer.start();

  auto fault_list = fault_info.rep_fault_list();

  // 故障シミュレータ
  auto fsim_option = option.get_param("fsim");
  Fsim fsim(fault_list, fsim_option);

  // パタンを作るオブジェクト
  auto patgen_option = option.get_param("patgen");
  auto patgen = PatGen::new_obj(fault_info, patgen_option);

  // 等価故障/支配故障の候補を管理するオブジェクト
  auto candmgr_option = option.get_param("candmgr");
  auto candmgr = CandMgr::new_obj(fault_list, candmgr_option);
  //auto naive_mgr = CandMgr::new_naive_mgr(fault_list, candmgr_option);
  auto naive_mgr = CandMgr::new_dichotomy_mgr(fault_list, candmgr_option);
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
    auto change2 = naive_mgr->update(dpat_array);
    if ( change != change2 ) {
      std::cout << "change = " << change
		<< ", change2 = " << change2 << std::endl;
      abort();
    }
    if ( change ) {
      no_change = 0;
    }
    else {
      no_change += BATCH_SIZE;
    }
    auto cand1 = candmgr->end(true);
    auto cand2 = naive_mgr->end(true);
    if ( *cand1 != *cand2 ) {
      std::cout << "cand1" << std::endl;
      cand1->print(std::cout);
      std::cout << std::endl;
      std::cout << "naive_cand" << std::endl;
      cand2->print(std::cout);
      std::cout << std::endl;
      cand1->check(*cand2);
      abort();
    }
  }

  // 結果の EqDomCand を作る．
  auto cand = candmgr->end(true);
  auto naive_cand = naive_mgr->end(true);
  timer.stop();

  if ( *cand != *naive_cand ) {
    std::cout << "cand" << std::endl;
    cand->print(std::cout);
    std::cout << std::endl;
    std::cout << "naive_cand" << std::endl;
    naive_cand->print(std::cout);
    std::cout << std::endl;
    cand->check(*naive_cand);
  }

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

  return cand;
}

END_NAMESPACE_DRUID
