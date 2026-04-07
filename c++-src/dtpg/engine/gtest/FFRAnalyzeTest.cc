
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
    auto fault_info = FaultInfo(network.max_fault_id());
    FFRAnalyze::run(ffr, fault_list1, fault_info);

    auto nf = fault_list1.size();
    std::vector<bool> mark(nf, true);
    // 検出可能のチェック
    for ( SizeType i = 0; i < nf; ++ i ) {
      auto fault = fault_list1[i];
      NaiveDtpgEngine engine(fault);
      auto res = engine.solve();
      if ( res == SatBool3::True ) {
	EXPECT_EQ( FaultStatus::Detected, fault_info.fault_status(fault) )
	  << fault.str();
      }
      else if ( res == SatBool3::False ) {
	EXPECT_EQ( FaultStatus::Untestable, fault_info.fault_status(fault) )
	  << fault.str();
	mark[i] = false;
      }
    }

    // 支配関係のチェック
    for ( SizeType i1 = 0; i1 < nf - 1; ++ i1 ) {
      auto fault1 = fault_list1[i1];
      if ( !mark[i1] ) {
	continue;
      }
      for ( SizeType i2 = i1 + 1; i2 < nf; ++ i2 ) {
	auto fault2 = fault_list1[i2];
	if ( !mark[i2] ) {
	  continue;
	}
	NaiveDualEngine engine(fault1, fault2);
	auto res = engine.solve(true, true);
	std::ostringstream buf;
	buf << fault1.str() << " " << fault2.str();
	auto pair_name = buf.str();
	if ( res == SatBool3::True ) {
	  auto res1 = engine.solve(true, false);
	  if ( res1 == SatBool3::False ) {
	    EXPECT_EQ( true, fault_info.is_dominated(fault2) ) << pair_name;
	    mark[i2] = false;
	  }
	  else {
	    auto res2 = engine.solve(false, true);
	    if ( res2 == SatBool3::False ) {
	      EXPECT_EQ( true, fault_info.is_dominated(fault1) ) << pair_name;
	      mark[i1] = false;
	      break;
	    }
	  }
	}
      }
    }

    for ( SizeType i = 0; i < nf; ++ i ) {
      auto fault = fault_list1[i];
      EXPECT_EQ( mark[i], fault_info.is_rep(fault) );
    }

    // 両立関係のチェック
    for ( SizeType i1 = 0; i1 < nf - 1; ++ i1 ) {
      auto fault1 = fault_list1[i1];
      if ( !fault_info.is_rep(fault1) ) {
	continue;
      }
      for ( SizeType i2 = i1 + 1; i2 < nf; ++ i2 ) {
	auto fault2 = fault_list1[i2];
	if ( !fault_info.is_rep(fault2) ) {
	  continue;
	}
	NaiveDualEngine engine(fault1, fault2);
	auto res = engine.solve(true, true);
	std::ostringstream buf;
	buf << fault1.str() << " " << fault2.str();
	auto pair_name = buf.str();
	if ( res == SatBool3::True ) {
	  EXPECT_TRUE( fault_info.check_compatible(fault1, fault2) ) << pair_name;
	}
	else {
	  EXPECT_FALSE( fault_info.check_compatible(fault1, fault2) ) << pair_name;
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
