
/// @file NaiveReduce.cc
/// @brief NaiveReduce の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "NaiveReduce.h"
#include "NaiveMgr.h"
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

std::vector<PackedVal>
make_dpat_array(
  const FsimResults& res
)
{
  auto ntv = res.tv_num();
  std::vector<PackedVal> dpat_array(res.network().max_fault_id(), 0);
  for ( SizeType i = 0; i < ntv; ++ i ) {
    PackedVal bit = 1ULL << i;
    for ( auto fault: res.fault_list(i) ) {
      dpat_array[fault.id()] |= bit;
    }
  }
  return dpat_array;
}

// count_array に従って比較を行うファンクタクラス
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


// @brief 故障グループの細分化を行ってから支配関係を調べる．
void
NaiveReduce::run(
  FaultInfo& fault_info,
  const ConfigParam& option
)
{
  SizeType NO_CHANGE_LIMIT = option.get_int_elem("no_change_limit", 1000);
  SizeType BATCH_SIZE = std::min(64, option.get_int_elem("batch_size", 64));
  SizeType TIME_LIMIT = option.get_int_elem("time_limit", 0);
  auto verbose = option.get_bool_elem("verbose", false);

  Timer timer;
  timer.start();

  SizeType check_count = 0;
  SizeType succ_count = 0;
  auto fault_list = fault_info.rep_fault_list();

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

  Timer timer1;
  timer1.start();

  Timer fsim_timer;
  SizeType tv_count = 0;
  std::mt19937 randgen;
  SizeType no_change = 0;
  NaiveMgr naivemgr(fault_list, network.max_fault_id());
  while ( no_change < NO_CHANGE_LIMIT ) {
    std::vector<TestVector> tv_list(BATCH_SIZE);
    for ( SizeType base = 0; base < BATCH_SIZE; ++ base ) {
      if ( heap.empty() ) {
	// ランダムパタンを作る．
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
	// X の部分はランダムに埋める．
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

    auto dpat_array = make_dpat_array(res);

    auto change = naivemgr.add(dpat_array);
    if ( change ) {
      no_change = 0;
    }
    else {
      no_change += BATCH_SIZE;
    }
  }
  timer1.stop();
  if ( verbose ) {
    std::cout << "Total # of patterns:    " << std::setw(8) << std::right << tv_count << std::endl
	      << "No Change Limit:        " << std::setw(8) << std::right << NO_CHANGE_LIMIT << std::endl;
  }

  Timer timer_phase1;
  timer_phase1.start();
  for ( auto fault1: fault_list ) {
    if ( !fault_info.is_rep(fault1) ) {
      continue;
    }
    for ( auto fault2: naivemgr.eqcand_list(fault1) ) {
      if ( !fault_info.is_rep(fault2) ) {
	continue;
      }
      NaiveDualEngine engine(fault1, fault2, option);
      ++ check_count;
      auto res10 = engine.solve(true, false, TIME_LIMIT);
      auto res01 = engine.solve(false, true, TIME_LIMIT);
      if ( res10 == SatBool3::False && res01 == SatBool3::False ) {
	// fault1 と fault2 は等価
	fault_info.set_rep(fault2, fault1);
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
    if ( !fault_info.is_rep(fault1) ) {
      continue;
    }
    for ( auto fault2: naivemgr.domcand_list(fault1) ) {
      if ( !fault_info.is_rep(fault2) ) {
	continue;
      }
      NaiveDualEngine engine(fault1, fault2, option);
      ++ check_count;
      auto res10 = engine.solve(true, false, TIME_LIMIT);
      if ( res10 == SatBool3::False ) {
	// fault2 は fault1 に支配されている．
	fault_info.set_dominator(fault2, fault1);
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
	      << "Screening Time:           " << time_str(timer1) << std::endl
	      << " (Fsim Time):             " << time_str(fsim_timer) << std::endl
	      << "Phase1 Time:              " << time_str(timer_phase1) << std::endl
	      << "Phase2 Time:              " << time_str(timer_phase2) << std::endl
	      << "Total CPU Time:           " << time_str(timer) << std::endl;
  }
}

END_NAMESPACE_DRUID
