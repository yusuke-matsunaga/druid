
/// @file classify.cc
/// @brief classify の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "Fsim.h"
#include "TestVector.h"


BEGIN_NAMESPACE_DRUID

// @brief 故障を分類する．
vector<vector<TpgFault>>
classify(
  const TpgNetwork& network,
  const vector<TpgFault>& fault_list,
  const vector<TestVector>& tv_list
)
{
  Fsim fsim;

  bool has_previous_state = tv_list.front().has_aux_input();
  fsim.initialize(network, has_previous_state, false);
  fsim.set_fault_list(fault_list);

  // 最初はすべての故障が一つのグループとなっている．
  vector<vector<TpgFault>> fg_list{fault_list};
  cout << "# of testvectors: " << tv_list.size() << endl;
  SizeType i = 0;
  for ( auto tv: tv_list ) {
    auto fault_list1 = fsim.sppfp(tv);
    unordered_set<SizeType> fault_mark;
    for ( auto fault: fault_list1 ) {
      fault_mark.emplace(fault.id());
    }
    vector<vector<TpgFault>> new_fg_list;
    for ( auto& fg: fg_list ) {
      vector<TpgFault> undet_list;
      unordered_map<DiffBits, vector<TpgFault>> det_fault_group;
      // fg に含まれる故障を今回のシミュレーションの結果で分類する．
      for ( auto fault: fg ) {
	if ( fault_mark.count(fault.id()) > 0 ) {
	  auto dbits = fsim.sppfp_diffbits(fault);
	  if ( det_fault_group.count(dbits) == 0 ) {
	    det_fault_group.emplace(dbits, vector<TpgFault>{});
	  }
	  det_fault_group.at(dbits).push_back(fault);
	}
	else {
	  undet_list.push_back(fault);
	}
      }
      if ( undet_list.size() >= 2 ) {
	new_fg_list.push_back(undet_list);
      }
      else if ( undet_list.size() == 1 ) {
	auto fault = undet_list.front();
	fsim.set_skip(fault);
      }
      for ( auto& p: det_fault_group ) {
	auto& fault_list1 = p.second;
	if ( fault_list1.size() >= 2 ) {
	  new_fg_list.push_back(fault_list1);
	}
	else {
	  // fault_list1.size() == 1
	  auto fault = fault_list1.front();
	  fsim.set_skip(fault);
	}
      }
    }
    if ( new_fg_list.empty() ) {
      break;
    }
    std::swap(fg_list, new_fg_list);
  }

  return fg_list;
}

END_NAMESPACE_DRUID
