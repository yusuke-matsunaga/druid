
/// @file FsimTest.cc
/// @brief FsimTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include <gtest/gtest.h>
#include "Fsim.h"
#include "RefSim.h"
#include "TpgNetwork.h"
#include "TpgFaultMgr.h"
#include "TestVector.h"
#include "NodeValList.h"


BEGIN_NAMESPACE_DRUID

class FsimTest :
  public ::testing::TestWithParam<string>
{
public:

  /// @brief 縮退故障のテストを行う．
  void
  sa_test();

  /// @brief 遷移故障のテストを行う．
  void
  td_test();

};

void
FsimTest::sa_test()
{
  auto filename = string{TESTDATA_DIR} + "/" + GetParam();
  auto tpg_network = TpgNetwork::read_blif(filename);

  Fsim fsim;

  fsim.initialize(tpg_network, false, false);

  TpgFaultMgr fmgr;
  fmgr.gen_fault_list(tpg_network, FaultType::StuckAt);

  auto fault_list = fmgr.fault_list();
  fsim.set_fault_list(fault_list);

  SizeType input_num = tpg_network.input_num();
  SizeType dff_num = tpg_network.dff_num();

  std::mt19937 randgen;
  SizeType nv = 1000;

  RefSim refsim{tpg_network};

  TestVector tv(input_num, dff_num, false);
  for ( SizeType i = 0; i < nv; ++ i ) {
    tv.set_from_random(randgen);
    for ( auto fault: fault_list ) {
      fsim.spsfp(tv, fault);
      auto dbits = fsim.spsfp_diffbits();
      auto ref_dbits = refsim.simulate_sa(tv, fault);
      EXPECT_EQ( ref_dbits, dbits );
    }
  }
}

void
FsimTest::td_test()
{
  auto filename = string{TESTDATA_DIR} + "/" + GetParam();
  auto tpg_network = TpgNetwork::read_blif(filename);

  Fsim fsim;

  fsim.initialize(tpg_network, true, false);

  TpgFaultMgr fmgr;
  fmgr.gen_fault_list(tpg_network, FaultType::TransitionDelay);

  auto fault_list = fmgr.fault_list();
  fsim.set_fault_list(fault_list);

  SizeType input_num = tpg_network.input_num();
  SizeType dff_num = tpg_network.dff_num();

  std::mt19937 randgen;
  SizeType nv = 1000;

  RefSim refsim{tpg_network};

  TestVector tv(input_num, dff_num, true);
  for ( SizeType i = 0; i < nv; ++ i ) {
    tv.set_from_random(randgen);
    for ( auto fault: fault_list ) {
      fsim.spsfp(tv, fault);
      auto dbits = fsim.spsfp_diffbits();
      auto ref_dbits = refsim.simulate_td(tv, fault);
      EXPECT_EQ( ref_dbits, dbits );
    }
  }
}

TEST_P(FsimTest, sa_test)
{
  sa_test();
}

TEST_P(FsimTest, td_test)
{
  td_test();
}

INSTANTIATE_TEST_SUITE_P(FsimTest, FsimTest,
			 ::testing::Values("s27.blif", "s1196.blif", "s5378.blif"));

END_NAMESPACE_DRUID
