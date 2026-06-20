
/// @file FFRAnalyzeTest.cc
/// @brief FFRAnalyze のテストプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include <gtest/gtest.h>
#include "FFRAnalyze.h"
#include "dtpg/NaiveDtpgEngine.h"
#include "dtpg/NaiveDualEngine.h"
#include "types/TpgNetwork.h"
#include "types/TpgFault.h"
#include "types/TpgFaultList.h"


BEGIN_NAMESPACE_DRUID

class FFRAnalyzeTest:
public ::testing::Test
{
public:

  void
  check(
    const TpgNetwork& network
  );

};

void
FFRAnalyzeTest::check(
  const TpgNetwork& network
)
{
  auto fault_list = network.rep_fault_list();
  auto ffr_fault_list_array = fault_list.ffr_split();
  for ( auto ffr: network.ffr_list() ) {
    auto fault_list1 = ffr_fault_list_array[ffr.id()];
    auto fault_info = FaultInfo(fault_list1);
    FFRAnalyze::run(ffr, fault_list1, fault_info);

    auto nf = fault_list1.size();
    std::vector<bool> mark(nf, true);
    // 検出可能のチェック
    for ( auto fault: fault_list1 ) {
      auto status = fault_info.status(fault);
      NaiveDtpgEngine engine(fault);
      auto res = engine.solve();
      if ( res == SatBool3::True ) {
	EXPECT_EQ( FaultStatus::Detected, status )
	  << fault.str();
      }
      else if ( res == SatBool3::False ) {
	EXPECT_EQ( FaultStatus::Untestable, status )
	  << fault.str();
	mark[fault.id()] = false;
      }
    }

    // 支配関係のチェック
    for ( auto fault1: fault_list1 ) {
      if ( fault_info.is_dominated(fault1) ) {
	auto fault2 = fault_info.dominator(fault1);
	NaiveDualEngine engine(fault1, fault2);
	auto res = engine.solve(false, true);
	std::ostringstream buf;
	buf << fault1.str() << ", " << fault2.str();
	EXPECT_EQ( SatBool3::False, res ) << buf.str();
      }
      else {
	for ( auto fault2: fault_list1 ) {
	  if ( fault2 == fault1 ) {
	    continue;
	  }
	  if ( !fault_info.is_rep(fault2) ) {
	    continue;
	  }
	  NaiveDualEngine engine(fault1, fault2);
	  auto res = engine.solve(false, true);
	  std::ostringstream buf;
	  buf << fault1.str() << ", " << fault2.str();
	  EXPECT_EQ( SatBool3::True, res ) << buf.str();
	}
      }
    }
  }
}


TEST_F(FFRAnalyzeTest, test_s27)
{
  auto data_dir = std::filesystem::path{TESTDATA_DIR};
  auto filename = data_dir / "s27.blif";
  auto network = TpgNetwork::read_blif(filename, FaultType::StuckAt);

  check(network);
}


TEST_F(FFRAnalyzeTest, test_s298)
{
  auto data_dir = std::filesystem::path{TESTDATA_DIR};
  auto filename = data_dir / "s298.blif";
  auto network = TpgNetwork::read_blif(filename, FaultType::StuckAt);

  check(network);
}

END_NAMESPACE_DRUID
