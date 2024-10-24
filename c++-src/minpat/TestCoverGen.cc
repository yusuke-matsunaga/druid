
/// @file TestCoverGen.cc
/// @brief TestCoverGen の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "TestCoverGen.h"
#include "TestCover.h"
#include "FaultInfoMgr.h"
#include "FFRFaultList.h"
#include "ExCubeGen.h"
#include "OpBase.h"
#include "ym/Bdd.h"
#include "ym/BddMgr.h"
#include "ym/Timer.h"


#define DBG_OUT cerr

BEGIN_NAMESPACE_DRUID

// @brief 各故障のテストカバーを生成する．
vector<TestCover>
TestCoverGen::run(
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
  SizeType total_cube_num = 0;
  SizeType total_literal_num = 0;
  SizeType total_literal_num2 = 0;
  SizeType total_literal_num3 = 0;
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
    ExCubeGen gen{network, ffr, option};
    vector<Bdd> bdd_list;
    for ( auto fault: ffr_fault_list.fault_list(ffr) ) {
      auto testcover = gen.run(fault);
      cover_list.push_back(testcover);
      auto nc = testcover.cube_num();
      if ( nc > 1 ) {
	SizeType nl = testcover.common_cube().size() * 2;
	for ( auto cube: testcover.cube_list() ) {
	  nl += cube.size() * 3 + 1;
	}
	total_cube_num += nc;
	total_literal_num += nl;
#if 1
	cout << " " << testcover.cube_num()
	     << " | " << testcover.literal_num() << endl;
	BddMgr mgr;
	auto bdd = gen.make_bdd(mgr, testcover);
	//bdd_list.push_back(bdd);
	auto n = bdd.size();
	auto nl2 = n * 3;
	total_literal_num2 += nl2;
	cout << " " << n << endl;
#endif
      }
      if ( debug > 1 ) {
	DBG_OUT << "  " << testcover.cube_num()
		<< " | " << testcover.literal_num() << endl;
      }
    }
    //total_literal_num3 += gen.bddmgr().bdd_size(bdd_list) * 3;
    timer.stop();
    auto time = timer.get_time();
    if ( max_time < time ) {
      max_time = time;
      max_ffr = ffr;
#if 0
      cout << "max updated: FFR#" << max_ffr->id()
	   << " CPU Time: " << max_time << endl;
#endif
    }
  }

  timer.stop();
  if ( debug > 0 ) {
    DBG_OUT << "Total # of cubes:    " << total_cube_num << endl
	    << "Total # of literals: " << total_literal_num << endl
	    << "CPU time:            "
	    << (timer.get_time() / 1000.0) << endl;
  }
  cout << "Total # of cubes:          " << total_cube_num << endl
       << "Total # of literals:       " << total_literal_num << endl
       << "Total # of literals(BDD):  " << total_literal_num2 << endl
       << "Total # of literals(SBDD): " << total_literal_num3 << endl;

  return cover_list;
}

// @brief 各故障のテストカバーを生成する．
vector<TestCover>
TestCoverGen::run(
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
#if 0
      cout << "max updated: FFR#" << max_ffr->id()
	   << " CPU Time: " << max_time << endl;
#endif
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
