
/// @file CondGenMgr.cc
/// @brief CondGenMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "CondGenMgr.h"
#include "CondGen.h"
#include "TpgNetwork.h"
#include "TpgFault.h"
#include "FFRFaultList.h"
#include "OpBase.h"
#include "ym/Timer.h"


#define DBG_OUT cerr

BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス CondGenMgr
//////////////////////////////////////////////////////////////////////

// @brief 故障検出条件を求める．
void
CondGenMgr::root_cond(
  const TpgNetwork& network,
  SizeType limit,
  RootCondCallback cbfunc,
  const JsonValue& option
)
{
  int debug = OpBase::get_debug(option);

  for ( auto ffr: network.ffr_list() ) {
    if ( debug > 1 ) {
      DBG_OUT << "FFR#" << ffr->id()
	      << " / "
	      << network.ffr_num() << endl;
    }

    CondGen gen{network, ffr, option};
    Timer timer;
    timer.start();
    SizeType count = 0;
    auto cond = gen.root_cond(limit, count);
    timer.stop();
    auto time = timer.get_time();
    cbfunc(ffr, cond, count, time);
  }
}

// @brief 故障検出条件を求める．
void
CondGenMgr::fault_cond(
  const TpgNetwork& network,
  const vector<const TpgFault*>& fault_list,
  SizeType limit,
  FaultCondCallback cbfunc,
  const JsonValue& option
)
{
  int debug = OpBase::get_debug(option);

  FFRFaultList ffr_fault_list{network, fault_list};
  for ( auto ffr: ffr_fault_list.ffr_list() ) {
    if ( debug > 1 ) {
      DBG_OUT << "FFR#" << ffr->id()
	      << " [" << ffr_fault_list.fault_list(ffr).size() << "]"
	      << " / "
	      << ffr_fault_list.ffr_list().size() << endl;
    }

    CondGen gen{network, ffr, option};
    for ( auto fault: ffr_fault_list.fault_list(ffr) ) {
      Timer timer;
      timer.start();
      SizeType count = 0;
      auto cond = gen.fault_cond(fault, limit, count);
      timer.stop();
      auto time = timer.get_time();
      cbfunc(fault, cond, count, time);
    }
  }
}

END_NAMESPACE_DRUID
