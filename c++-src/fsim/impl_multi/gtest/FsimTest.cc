
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

  /// @brief 縮退故障の spsfp のテストを行う．
  void
  spsfp_sa_test();

  /// @brief 遷移故障の spsfp テストを行う．
  void
  spsfp_td_test();

  /// @brief 縮退故障の sppfp のテストを行う．
  void
  sppfp_sa_test();

  /// @brief 遷移故障の sppfp テストを行う．
  void
  sppfp_td_test();

  /// @brief 縮退故障の ppsfp のテストを行う．
  void
  ppsfp_sa_test();

  /// @brief 遷移故障の ppsfp のテストを行う．
  void
  ppsfp_td_test();


private:

  const SizeType PAT_NUM{100};

};

void
FsimTest::spsfp_sa_test()
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
  SizeType nv = PAT_NUM;

  RefSim refsim{tpg_network};

  TestVector tv(input_num, dff_num, false);
  for ( SizeType i = 0; i < nv; ++ i ) {
    tv.set_from_random(randgen);
    for ( auto fault: fault_list ) {
      DiffBits dbits;
      auto diff = fsim.spsfp(tv, fault, dbits);
      EXPECT_EQ( dbits.any(), diff );
      auto ref_dbits = refsim.simulate_sa(tv, fault);
      EXPECT_EQ( ref_dbits, dbits );
    }
  }
}

void
FsimTest::spsfp_td_test()
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
  SizeType nv = PAT_NUM;

  RefSim refsim{tpg_network};

  TestVector tv(input_num, dff_num, true);
  for ( SizeType i = 0; i < nv; ++ i ) {
    tv.set_from_random(randgen);
    for ( auto fault: fault_list ) {
      DiffBits dbits;
      auto diff = fsim.spsfp(tv, fault, dbits);
      EXPECT_EQ( dbits.any(), diff );
      auto ref_dbits = refsim.simulate_td(tv, fault);
      EXPECT_EQ( ref_dbits, dbits );
    }
  }
}

void
FsimTest::sppfp_sa_test()
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
  SizeType nv = PAT_NUM;

  TestVector tv(input_num, dff_num, false);
  for ( SizeType i = 0; i < nv; ++ i ) {
    tv.set_from_random(randgen);
    unordered_map<SizeType, DiffBits> dbits_dict;
    for ( auto fault: fault_list ) {
      DiffBits dbits;
      if ( fsim.spsfp(tv, fault, dbits) ) {
	dbits_dict.emplace(fault.id(), dbits);
      }
    }
    auto fault_list = fsim.sppfp(tv);
    EXPECT_EQ( dbits_dict.size(), fault_list.size() );
    for ( auto fault: fault_list ) {
      EXPECT_TRUE( dbits_dict.count(fault.id()) );
      auto dbits = fsim.sppfp_diffbits(fault);
      EXPECT_EQ( dbits_dict.at(fault.id()), dbits);
    }
  }
}

void
FsimTest::sppfp_td_test()
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
  SizeType nv = PAT_NUM;

  TestVector tv(input_num, dff_num, true);
  for ( SizeType i = 0; i < nv; ++ i ) {
    tv.set_from_random(randgen);
    unordered_map<SizeType, DiffBits> dbits_dict;
    for ( auto fault: fault_list ) {
      DiffBits dbits;
      if ( fsim.spsfp(tv, fault, dbits) ) {
	dbits_dict.emplace(fault.id(), dbits);
      }
    }
    auto fault_list = fsim.sppfp(tv);
    EXPECT_EQ( dbits_dict.size(), fault_list.size() );
    for ( auto fault: fault_list ) {
      EXPECT_TRUE( dbits_dict.count(fault.id()) );
      auto dbits = fsim.sppfp_diffbits(fault);
      EXPECT_EQ( dbits_dict.at(fault.id()), dbits);
    }
  }
}

string
make_str(
  SizeType tv_id,
  const TpgFault& fault
)
{
  ostringstream buf;
  buf << tv_id << "|" << fault.id();
  return buf.str();
}

void
FsimTest::ppsfp_sa_test()
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
  SizeType nv = PAT_NUM;

  vector<TestVector> tv_list(nv);
  for ( SizeType i = 0; i < nv; ++ i ) {
    TestVector tv(input_num, dff_num, false);
    tv.set_from_random(randgen);
    tv_list[i] = tv;
  }

  unordered_map<string, DiffBits> tv_fault_dict;
  for ( SizeType i = 0; i < nv; ++ i ) {
    auto& tv = tv_list[i];
    for ( auto fault: fault_list ) {
      DiffBits dbits;
      if ( fsim.spsfp(tv, fault, dbits) ) {
	tv_fault_dict.emplace(make_str(i, fault), dbits);
      }
    }
  }
  unordered_map<string, DiffBits> tv_fault_dict2;
  fsim.ppsfp(tv_list,
	     [&](SizeType i,
		 TpgFault f,
		 DiffBits dbits)->bool {
	       tv_fault_dict2.emplace(make_str(i, f), dbits);
	       return true;
	     });
  EXPECT_EQ( tv_fault_dict.size(), tv_fault_dict2.size() );
  for ( auto& p: tv_fault_dict ) {
    auto str = p.first;
    EXPECT_TRUE( tv_fault_dict2.count(str) > 0 );
    auto dbits = p.second;
    EXPECT_EQ( dbits, tv_fault_dict2.at(str) );
  }
}

void
FsimTest::ppsfp_td_test()
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
  SizeType nv = PAT_NUM;

  vector<TestVector> tv_list(nv);
  for ( SizeType i = 0; i < nv; ++ i ) {
    TestVector tv(input_num, dff_num, true);
    tv.set_from_random(randgen);
    tv_list[i] = tv;
  }

  unordered_map<string, DiffBits> tv_fault_dict;
  for ( SizeType i = 0; i < nv; ++ i ) {
    auto& tv = tv_list[i];
    for ( auto fault: fault_list ) {
      DiffBits dbits;
      if ( fsim.spsfp(tv, fault, dbits) ) {
	tv_fault_dict.emplace(make_str(i, fault), dbits);
      }
    }
  }
  unordered_map<string, DiffBits> tv_fault_dict2;
  fsim.ppsfp(tv_list,
	     [&](SizeType i,
		 TpgFault f,
		 DiffBits dbits)->bool {
	       tv_fault_dict2.emplace(make_str(i, f), dbits);
	       return true;
	     });
  EXPECT_EQ( tv_fault_dict.size(), tv_fault_dict2.size() );
  for ( auto& p: tv_fault_dict ) {
    auto str = p.first;
    EXPECT_TRUE( tv_fault_dict2.count(str) > 0 );
    auto dbits = p.second;
    EXPECT_EQ( dbits, tv_fault_dict2.at(str) );
  }
}

TEST_P(FsimTest, spsfp_sa_test)
{
  spsfp_sa_test();
}

TEST_P(FsimTest, spsfp_td_test)
{
  spsfp_td_test();
}

TEST_P(FsimTest, sppfp_sa_test)
{
  sppfp_sa_test();
}

TEST_P(FsimTest, sppfp_td_test)
{
  sppfp_td_test();
}

TEST_P(FsimTest, ppsfp_sa_test)
{
  ppsfp_sa_test();
}

TEST_P(FsimTest, ppsfp_td_test)
{
  ppsfp_td_test();
}

INSTANTIATE_TEST_SUITE_P(FsimTest, FsimTest,
			 ::testing::Values("s27.blif", "s1196.blif"));

END_NAMESPACE_DRUID
