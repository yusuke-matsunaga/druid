
/// @file Dichotomy.cc
/// @brief Dichotomy の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "Dichotomy.h"
#include "DiGroupMgr.h"
#include "types/TpgNetwork.h"
#include "types/TestVector.h"
#include "types/PackedVal.h"
#include "fsim/Fsim.h"
#include "dtpg/NaiveDualEngine.h"
#include "ym/HeapTree.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

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

  Timer timer;
  timer.start();

  SizeType loop_count = 0;
  while ( mgr.group_num() < fault_num && !heap.empty() ) {
    // 検出回数が最小の故障を一つ選ぶ．
    auto fid = heap.get_min();
    auto min_fault = network.fault(fid);
    // この故障を検出するテストベクタを用いて故障シミュレーションを行う．
    auto tv = fault_info.testvector(min_fault);
    auto res = fsim.sppfp(tv);
    ++ loop_count;
    auto fault_list1 = res.fault_list(0);
    // シミュレーション結果に基づいて細分化を行う．
    auto new_mgr = DiGroupMgr::dichotomy(mgr, fault_list1);
    // 検出回数を更新する．
    for ( auto fault: fault_list1 ) {
      auto fid = fault.id();
      comp.inc_count(fid);
      if ( heap.is_in(fid) ) {
	heap.update(fid);
      }
    }
    if ( new_mgr != mgr ) {
      // 細分化できたら更新する．
      std::swap(mgr, new_mgr);
      if ( debug > 1 ) {
	std::cout << "#" << loop_count << ": "
		  << mgr.group_num()
		  << std::endl;
	++ loop_count;
      }
    }
  }
  // 変化がなくなるまでランダムシミュレーションを行う．
  std::mt19937 randgen;
  SizeType no_change = 0;
  while ( mgr.group_num() < fault_num && no_change < NO_CHANGE_LIMIT ) {
    auto tv = TestVector(network);
    tv.set_from_random(randgen);
    auto res = fsim.sppfp(tv);
    ++ loop_count;
    auto fault_list1 = res.fault_list(0);
    // シミュレーション結果に基づいて細分化を行う．
    auto new_mgr = DiGroupMgr::dichotomy(mgr, fault_list1);
    if ( new_mgr != mgr ) {
      // 細分化できたら更新する．
      std::swap(mgr, new_mgr);
      if ( debug > 1 ) {
	std::cout << "#" << loop_count << ": "
		  << mgr.group_num()
		  << std::endl;
	++ loop_count;
      }
    }
    else {
      ++ no_change;
    }
  }

  timer.stop();
  if ( verbose ) {
    std::cout << "# of faults:            " << std::setw(8) << std::right << fault_list.size() << std::endl
	      << "# of Groups:            " << std::setw(8) << std::right << mgr.group_num() << std::endl
	      << "Total # of simulations: " << std::setw(8) << std::right << loop_count << std::endl
	      << "CPU Time:               " << std::setw(8) << std::right
	      << timer.get_time() << "ms" << std::endl;
  }

  timer.reset();
  timer.start();

  // DiGroup 内の故障が等価故障かどうか調べる．
  SizeType check_count = 0;
  SizeType succ_count = 0;
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
	auto res01 = engine.solve(false, true);
	auto res10 = engine.solve(true, false);
	if ( res01 == SatBool3::False ) {
	  if ( res10 == SatBool3::False ) {
	    // fault1 と fault2 は等価故障
	    if ( fault1.id() < fault2.id() ) {
	      fault_info.set_rep(fault2, fault1);
	      ++ succ_count;
	      if ( debug > 0 ) {
		std::cout << fault1.str() << " and " << fault2.str()
			  << " are equivalent" << std::endl;
	      }
	    }
	    else {
	      fault_info.set_rep(fault1, fault2);
	      ++ succ_count;
	      if ( debug > 0 ) {
		std::cout << fault2.str() << " and " << fault1.str()
			  << " are equivalent" << std::endl;
	      }
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

  timer.stop();
  if ( verbose ) {
    auto rep_num = fault_info.rep_fault_list().size();
    std::cout << "Equivalence check end:    " << std::endl
	      << "Total checks:             " << std::setw(8) << std::right << check_count << std::endl
	      << "Total succeeds:           " << std::setw(8) << std::right << succ_count << std::endl
	      << "# of faults:              " << std::setw(8) << std::right << rep_num << std::endl
	      << "CPU TIme:                 " << std::setw(8) << std::right << timer.get_time() << "ms" << std::endl;
  }
  timer.reset();
  timer.start();

  // 残ったグループ内の故障は無関係となる．
  check_count = 0;
  succ_count = 0;
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
	std::cout << "  Group#" << group1->id() << ":";
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
	  auto res = engine.solve(true, false);
	  ++ check_count;
	  if ( res == SatBool3::False ) {
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

  timer.stop();
  if ( verbose ) {
    std::cout << "Dominance check end:      " << std::endl
	      << "Total checks:             " << std::setw(8) << std::right << check_count << std::endl
	      << "Total succeeds:           " << std::setw(8) << std::right << succ_count << std::endl
	      << "CPU TIme:                 " << std::setw(8) << std::right << timer.get_time()
	      << "ms" << std::endl;
  }
}

END_NAMESPACE_DRUID
