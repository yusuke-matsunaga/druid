
/// @file DtpgEngineTest2.cc
/// @brief DtpgEngine のテストプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include <gtest/gtest.h>
#include "minpat/FaultAnalyze.h"
#include "dtpg/NaiveDtpgEngine.h"
#include "dtpg/NaiveDualEngine.h"
#include "dtpg/SuffCond.h"
#include "types/TpgNetwork.h"
#include "types/TpgFault.h"
#include "types/TpgFaultList.h"
#include "types/TestVector.h"


BEGIN_NAMESPACE_DRUID

TEST(FaultAnalyzeTest2, test1)
{
  auto data_dir = std::filesystem::path{TESTDATA_DIR};
  auto filename = data_dir / "s298.blif";
  auto network = TpgNetwork::read_blif(filename, FaultType::StuckAt);
  auto fault_list = network.rep_fault_list();
  TpgFault fault1;
  TpgFault fault2;
  for ( auto fault: fault_list ) {
    if ( fault.str() == "G#45:I1:SA1" ) {
      fault1 = fault;
    }
    else if ( fault.str() == "G#46:I3:SA0" ) {
      fault2 = fault;
    }
  }
  auto fault_info = FaultAnalyze::run(fault_list);
  EXPECT_EQ( true, fault_info.is_rep(fault1) ) << fault1.str();
  EXPECT_EQ( false, fault_info.is_rep(fault2) ) << fault2.str();
}

END_NAMESPACE_DRUID
