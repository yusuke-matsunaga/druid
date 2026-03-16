
/// @file DtpgMgr_Test.cc
/// @brief DtpgMgr のテストプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include <gtest/gtest.h>
#include "dtpg/DtpgMgr.h"
#include "dtpg/SuffCond.h"
#include "types/TpgNetwork.h"
#include "types/TpgFault.h"
#include "types/TpgFaultList.h"
#include "types/TestVector.h"


BEGIN_NAMESPACE_DRUID

TEST(DtpgMgrTest, test1)
{
  auto data_dir = std::filesystem::path{TESTDATA_DIR};
  auto filename = data_dir / "s1196.blif";
  auto network = TpgNetwork::read_blif(filename, FaultType::StuckAt);
  auto fault_list = network.rep_fault_list();

  for ( auto fault: fault_list ) {
    if ( fault.str() != "G#97:I0:SA1" ) {
      continue;
    }
    auto results = DtpgMgr::run(TpgFaultList({fault}));
    ASSERT_EQ( FaultStatus::Detected, results.status(fault) );
    auto cond = results.cond(fault);
    {
      std::cout << "assign_list = ";
      for ( auto nv: cond.main_cond() ) {
	std::cout << " " << nv;
      }
      std::cout << std::endl;
    }
  }
}

END_NAMESPACE_DRUID
