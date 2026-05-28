
/// @file DiFilter.cc
/// @brief DiFilter の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "DiFilter.h"
#include "DiGroupMgr.h"
#include "types/TpgNetwork.h"
#include "types/TestVector.h"
#include "types/PackedVal.h"
#include "fsim/Fsim.h"
#include "ym/HeapTree.h"
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

// count_array[id] の値で整列したヒープ木
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

END_NONAMESPACE


//////////////////////////////////////////////////////////////////////
// クラス DiFilter
//////////////////////////////////////////////////////////////////////

// @brief 等価故障と支配故障の候補を求める．
EqDomCand
DiFilter::run(
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

  auto fault_list = fault_info.rep_fault_list();
  auto fault_num = fault_list.size();
  auto network = fault_list.network();

  // 故障シミュレータ
  auto fsim_option = option.get_param("fsim");
  Fsim fsim(fault_list, fsim_option);

  // 故障番号をキーにして検出回数で比較する関数オブジェクト
  FaultComp comp(network.max_fault_id());
  // 故障番号を入れるヒープ木
  auto heap = HeapTree<SizeType, FaultComp>(comp);
  for ( auto fault: fault_list ) {
    heap.put_item(fault.id());
  }

  Timer timer;
  timer.start();

  // 初期グループは全ての故障を含んだ一つのグループ．
  auto mgr = DiGroupMgr(fault_list);

  Timer fsim_timer;
  Timer dicho_timer;
  SizeType tv_count = 0;
  std::mt19937 randgen;
  SizeType no_change = 0;
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
    // その結果を用いいてヒープ木を更新する．
    // 同時に各故障の検出ビットパタンを作る．
    auto ntv = res.tv_num();
    std::vector<PackedVal> dpat_array(network.max_fault_id(), PV_ALL0);
    for ( SizeType i = 0; i < ntv; ++ i ) {
      PackedVal bit = 1ULL << i;
      for ( auto fault: res.fault_list(i) ) {
	auto fid = fault.id();
	dpat_array[fid] |= bit;
	if ( heap.is_in(fid) ) {
	  comp.inc_count(fid);
	  heap.update(fid);
	}
      }
    }

    // シミュレーション結果に基づいて細分化を行う．
    dicho_timer.start();
    auto new_mgr = DiGroupMgr::dichotomy(mgr, dpat_array, option);
    dicho_timer.stop();

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

  // 結果の EqDomCand を作る．
  EqDomCand cand;
  cand.init(fault_list);
  for ( auto group: mgr.group_list() ) {
    cand.add_eqgroup(group->fault_list());
    for ( auto fault: group->fault_list() ) {
      TpgFaultList dom_list;
      for ( auto dom_group: group->dominance_list() ) {
	for ( auto fault1: dom_group->fault_list() ) {
	  if ( fault1 != fault ) {
	    dom_list.push_back(fault1);
	  }
	}
      }
      cand.set_domcand(fault, dom_list);
    }
  }
  timer.stop();

  if ( verbose ) {
    std::cout << "# of faults:            " << std::setw(8) << std::right << fault_list.size() << std::endl
	      << "# of Groups:            " << std::setw(8) << std::right << cand.eqgroup_num() << std::endl
	      << "Total # of patterns:    " << std::setw(8) << std::right << tv_count << std::endl
	      << "No Change Limit:        " << std::setw(8) << std::right << NO_CHANGE_LIMIT << std::endl
	      << "Filtering time:           " << time_str(timer) << std::endl
	      << " (Fsim time):             " << time_str(fsim_timer) << std::endl
	      << " (Dichotomy time):        " << time_str(dicho_timer) << std::endl;
  }

  return cand;
}

END_NAMESPACE_DRUID
