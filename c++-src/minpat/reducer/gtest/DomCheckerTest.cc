
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

  void
  check_sub(
    const DomChecker& checker,
    const TpgFaultList& fault_list1,
    const TpgFaultList& fault_list2,
    bool reverse
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
  std::vector<TpgFaultList> rep_fault_list_array(nf);
  auto ffr_option = JsonValue::object();
  ffr_option.add("ffr_reduction", true);
  for ( auto ffr: network.ffr_list() ) {
    auto& fault_list1 = ffr_fault_list_array[ffr.id()];
    FFRAnalyze::run(ffr, fault_list1, fault_info, ffr_option);
    auto& rep_fault_list = rep_fault_list_array[ffr.id()];
    for ( auto fault: fault_list1 ) {
      if ( fault_info.is_rep(fault) ) {
	rep_fault_list.push_back(fault);
      }
    }
  }

  int debug = 0;
  auto option = JsonValue::object();
  option.add("debug", debug);
  for ( SizeType i1 = 0; i1 < nf - 1; ++ i1 ) {
    auto ffr1 = network.ffr(i1);
    auto& fault_list1 = rep_fault_list_array[i1];
    for ( SizeType i2 = i1 + 1; i2 < nf; ++ i2 ) {
      auto ffr2 = network.ffr(i2);
      auto& fault_list2 = rep_fault_list_array[i2];
      auto checker = DomChecker(ffr1, ffr2, fault_list1, fault_list2, option);
      if ( debug ) {
	std::cout << std::endl
		  << "FFR#" << i1 << " and FFR#" << i2 << std::endl;
      }
      check_sub(checker, fault_list1, fault_list2, false);
      check_sub(checker, fault_list2, fault_list1, true);
    }
  }
}

void
DomCheckerTest::check_sub(
  const DomChecker& checker,
  const TpgFaultList& fault_list1,
  const TpgFaultList& fault_list2,
  bool reverse
)
{
  for ( SizeType pos = 0; pos < fault_list1.size(); ++ pos ) {
    auto fault1 = fault_list1[pos];

    // 等価故障のテスト
    {
      auto fault2 = reverse ? checker.rep2(pos) : checker.rep1(pos);
      if ( fault2.is_valid() ) {
	NaiveDualEngine engine(fault1, fault2);
	std::ostringstream buf;
	buf << fault1.str() << " and " << fault2.str();
	auto pair_name = buf.str();
	EXPECT_TRUE( fault1.id() > fault2.id() ) << pair_name;
	auto res11 = engine.solve(true, true);
	EXPECT_EQ( SatBool3::True, res11 ) << pair_name;
	auto res01 = engine.solve(false, true);
	EXPECT_EQ( SatBool3::False, res01 ) << pair_name;
	auto res10 = engine.solve(true, false);
	EXPECT_EQ( SatBool3::False, res10 ) << pair_name;
	continue;
      }
    }

    // 支配故障のテスト
    {
      auto fault2 = reverse ? checker.dominator2(pos) : checker.dominator1(pos);
      if ( fault2.is_valid() ) {
	NaiveDualEngine engine(fault1, fault2);
	std::ostringstream buf;
	buf << fault1.str() << " and " << fault2.str();
	auto pair_name = buf.str();
	auto res11 = engine.solve(true, true);
	EXPECT_EQ( SatBool3::True, res11 ) << pair_name;
	auto res01 = engine.solve(false, true);
	EXPECT_EQ( SatBool3::False, res01 ) << pair_name;
	continue;
      }
    }

    for ( SizeType pos2 = 0; pos2 < fault_list2.size(); ++ pos2 ) {
      auto fault2 = fault_list2[pos2];
      NaiveDualEngine engine(fault1, fault2);
      auto res11 = engine.solve(true, true);
      if ( res11 != SatBool3::True ) {
	continue;
      }
      std::ostringstream buf;
      buf << fault1.str() << " and " << fault2.str();
      auto pair_name = buf.str();
      auto res01 = engine.solve(false, true);
      auto res10 = engine.solve(true, false);
      if ( res01 == SatBool3::False ) {
	if ( res10 == SatBool3::False && fault1.id() < fault2.id() ) {
	  // fault1 と fault2 は等価故障で fault1 が代表故障
	  ;
	}
	else {
	  EXPECT_TRUE( false ) << pair_name;
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
