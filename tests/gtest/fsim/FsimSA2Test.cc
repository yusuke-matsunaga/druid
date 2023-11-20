
/// @file FsimSA2Test.cc
/// @brief FsimSA2Test の実装ファイル
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


BEGIN_NAMESPACE_DRUID

class FsimSA2Test :
  public ::testing::TestWithParam<string>
{
public:

  /// @brief テストを行う．
  void
  do_test();

};

void
FsimSA2Test::do_test()
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

TEST_P(FsimSA2Test, test1)
{
  do_test();
}

INSTANTIATE_TEST_SUITE_P(FsimSA2Test, FsimSA2Test,
			 ::testing::Values("s27.blif", "s1196.blif", "s5378.blif"));

END_NAMESPACE_DRUID
