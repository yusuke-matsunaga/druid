
/// @file TestExprGen.cc
/// @brief TestExprGen の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "TestExprGen.h"
#include "TestCover.h"
#include "FaultInfoMgr.h"
#include "FFRFaultList.h"
#include "ExCubeGen.h"
#include "ExprGen.h"
#include "OpBase.h"
#include "ym/Timer.h"


#define DBG_OUT cerr

BEGIN_NAMESPACE_DRUID

// @brief 各故障のテストカバーを生成する．
SizeType
TestExprGen::run(
  const TpgNetwork& network,
  const vector<const TpgFault*>& fault_list,
  const JsonValue& option
)
{
  Timer timer;
  timer.start();

  int debug = OpBase::get_debug(option);

  FFRFaultList ffr_fault_list{network, fault_list};

  vector<TestCover> cover_list;
  cover_list.reserve(fault_list.size());
  SizeType nc = 0;
  SizeType nl = 0;
  const TpgFFR* max_ffr = nullptr;
  double max_time = 0.0;
  for ( auto ffr: ffr_fault_list.ffr_list() ) {
    if ( debug > 1 ) {
      DBG_OUT << "FFR#" << ffr->id()
	      << " [" << ffr_fault_list.fault_list(ffr).size() << "]"
	      << " / "
	      << ffr_fault_list.ffr_list().size() << endl;
    }
    Timer timer;
    timer.start();
    ExprGen gen{network, ffr, option};
    for ( auto fault: ffr_fault_list.fault_list(ffr) ) {
      nc += gen.run(fault);
#if 0
      cover_list.push_back(testcover);
      nc += testcover.cube_num();
      nl += testcover.literal_num();
      if ( debug > 1 ) {
	DBG_OUT << "  " << testcover.cube_num()
		<< " | " << testcover.literal_num() << endl;
      }
#endif
    }
    timer.stop();
    auto time = timer.get_time();
    if ( max_time < time ) {
      max_time = time;
      max_ffr = ffr;
      cout << "max updated: FFR#" << max_ffr->id()
	   << " CPU Time: " << max_time << endl;
    }
  }

  timer.stop();
  if ( debug > 0 ) {
    DBG_OUT << "Total # of cubes:    " << nc << endl
	    << "Total # of literals: " << nl << endl
	    << "CPU time:            "
	    << (timer.get_time() / 1000.0) << endl;
  }

  return nc;
}

// @brief 各故障のテストカバーを生成する．
vector<TestCover>
TestExprGen::run(
  const FaultInfoMgr& finfo_mgr,
  const JsonValue& option
)
{
  Timer timer;
  timer.start();

  int debug = OpBase::get_debug(option);

  auto& network = finfo_mgr.network();
  auto& fault_list = finfo_mgr.active_fault_list();
  FFRFaultList ffr_fault_list{network, fault_list};

  vector<TestCover> cover_list;
  cover_list.reserve(fault_list.size());
  SizeType nc = 0;
  SizeType nl = 0;
  const TpgFFR* max_ffr = nullptr;
  double max_time = 0.0;
  for ( auto ffr: ffr_fault_list.ffr_list() ) {
    if ( debug > 1 ) {
      DBG_OUT << "FFR#" << ffr->id()
	      << " [" << ffr_fault_list.fault_list(ffr).size() << "]"
	      << " / "
	      << ffr_fault_list.ffr_list().size() << endl;
    }
    Timer timer;
    timer.start();
    ExCubeGen gen{network, ffr,
		  finfo_mgr.root_mandatory_condition(ffr),
		  option};
    for ( auto fault: ffr_fault_list.fault_list(ffr) ) {
      auto testcover = gen.run(fault);
      cover_list.push_back(testcover);
      nc += testcover.cube_num();
      nl += testcover.literal_num();
      if ( debug > 1 ) {
	DBG_OUT << "  " << testcover.cube_num()
		<< " | " << testcover.literal_num() << endl;
      }
    }
    timer.stop();
    auto time = timer.get_time();
    if ( max_time < time ) {
      max_time = time;
      max_ffr = ffr;
      cout << "max updated: FFR#" << max_ffr->id()
	   << " CPU Time: " << max_time << endl;
    }
  }

  timer.stop();
  if ( debug > 0 ) {
    DBG_OUT << "Total # of cubes:    " << nc << endl
	    << "Total # of literals: " << nl << endl
	    << "CPU time:            "
	    << (timer.get_time() / 1000.0) << endl;
  }

  return cover_list;
}

END_NAMESPACE_DRUID
