
/// @file DtpgEngineTest.cc
/// @brief DtpgEngine のテストプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include <gtest/gtest.h>
#include "dtpg/FaultAnalyze.h"
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
  public ::testing::Test
{
public:

  void
  check(
    const TpgNetwork& network
  );
};

void
FaultAnalyzeTest::check(
  const TpgNetwork& network
)
{
  auto fault_list = network.rep_fault_list();

  auto option = JsonValue::object();
  option.add("ffr_reduction", true);
  option.add("global_reduction", true);

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
  for ( SizeType i1 = 0; i1 < nd - 1; ++ i1 ) {
    auto fault1 = det_list[i1];
    if ( !ref_rep_mark[fault1.id()] ) {
      continue;
    }
    for ( SizeType i2 = i1 + 1; i2 < nd; ++ i2 ) {
      auto fault2 = det_list[i2];
      if ( !ref_rep_mark[fault2.id()] ) {
	continue;
      }
      NaiveDualEngine engine(fault1, fault2);
      auto res = engine.solve(true, true);
      if ( res != SatBool3::True ) {
	continue;
      }
      { // fault1 を検出して fault2 を検出しない条件
	auto res = engine.solve(true, false);
	if ( res == SatBool3::False ) {
	  ref_rep_mark[fault2.id()] = false;
	  if ( debug ) {
	    std::cout << fault2.str() << " is dominated by "
		      << fault1.str() << std::endl;
	  }
	  continue;
	}
      }
      { // fault2 を検出して fault1 を検出しない条件
	auto res = engine.solve(false, true);
	if ( res == SatBool3::False ) {
	  ref_rep_mark[fault1.id()] = false;
	  if ( debug ) {
	    std::cout << fault1.str() << " is dominated by "
		      << fault2.str() << std::endl;
	  }
	  break;
	}
      }
    }
  }

  for ( auto fault: fault_list ) {
    auto rep_mark = fault_info.is_rep(fault);
    EXPECT_EQ( ref_rep_mark[fault.id()], rep_mark ) << fault.str();
  }
}


TEST_F(FaultAnalyzeTest, s27)
{
  auto data_dir = std::filesystem::path{TESTDATA_DIR};
  auto filename = data_dir / "s27.blif";
  auto network = TpgNetwork::read_blif(filename, FaultType::StuckAt);
  check(network);
}


TEST_F(FaultAnalyzeTest, s298)
{
  auto data_dir = std::filesystem::path{TESTDATA_DIR};
  auto filename = data_dir / "s298.blif";
  auto network = TpgNetwork::read_blif(filename, FaultType::StuckAt);
  check(network);
}


TEST_F(FaultAnalyzeTest, b01)
{
  auto data_dir = std::filesystem::path{TESTDATA_DIR};
  auto filename = data_dir / "b01.bench";
  auto network = TpgNetwork::read_iscas89(filename, FaultType::StuckAt);
  check(network);
}

END_NAMESPACE_DRUID
