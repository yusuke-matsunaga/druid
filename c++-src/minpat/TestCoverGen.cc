
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
#include "ym/Timer.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// @brief オプションからデバッグフラグを取り出す．
bool
get_debug(
  const JsonValue& option
)
{
  if ( option.is_object() && option.has_key("debug") ) {
    return option.get("debug").get_bool();
  }
  return false;
}

END_NONAMESPACE

// @brief 各故障のテストカバーを生成する．
vector<TestCover>
TestCoverGen::run(
  const FaultInfoMgr& mgr,
  const JsonValue& option
)
{
  Timer timer;
  timer.start();

  bool debug = get_debug(option);

  auto& network = mgr.network();
  auto fault_list = mgr.active_fault_list();
  FFRFaultList ffr_fault_list{network, fault_list};

  vector<TestCover> cover_list;
  cover_list.reserve(fault_list.size());
  SizeType nc = 0;
  for ( auto ffr: ffr_fault_list.ffr_list() ) {
    ExCubeGen gen{network, ffr, option};
    for ( auto fault: ffr_fault_list.fault_list(ffr) ) {
      auto& finfo = mgr.fault_info(fault);
      if ( finfo.is_deleted() ) {
	continue;
      }
      auto& mand_cond = finfo.mandatory_condition();
      auto& suff_cond = finfo.sufficient_condition();
      auto testcover = gen.run(fault, mand_cond, suff_cond);
      cover_list.push_back(testcover);
      nc += testcover.cube_list().size();
    }
  }

  timer.stop();
  if ( debug ) {
    cerr << "Total # of cubes: " << nc << endl
	 << "CPU time:         " << timer.get_time() << endl;
  }

  return cover_list;
}

END_NAMESPACE_DRUID
