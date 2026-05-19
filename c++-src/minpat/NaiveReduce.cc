
/// @file NaiveReduce.cc
/// @brief NaiveReduce の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "NaiveReduce.h"
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

END_NONAMESPACE

// @brief 故障グループの細分化を行ってから支配関係を調べる．
void
NaiveReduce::run(
  FaultInfo& fault_info,
  const ConfigParam& option
)
{
  SizeType NO_CHANGE_LIMIT = option.get_int_elem("no_change_limit", 1000);
  SizeType BATCH_SIZE = std::min(32, option.get_int_elem("batch_size", 16));
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

  // 支配故障の候補集合を表すビットベクタ
  auto max_fid = network.max_fault_id();
  std::vector<bool> dom_cand_array(max_fid * max_fid, false);
  std::vector<bool> compat_array(max_fid * max_fid, false);
  for ( auto fault1: fault_list ) {
    for ( auto fault2: fault_list ) {
      dom_cand_array[fault1.id() * max_fid + fault2.id()] = true;
    }
  }

  Timer timer1;
  timer1.start();

  SizeType tv_count = 0;
  std::mt19937 randgen;
  SizeType no_change = 0;
  while ( no_change < NO_CHANGE_LIMIT ) {
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
    auto res = fsim.run_multi(tv_list, true);
    tv_count += BATCH_SIZE;

    auto ntv = res.tv_num();
    bool change = false;
    for ( SizeType i = 0; i < ntv; ++ i ) {
      std::vector<bool> mark(max_fid, false);
      for ( auto fault: res.fault_list(i) ) {
	mark[fault.id()] = true;
      }
#if 0
      auto n1 = res.fault_num(i);
      for ( SizeType i1 = 0; i1 < n1 - 1; ++ i1 ) {
	auto fault1 = res.fault(i, i1);
	for ( SizeType i2 = i1 + 1; i2 < n1; ++ i2 ) {
	  auto fault2 = res.fault(i, i2);
	  if ( !compat_array[fault1.id() * max_fid + fault2.id()] ) {
	    compat_array[fault1.id() * max_fid + fault2.id()] = true;
	    compat_array[fault2.id() * max_fid + fault1.id()] = true;
	    change = true;
	  }
	}
      }
#endif
      for ( auto fault1: res.fault_list(i) ) {
	for ( auto fault2: fault_list ) {
	  if ( fault1 == fault2 ) {
	    continue;
	  }
	  if ( dom_cand_array[fault1.id() * max_fid + fault2.id()] ) {
	    if ( !mark[fault2.id()] ) {
	      // fault1 が fault2 を支配することはない．
	      dom_cand_array[fault1.id() * max_fid + fault2.id()] = false;
	      change = true;
	    }
	  }
	}
      }
    }
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
	      << "CPU Time:               " << time_str(timer1) << std::endl;
  }

  Timer timer2;
  timer2.start();
  auto nf = fault_list.size();
  for ( SizeType i1 = 0; i1 < nf - 1; ++ i1 ) {
    auto fault1 = fault_list[i1];
    if ( !fault_info.is_rep(fault1) ) {
      continue;
    }
    for ( SizeType i2 = i1 + 1; i2 < nf; ++ i2 ) {
      auto fault2 = fault_list[i2];
      if ( !fault_info.is_rep(fault2) ) {
	continue;
      }
      if ( !dom_cand_array[fault1.id() * max_fid + fault2.id()] &&
	   !dom_cand_array[fault2.id() * max_fid + fault1.id()] ) {
	continue;
      }
      NaiveDualEngine engine(fault1, fault2, option);
      auto res11 = engine.solve(true, true);
      ++ check_count;
      if ( res11 != SatBool3::True ) {
	// fault1 と fault2 は排他的
	continue;
      }
      SatBool3 res01 = SatBool3::True;
      if ( dom_cand_array[fault1.id() * max_fid + fault2.id()] ) {
	res01 = engine.solve(false, true);
      }
      SatBool3 res10 = SatBool3::True;
      if ( dom_cand_array[fault2.id() * max_fid + fault1.id()] ) {
	res10 = engine.solve(true, false);
      }
      if ( res01 == SatBool3::False ) {
	if ( res10 == SatBool3::False ) {
	  // fault1 と fault2 は等価故障
	  // fault1.id() < fault2.id() のはず
	  fault_info.set_rep(fault2, fault1);
	  ++ succ_count;
	  continue;
	}
	else if ( res10 == SatBool3::True ) {
	  // fault1 は fault2 に支配されている．
	  fault_info.set_dominator(fault1, fault2);
	  ++ succ_count;
	  break;
	}
      }
      else if ( res01 == SatBool3::True ) {
	if ( res10 == SatBool3::False ) {
	  // fault2 は fault1 に支配されている．
	  fault_info.set_dominator(fault2, fault1);
	  ++ succ_count;
	  continue;
	}
      }
    }
  }
  timer2.stop();

  timer.stop();
  if ( verbose ) {
    std::cout << "Dominance check end:      " << std::endl
	      << "Total checks:             " << std::setw(8) << std::right << check_count << std::endl
	      << "Total succeeds:           " << std::setw(8) << std::right << succ_count << std::endl
	      << "Phase2 Time:              " << time_str(timer2) << std::endl
	      << "Total CPU Time:           " << time_str(timer) << std::endl;
  }
}

END_NAMESPACE_DRUID
