
/// @file global_reduction.cc
/// @brief global_reduction の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "Reducer.h"
#include "TpgNetwork.h"
#include "FFRFaultList.h"
#include "FFRDomChecker.h"
#include "DomCandMgr.h"
#include "XChecker.h"
#include "SimpleDomChecker.h"
#include "DomChecker.h"
#include "OpBase.h"
#include "ym/Timer.h"


#define DBG_OUT cerr

BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

struct Key {
  SizeType fault_id;
  SizeType ffr_id;
};

struct KeyHash {
  SizeType
  operator()(
    const Key& key
  ) const
  {
    return key.fault_id * 1023 + key.ffr_id;
  }
};

struct KeyEq {
  bool
  operator()(
    const Key& a,
    const Key& b
  ) const
  {
    return a.fault_id == b.fault_id && a.ffr_id == b.ffr_id;
  }
};

END_NONAMESPACE

// @brief 異なる FFR 間の支配故障のチェックを行う．
vector<const TpgFault*>
global_reduction(
  const TpgNetwork& network,
  const vector<const TpgFault*>& src_fault_list,
  const DomCandMgr& mgr,
  const XChecker& xc,
  const JsonValue& option
)
{
  Timer timer;
  timer.start();

  int debug = OpBase::get_debug(option);
  if ( debug > 0 ) {
    DBG_OUT << "---------------------------------------" << endl;
  }

  SizeType check1_num = 0;
  SizeType check2_num = 0;
  SizeType dom1_num = 0;
  SizeType dom2_num = 0;
  SizeType success_num = 0;

  FFRFaultList ffr_fault_list{network, src_fault_list};
  vector<bool> del_mark(network.max_fault_id(), false);
  for ( auto ffr1: ffr_fault_list.ffr_list() ) {
    // 候補の可能性のある故障のリスト
    vector<const TpgFault*> fault2_list;
    // fault2_list のマーク
    vector<bool> fault2_mark(network.max_fault_id(), false);
    // ffr1 に含まれる故障の被支配故障の候補が属するFFRを求める．
    vector<const TpgFFR*> ffr2_list;
    unordered_set<SizeType> ffr2_mark;
    // 故障番号とFFR番号のペアをキーにして故障のリストを保持する辞書
    unordered_map<Key, vector<const TpgFault*>, KeyHash, KeyEq> fault2_list_map;
    for ( auto fault1: ffr_fault_list.fault_list(ffr1) ) {
      if ( del_mark[fault1->id()] ) {
	continue;
      }
      for ( auto fault2: mgr.dom_cand_list(fault1) ) {
	if ( del_mark[fault2->id()] ) {
	  continue;
	}
	auto ffr2 = network.ffr(fault2);
	if ( ffr2 == ffr1 ) {
	  continue;
	}
	if ( !xc.check_intersect(ffr1, ffr2) ) {
	  continue;
	}
	if ( !fault2_mark[fault2->id()] ) {
	  fault2_mark[fault2->id()] = true;
	  fault2_list.push_back(fault2);
	}
	auto ffr2_id = ffr2->id();
	if ( ffr2_mark.count(ffr2_id) == 0 ) {
	  ffr2_mark.emplace(ffr2_id);
	  ffr2_list.push_back(ffr2);
	}
	auto key = Key{fault1->id(), ffr2_id};
	if ( fault2_list_map.count(key) == 0 ) {
	  fault2_list_map.emplace(key, vector<const TpgFault*>{});
	}
	fault2_list_map.at(key).push_back(fault2);
      }
    }
    if ( fault2_list.empty() ) {
      continue;
    }
    ++ dom1_num;
    SimpleDomChecker checker1{network, ffr1, fault2_list, option};
    for ( auto ffr2: ffr2_list ) {
      ++ dom2_num;
      DomChecker checker2{network, ffr1, ffr2, option};
      for ( auto fault1: ffr_fault_list.fault_list(ffr1) ) {
	if ( del_mark[fault1->id()] ) {
	  continue;
	}
	auto key = Key{fault1->id(), ffr2->id()};
	if ( fault2_list_map.count(key) == 0 ) {
	  continue;
	}
	if ( !xc.check_intersect(ffr1, ffr2) ) {
	  continue;
	}
	// fault1 の検出条件と ffr2 の根の出力の故障伝搬条件を調べる．
	++ check2_num;
	if ( !checker2.check(fault1) ) {
	  continue;
	}
	// fault1 の検出条件と fault2 の FFR 内の検出条件を調べる．
	auto& fault2_list = fault2_list_map.at(key);
	for ( auto fault2: fault2_list ) {
	  if ( del_mark[fault2->id()] ) {
	    continue;
	  }
	  ++ check1_num;
	  if ( checker1.check(fault1, fault2) ) {
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
    DBG_OUT << "after global dominance reduction:      " << fault_list.size() << endl
	    << "    # of total checkes(1):             " << check1_num << endl
	    << "    # of total checkes(2):             " << check2_num << endl
	    << "    # of total successes:              " << success_num << endl
	    << "    # of DomCheckers(1):               " << dom1_num << endl
	    << "    # of DomCheckers(2):               " << dom2_num << endl
	    << "CPU time:                              "
	    << (timer.get_time() / 1000.0) << endl;
  }

  return fault_list;
}

END_NAMESPACE_DRUID
