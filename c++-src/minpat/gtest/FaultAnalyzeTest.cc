
/// @file DtpgEngineTest.cc
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

BEGIN_NONAMESPACE
static
const bool debug = false;
END_NONAMESPACE

class FaultAnalyzeTest:
public ::testing::TestWithParam<std::tuple<std::string, bool, bool>>
{
public:

  void
  do_test();

};

void
FaultAnalyzeTest::do_test()
{
  auto name = std::get<0>(GetParam());
  auto multi_thread = std::get<1>(GetParam());
  auto simple_reduction = std::get<2>(GetParam());

  auto data_dir = std::filesystem::path{TESTDATA_DIR};
  auto filename = data_dir / name;
  auto network = TpgNetwork::read_blif(filename, FaultType::StuckAt);
  auto fault_list = network.rep_fault_list();

  auto option = JsonValue::object();
  option.add("ffr_reduction", true);
  option.add("simple_reduction", simple_reduction);
  option.add("global_reduction", true);
  option.add("multi_thread", multi_thread);

  auto fault_info = FaultAnalyze::run(fault_list, option);

  std::vector<TpgFault> det_list;
  std::vector<bool> ref_rep_mark(network.max_fault_id(), false);
  det_list.reserve(fault_list.size());
  for ( auto fault: fault_list ) {
    NaiveDtpgEngine engine(fault);
    auto res = engine.solver().solve();
    if ( res == SatBool3::True ) {
      det_list.push_back(fault);
      ref_rep_mark[fault.id()] = true;
    }
  }
  auto nd = det_list.size();
  for ( auto fault1: det_list ) {
    if ( fault_info.is_dominated(fault1) ) {
      auto fault2 = fault_info.dominator(fault1);
      NaiveDualEngine engine(fault1, fault2);
      auto res = engine.solve(false, true);
      std::ostringstream buf;
      buf << fault1.str() << "@FFR#" << network.ffr(fault1).id()
	  << ", " << fault2.str() << "@FFR#" << network.ffr(fault2).id();
      EXPECT_EQ( SatBool3::False, res ) << buf.str();
      continue;
    }
    auto rep_fault = fault_info.rep_fault(fault1);
    if ( rep_fault.is_valid() && rep_fault != fault1 ) {
      EXPECT_TRUE( rep_fault.id() < fault1.id() );
      NaiveDualEngine engine(fault1, rep_fault);
      auto res11 = engine.solve(true, true);
      EXPECT_EQ( SatBool3::True, res11 );
      auto res01 = engine.solve(false, true);
      EXPECT_EQ( SatBool3::False, res01 );
      auto res10 = engine.solve(true, false);
      EXPECT_EQ( SatBool3::False, res10 );
      continue;
    }
    for ( auto fault2: det_list ) {
      if ( fault2 == fault1 ) {
	continue;
      }
      if ( !fault_info.is_rep(fault2) ) {
	continue;
      }
      NaiveDualEngine engine(fault1, fault2);
      auto res11 = engine.solve(true, true);
      if ( res11 != SatBool3::True ) {
	continue;
      }
      auto res = engine.solve(false, true);
      std::ostringstream buf;
      buf << fault1.str() << "@FFR#" << network.ffr(fault1).id()
	  << ", " << fault2.str() << "@FFR#" << network.ffr(fault2).id();
      EXPECT_EQ( SatBool3::True, res ) << buf.str();
    }
  }
}

TEST_P(FaultAnalyzeTest, test1)
{
  do_test();
}

INSTANTIATE_TEST_SUITE_P(FaultAnalyzeTest, FaultAnalyzeTest,
			 ::testing::Combine(::testing::Values("s27.blif",
							      "s298.blif",
							      "C432.blif"),
					    ::testing::Values(false, true),
					    ::testing::Values(false, true)));

END_NAMESPACE_DRUID
