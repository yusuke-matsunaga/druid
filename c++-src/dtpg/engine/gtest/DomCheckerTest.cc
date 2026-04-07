
/// @file DomCheckerTest.cc
/// @brief DomChecker のテストプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include <gtest/gtest.h>
#include "dtpg/NaiveDualEngine.h"
#include "FFRAnalyze.h"
#include "DomChecker.h"
#include "types/TpgNetwork.h"
#include "types/TpgFault.h"
#include "types/TpgFaultList.h"


BEGIN_NAMESPACE_DRUID

class DomCheckerTest:
public ::testing::Test
{
public:

  void
  check(
    const TpgNetwork& network
  );

};

void
DomCheckerTest::check(
  const TpgNetwork& network
)
{
  auto fault_list = network.rep_fault_list();
  auto ffr_fault_list_array = fault_list.ffr_split();
  auto nf = network.ffr_num();

  auto fault_info = FaultInfo(network.max_fault_id());
  for ( auto ffr: network.ffr_list() ) {
    auto& fault_list1 = ffr_fault_list_array[ffr.id()];
    FFRAnalyze::run(ffr, fault_list1, fault_info);
  }
  std::vector<TpgFaultList> rep_fault_list_array(nf);
  for ( auto ffr: network.ffr_list() ) {
    auto& fault_list1 = ffr_fault_list_array[ffr.id()];
    auto& rep_fault_list = rep_fault_list_array[ffr.id()];
    for ( auto fault: fault_list1 ) {
      if ( fault_info.is_rep(fault) ) {
	rep_fault_list.push_back(fault);
      }
    }
  }

  for ( SizeType i1 = 0; i1 < nf - 1; ++ i1 ) {
    auto ffr1 = network.ffr(i1);
    auto& fault_list1 = rep_fault_list_array[i1];
    for ( SizeType i2 = i1 + 1; i2 < nf; ++ i2 ) {
      auto ffr2 = network.ffr(i2);
      auto& fault_list2 = rep_fault_list_array[i2];
      DomChecker::run(ffr1, ffr2, fault_list1, fault_list2, fault_info);
      for ( auto fault1: fault_list1 ) {
	if ( !fault_info.is_rep(fault1) ) {
	  continue;
	}
	for ( auto fault2: fault_list2 ) {
	  if ( !fault_info.is_rep(fault2) ) {
	    continue;
	  }
	  NaiveDualEngine engine(fault1, fault2);
	  auto res = engine.solve(true, true);
	  if ( res != SatBool3::True ) {
	    continue;
	  }
	  auto res1 = engine.solve(true, false);
	  if ( res1 == SatBool3::False ) {
	    EXPECT_FALSE( fault_info.is_rep(fault2) ) << fault2.str();
	  }
	  else {
	    auto res2 = engine.solve(false, true);
	    if ( res2 == SatBool3::False ) {
	      EXPECT_FALSE( fault_info.is_rep(fault1) ) << fault1.str();
	      break;
	    }
	  }
	}
      }
    }
  }

  for ( SizeType i1 = 0; i1 < nf - 1; ++ i1 ) {
    auto ffr1 = network.ffr(i1);
    auto& fault_list1 = rep_fault_list_array[i1];
    for ( SizeType i2 = i1 + 1; i2 < nf; ++ i2 ) {
      auto ffr2 = network.ffr(i2);
      auto& fault_list2 = rep_fault_list_array[i2];
      DomChecker::run(ffr1, ffr2, fault_list1, fault_list2, fault_info);
      for ( auto fault1: fault_list1 ) {
	if ( !fault_info.is_rep(fault1) ) {
	  continue;
	}
	for ( auto fault2: fault_list2 ) {
	  if ( !fault_info.is_rep(fault2) ) {
	    continue;
	  }
	  std::ostringstream buf;
	  buf << fault1.str() << " @FFR#" << i1
	      << " and " << fault2.str() << " @FFR#" << i2;
	  auto pair_name = buf.str();
	  NaiveDualEngine engine(fault1, fault2);
	  auto res = engine.solve(true, true);
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
}

TEST_F(DomCheckerTest, test_s27)
{
  auto data_dir = std::filesystem::path{TESTDATA_DIR};
  auto filename = data_dir / "s27.blif";
  auto network = TpgNetwork::read_blif(filename, FaultType::StuckAt);

  check(network);
}

TEST_F(DomCheckerTest, test_s298)
{
  auto data_dir = std::filesystem::path{TESTDATA_DIR};
  auto filename = data_dir / "s298.blif";
  auto network = TpgNetwork::read_blif(filename, FaultType::StuckAt);

  check(network);
}

END_NAMESPACE_DRUID
