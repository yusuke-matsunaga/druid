
/// @file ffr_reduction.cc
/// @brief ffr_reduction の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "Reducer.h"
#include "TpgNetwork.h"
#include "FFRFaultList.h"
#include "FFRDomChecker.h"
#include "DomCandMgr.h"
#include "OpBase.h"
#include "ym/Timer.h"


BEGIN_NAMESPACE_DRUID

vector<const TpgFault*>
ffr_reduction(
  const TpgNetwork& network,
  const vector<const TpgFault*>& src_fault_list,
  const DomCandMgr& mgr,
  const JsonValue& option
)
{
  Timer timer;
  timer.start();

  int debug = OpBase::get_debug(option);
  if ( debug > 0 ) {
    cerr << "---------------------------------------" << endl;
    cerr << "# of initial faults:                   "
	 << src_fault_list.size() << endl;
  }

  SizeType check_num = 0;
  SizeType dom_num = 0;
  SizeType success_num = 0;

  FFRFaultList ffr_fault_list{network, src_fault_list};
  vector<bool> del_mark(network.max_fault_id(), false);
  for ( auto ffr: ffr_fault_list.ffr_list() ) {
    FFRDomChecker checker{network, ffr, option};
    ++ dom_num;

    // 支配関係を調べ，代表故障のみを残す．
    // FFR 単位の故障リスト
    for ( auto fault1: ffr_fault_list.fault_list(ffr) ) {
      if ( del_mark[fault1->id()] ) {
	continue;
      }
      auto fault1_root = fault1->ffr_root();
      vector<const TpgFault*> fault2_list;
      for ( auto fault2: mgr.dom_cand_list(fault1) ) {
	if ( fault2->ffr_root() == fault1_root &&
	     !del_mark[fault2->id()] ) {
	  ++ check_num;
	  if ( checker.check(fault1, fault2) ) {
	    del_mark[fault2->id()] = true;
	    ++ success_num;
	  }
	}
      }
    }
  }
  vector<const TpgFault*> fault_list;
  fault_list.reserve(src_fault_list.size() - success_num);
  for ( auto fault: src_fault_list ) {
    if ( !del_mark[fault->id()] ) {
      fault_list.push_back(fault);
    }
  }
  timer.stop();

  if ( debug > 0 ) {
    cerr << "after FFR dominance reduction:         " << fault_list.size() << endl;
    cerr << "    # of total checkes:                " << check_num << endl
	 << "    # of total successes:              " << success_num << endl
	 << "    # of FFRDomCheckers:               " << dom_num << endl
	 << "CPU time:                              "
	 << (timer.get_time() / 1000.0) << endl;
  }

  return fault_list;
}

END_NAMESPACE_DRUID
