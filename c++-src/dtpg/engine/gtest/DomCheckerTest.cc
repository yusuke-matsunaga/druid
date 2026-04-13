
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

  auto fault_info = FaultInfo(fault_list);
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
      auto checker = DomChecker(ffr1, ffr2, fault_list1, fault_list2);
      for ( SizeType pos = 0; pos < fault_list1.size(); ++ pos ) {
	auto fault1 = fault_list1[pos];
	// 支配故障のテスト
	auto fault2 = checker.dominator1(pos);
	if ( fault2.is_valid() ) {
	  NaiveDualEngine engine(fault1, fault2);
	  auto res = engine.solve(false, true);
	  std::ostringstream buf;
	  buf << fault1.str() << " @FFR#" << i1
	      << " and " << fault2.str() << " @FFR#" << i2;
	  auto pair_name = buf.str();
	  EXPECT_EQ( SatBool3::False, res ) << pair_name;
	}
	else {
	  for ( SizeType pos2 = 0; pos2 < fault_list2.size(); ++ pos2 ) {
	    if ( checker.dominator2(pos2).is_valid() ) {
	      continue;
	    }
	    auto fault2 = fault_list2[pos2];
	    NaiveDualEngine engine(fault1, fault2);
	    auto res = engine.solve(false, true);
	    std::ostringstream buf;
	    buf << fault1.str() << " @FFR#" << i1
		<< " and " << fault2.str() << " @FFR#" << i2;
	    auto pair_name = buf.str();
	    EXPECT_EQ( SatBool3::True, res ) << pair_name;
	  }
	}
      }
      for ( SizeType pos = 0; pos < fault_list2.size(); ++ pos ) {
	auto fault1 = fault_list2[pos];
	// 支配故障のテスト
	auto fault2 = checker.dominator2(pos);
	if ( fault2.is_valid() ) {
	  NaiveDualEngine engine(fault1, fault2);
	  auto res = engine.solve(false, true);
	  std::ostringstream buf;
	  buf << fault1.str() << " @FFR#" << i1
	      << " and " << fault2.str() << " @FFR#" << i2;
	  auto pair_name = buf.str();
	  EXPECT_EQ( SatBool3::False, res ) << pair_name;
	}
	else {
	  for ( SizeType pos2 = 0; pos2 < fault_list1.size(); ++ pos2 ) {
	    if ( checker.dominator1(pos2).is_valid() ) {
	      continue;
	    }
	    auto fault2 = fault_list1[pos2];
	    NaiveDualEngine engine(fault1, fault2);
	    auto res = engine.solve(false, true);
	    std::ostringstream buf;
	    buf << fault1.str() << " @FFR#" << i1
		<< " and " << fault2.str() << " @FFR#" << i2;
	    auto pair_name = buf.str();
	    EXPECT_EQ( SatBool3::True, res ) << pair_name;
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

TEST_F(DomCheckerTest, test_C432)
{
  auto data_dir = std::filesystem::path{TESTDATA_DIR};
  auto filename = data_dir / "C432.blif";
  auto network = TpgNetwork::read_blif(filename, FaultType::StuckAt);

  check(network);
}

END_NAMESPACE_DRUID
