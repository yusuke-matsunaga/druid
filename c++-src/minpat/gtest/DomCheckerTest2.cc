
/// @file DomCheckerTest.cc
/// @brief DomChecker のテストプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include <gtest/gtest.h>
#include "dtpg/NaiveDualEngine.h"
#include "minpat/FaultInfo.h"
#include "FFRAnalyze.h"
#include "DomChecker.h"
#include "types/TpgNetwork.h"
#include "types/TpgFault.h"
#include "types/TpgFaultList.h"


BEGIN_NAMESPACE_DRUID

class DomCheckerTest2:
public ::testing::Test
{
public:

  void
  check_sub(
    const DomChecker& checker,
    const TpgFaultList& fault_list1,
    const TpgFaultList& fault_list2,
    bool reverse
  );

};


void
DomCheckerTest2::check_sub(
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

TEST_F(DomCheckerTest2, test1)
{
  auto data_dir = std::filesystem::path{TESTDATA_DIR};
  auto filename = data_dir / "s27.blif";
  auto network = TpgNetwork::read_blif(filename, FaultType::StuckAt);
  auto fault_list = network.rep_fault_list();
  auto fault_list_array = fault_list.ffr_split();

  FaultInfo fault_info(fault_list);

  SizeType i1 = 0;
  auto ffr1 = network.ffr(i1);
  auto& fault_list1 = fault_list_array[i1];
  FFRAnalyze::run(ffr1, fault_list1, fault_info);
  TpgFaultList rep_fault_list1;
  rep_fault_list1.reserve(fault_list1.size());
  for ( auto fault: fault_list1 ) {
    if ( fault_info.is_rep(fault) ) {
      rep_fault_list1.push_back(fault);
    }
  }

  SizeType i2 = 5;
  auto ffr2 = network.ffr(i2);
  auto& fault_list2 = fault_list_array[i2];
  FFRAnalyze::run(ffr2, fault_list2, fault_info);
  TpgFaultList rep_fault_list2;
  rep_fault_list2.reserve(fault_list2.size());
  for ( auto fault: fault_list2 ) {
    if ( fault_info.is_rep(fault) ) {
      rep_fault_list2.push_back(fault);
    }
  }

  auto option = JsonValue::object();
  option.add("debug", 1);
  auto checker = DomChecker(ffr1, ffr2, rep_fault_list1, rep_fault_list2, option);
  check_sub(checker, rep_fault_list1, rep_fault_list2, false);
  check_sub(checker, rep_fault_list2, rep_fault_list1, true);
}

END_NAMESPACE_DRUID
