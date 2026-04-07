
/// @file DtpgEngineTest.cc
/// @brief DtpgEngine のテストプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include <gtest/gtest.h>
#include "dtpg/DtpgEngine.h"
#include "dtpg/NaiveDtpgEngine.h"
#include "dtpg/SuffCond.h"
#include "types/TpgNetwork.h"
#include "types/TpgFault.h"
#include "types/TpgFaultList.h"
#include "types/TestVector.h"


BEGIN_NAMESPACE_DRUID

TEST(DtpgEngineTest, node_mode)
{
  auto data_dir = std::filesystem::path{TESTDATA_DIR};
  auto filename = data_dir / "s1196.blif";
  auto network = TpgNetwork::read_blif(filename, FaultType::StuckAt);
  auto fault_list = network.rep_fault_list();

  auto node_fault_list_array = fault_list.node_split();
  for ( auto node: network.node_list() ) {
    auto& fault_list1 = node_fault_list_array[node.id()];
    DtpgEngine engine(node);
    for ( auto fault: fault_list1 ) {
      auto lits = engine.make_detect_condition(fault);
      auto res = engine.solver().solve(lits);
      NaiveDtpgEngine ref_engine(fault);
      auto ref_res = ref_engine.solver().solve();
      EXPECT_EQ( ref_res, res );
    }
  }
}

TEST(DtpgEngineTest, ffr_mode)
{
  auto data_dir = std::filesystem::path{TESTDATA_DIR};
  auto filename = data_dir / "s1196.blif";
  auto network = TpgNetwork::read_blif(filename, FaultType::StuckAt);
  auto fault_list = network.rep_fault_list();

  auto ffr_fault_list_array = fault_list.ffr_split();
  for ( auto ffr: network.ffr_list() ) {
    auto& fault_list1 = ffr_fault_list_array[ffr.id()];
    DtpgEngine engine(ffr);
    for ( auto fault: fault_list1 ) {
      auto lits = engine.make_detect_condition(fault);
      auto res = engine.solver().solve(lits);
      NaiveDtpgEngine ref_engine(fault);
      auto ref_res = ref_engine.solver().solve();
      EXPECT_EQ( ref_res, res );
    }
  }
}

TEST(DtpgEngineTest, mffc_mode)
{
  auto data_dir = std::filesystem::path{TESTDATA_DIR};
  auto filename = data_dir / "s1196.blif";
  auto network = TpgNetwork::read_blif(filename, FaultType::StuckAt);
  auto fault_list = network.rep_fault_list();

  auto mffc_fault_list_array = fault_list.mffc_split();
  for ( auto mffc: network.mffc_list() ) {
    TpgFFR ffr1;
    TpgFaultList fault_list1;
    std::tie(ffr1, fault_list1) = mffc_fault_list_array[mffc.id()];
    if ( ffr1.is_valid() ) {
      DtpgEngine engine(ffr1);
      for ( auto fault: fault_list1 ) {
	auto lits = engine.make_detect_condition(fault);
	auto res = engine.solver().solve(lits);
	NaiveDtpgEngine ref_engine(fault);
	auto ref_res = ref_engine.solver().solve();
	EXPECT_EQ( ref_res, res );
      }
    }
    else {
      DtpgEngine engine(mffc);
      for ( auto fault: fault_list1 ) {
	auto lits = engine.make_detect_condition(fault);
	auto res = engine.solver().solve(lits);
	NaiveDtpgEngine ref_engine(fault);
	auto ref_res = ref_engine.solver().solve();
	EXPECT_EQ( ref_res, res );
      }
    }
  }
}

END_NAMESPACE_DRUID
