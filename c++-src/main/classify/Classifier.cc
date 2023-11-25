
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


  SizeType max_fid = 0;
  for ( auto f: fault_list ) {
    max_fid = std::max(max_fid, f.id());
  }
  ++ max_fid;

  // 最初はすべての故障が一つのグループとなっている．
  cout << "# of testvectors: " << tv_list.size() << endl;
  SizeType i = 0;
  vector<vector<SizeType>> sig_array(max_fid);
  for ( auto tv: tv_list ) {
    cout << "#" << i << endl; ++ i;
    auto fault_list1 = fsim.sppfp(tv);
    unordered_map<DiffBits, vector<SizeType>> det_fault_group;
    unordered_set<SizeType> fault_mark;
    for ( auto fault: fault_list1 ) {
      fault_mark.emplace(fault.id());
      auto dbits = fsim.sppfp_diffbits(fault);
      if ( det_fault_group.count(dbits) == 0 ) {
	det_fault_group.emplace(dbits, vector<SizeType>{});
      }
      det_fault_group.at(dbits).push_back(fault.id());
    }

    // 未検出の故障のシグネチャは 0
    SizeType sig = 0;
    for ( auto fault: fault_list ) {
      auto fid = fault.id();
      if ( fault_mark.count(fid) == 0 ) {
	sig_array[fid].push_back(sig);
      }
    }
    for ( auto& p: det_fault_group ) {
      ++ sig;
      for ( auto fid: p.second ) {
	sig_array[fid].push_back(sig);
      }
    }
  }

  return vector<vector<TpgFault>>{};
}

END_NAMESPACE_DRUID
