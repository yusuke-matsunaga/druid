
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

  SizeType loop_count = 0;
  while ( mgr.group_num() < fault_num && !heap.empty() ) {
    // 検出回数が最小の故障を一つ選ぶ．
#if 0
    // このやり方は常に O(fault_num) かかるので効率的ではない．
    TpgFault min_fault;
    SizeType min_count = 0;
    bool first = true;
    for ( auto fault: fault_list ) {
      if ( mark[fault.id()] ) {
	continue;
      }
      auto count = count_array[fault.id()];
      if ( first ) {
	min_count = count;
	min_fault = fault;
	first = false;
      }
      else if ( min_count > count ) {
	min_count = count;
	min_fault = fault;
      }
    }
    if ( first ) {
      break;
    }
#else
    auto fid = heap.get_min();
    auto min_fault = network.fault(fid);
#endif
    // この故障を検出するテストベクタを用いて故障シミュレーションを行う．
    auto tv = fault_info.testvector(min_fault);
    auto res = fsim.sppfp(tv);
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
      {
	std::cout << "#" << loop_count << ": "
		  << mgr.group_num()
		  << std::endl;
	++ loop_count;
      }
    }
  }

  {
    std::cout << "# of faults:     " << fault_list.size() << std::endl
	      << "# of Groups:     " << mgr.group_num() << std::endl;
  }

  // DiGroup 内の故障が等価故障かどうか調べる．
  SizeType check_count = 0;
  for ( auto group: mgr.group_list() ) {
    auto& fault_list = group->fault_list();
    auto nf = fault_list.size();
    if ( nf == 1 ) {
      continue;
    }
    if ( 0 ) {
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
	NaiveDualEngine engine(fault1, fault2, option);
	++ check_count;
	auto res01 = engine.solve(false, true);
	auto res10 = engine.solve(true, false);
	if ( res01 == SatBool3::False ) {
	  if ( res10 == SatBool3::False ) {
	    // fault1 と fault2 は等価故障
	    if ( fault1.id() < fault2.id() ) {
	      fault_info.set_rep(fault2, fault1);
	      {
		std::cout << fault1.str() << " and " << fault2.str()
			  << " are equivalent" << std::endl;
	      }
	    }
	    else {
	      fault_info.set_rep(fault1, fault2);
	      {
		std::cout << fault2.str() << " and " << fault1.str()
			  << " are equivalent" << std::endl;
	      }
	    }
	  }
	  else if ( res10 == SatBool3::True ) {
	    // fault1 は fault2 に支配されている．
	    fault_info.set_dominator(fault1, fault2);
	    {
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
	    {
	      std::cout << fault2.str() << " is dominated by "
			<< fault1.str() << std::endl;
	    }
	    continue;
	  }
	}
      }
    }
  }
  // 残ったグループ内の故障は無関係となる．
  for ( auto group1: mgr.group_list() ) {
    auto fault_list1 = rep_fault_list(group1->fault_list(), fault_info);
    if ( fault_list1.empty() ) {
      continue;
    }
    if ( 0 ) {
      std::cout << "Group#" << group1->id() << ":";
      for ( auto fault: fault_list1 ) {
	std::cout << " " << fault.str();
      }
      std::cout << std::endl;
    }
    auto nf1 = fault_list1.size();
    for ( auto group2: group1->dominate_list() ) {
      auto fault_list2 = rep_fault_list(group2->fault_list(), fault_info);
      if ( fault_list2.empty() ) {
	continue;
      }
      if ( 0 ) {
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
	  NaiveDualEngine engine(fault1, fault2, option);
	  auto res = engine.solve(true, false);
	  ++ check_count;
	  if ( res == SatBool3::False ) {
	    fault_info.set_dominator(fault2, fault1);
	    {
	      std::cout << fault2.str() << " is dominated by "
			<< fault1.str() << std::endl;
	    }
	    continue;
	  }
	}
      }
    }
  }
#if 0
  std::mt19937 randgen;

  // シミュレーションの結果を用いて二分法を行う．
  SizeType count = 0;
  while ( mgr.group_num() < fault_list.size() ) {
    // ランダムパタンを作る．
    std::vector<TestVector> tv_list;
    tv_list.reserve(PV_BITLEN);
    for ( SizeType i = 0; i < PV_BITLEN; ++ i ) {
      auto tv = TestVector(network);
      tv.set_from_random(randgen);
      tv_list.push_back(tv);
    }

    // そのパタンを使って故障シミュレーションを行う．
    auto res = fsim.ppsfp(tv_list);
    bool changed = false;
    for ( SizeType i = 0; i < PV_BITLEN; ++ i ) {
      auto new_mgr = DiGroupMgr::dichotomy(mgr, res.fault_list(i));
      if ( new_mgr != mgr ) {
	// 細分化できたら更新する．
	std::swap(mgr, new_mgr);
	changed = true;
	{
	  std::cout << "#" << count << ": "
		    << mgr.group_num()
		    << std::endl;
	  ++ count;
	}
      }
    }
    if ( !changed ) {
      break;
    }
  }

  // 未検出の故障を対象に DTPG を行う．
  auto undet_group = mgr.undet_group();
  if ( undet_group != nullptr ) {
    TpgFaultList untest_list;
    TpgFaultList abort_list;
    auto fault_list = undet_group->fault_list();
    {
      std::cout << "# of undet faults: " << fault_list.size() << std::endl;
    }
    std::vector<TestVector> tv_list;
    tv_list.reserve(fault_list.size());
    auto dtpg_option = option.get_param("dtpg");
    auto res = DtpgMgr::run(fault_list, dtpg_option);
    for ( auto fault: fault_list ) {
      auto status = res.status(fault);
      switch ( status ) {
      case FaultStatus::Detected:
      {
	auto tv = res.testvector(fault);
	tv.fix_x_from_random(randgen);
	tv_list.push_back(tv);
      }
      break;
      case FaultStatus::Untestable:
	untest_list.push_back(fault);
	break;
      case FaultStatus::Undetected:
	abort_list.push_back(fault);
	break;
      }
    }
    Fsim fsim(fault_list, fsim_option);
    for ( auto& tv: tv_list ) {
      auto res = fsim.sppfp(tv);
      auto new_mgr = DiGroupMgr::dichotomy(mgr, res.fault_list(0));
      if ( new_mgr != mgr ) {
	std::swap(mgr, new_mgr);
      }
    }
  }

  return mgr;
#endif
}

END_NAMESPACE_DRUID
