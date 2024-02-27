
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

  /// @brief spsfp のテストを行う．
  void
  spsfp_test(
    FaultType fault_type
  );

  /// @brief sppfp テストを行う．
  void
  sppfp_test(
    bool multi,
    FaultType fault_type
  );

  /// @brief ppsfp のテストを行う．
  void
  ppsfp_test(
    bool multi,
    FaultType fault_type
  );

  void
  compare_results(
    const unordered_map<SizeType, DiffBitsArray>& result_1,
    const unordered_map<SizeType, DiffBitsArray>& result_2
  );


private:

  const SizeType PAT_NUM{100};

};

void
FsimTest::spsfp_test(
  FaultType fault_type
)
{
  auto filename = string{TESTDATA_DIR} + "/" + GetParam();
  auto tpg_network = TpgNetwork::read_blif(filename);

  Fsim fsim;

  bool has_prev = fault_type == FaultType::TransitionDelay;
  fsim.initialize(tpg_network, has_prev, false, false);

  TpgFaultMgr fmgr;
  fmgr.gen_fault_list(tpg_network, fault_type);

  auto fault_list = fmgr.fault_list();
  fsim.set_fault_list(fault_list);

  SizeType input_num = tpg_network.input_num();
  SizeType dff_num = tpg_network.dff_num();

  std::mt19937 randgen;
  SizeType nv = PAT_NUM;

  RefSim refsim{tpg_network};

  TestVector tv(input_num, dff_num, has_prev);
  for ( SizeType i = 0; i < nv; ++ i ) {
    tv.set_from_random(randgen);
    for ( auto fault: fault_list ) {
      DiffBits dbits;
      auto diff = fsim.spsfp(tv, fault, dbits);
      EXPECT_EQ( dbits.elem_num() > 0, diff );
      auto ref_dbits = refsim.simulate(tv, fault, fault_type);
      dbits.sort();
      ref_dbits.sort();
      EXPECT_EQ( ref_dbits, dbits );
    }
  }
}

void
FsimTest::sppfp_test(
  bool multi,
  FaultType fault_type
)
{
  auto filename = string{TESTDATA_DIR} + "/" + GetParam();
  auto tpg_network = TpgNetwork::read_blif(filename);

  Fsim fsim;

  bool has_prev = fault_type == FaultType::TransitionDelay;
  fsim.initialize(tpg_network, has_prev, false, multi);

  TpgFaultMgr fmgr;
  fmgr.gen_fault_list(tpg_network, fault_type);

  auto fault_list = fmgr.fault_list();
  fsim.set_fault_list(fault_list);

  SizeType input_num = tpg_network.input_num();
  SizeType dff_num = tpg_network.dff_num();

  std::mt19937 randgen;
  SizeType nv = PAT_NUM;

  TestVector tv(input_num, dff_num, has_prev);
  for ( SizeType i = 0; i < nv; ++ i ) {
    tv.set_from_random(randgen);
    unordered_map<SizeType, DiffBits> dbits_dict;
    for ( auto fault: fault_list ) {
      DiffBits dbits;
      if ( fsim.spsfp(tv, fault, dbits) ) {
	dbits.sort();
	dbits_dict.emplace(fault.id(), dbits);
      }
    }
    fsim.sppfp(tv,
	       [&](
		 const TpgFault& f,
		 const DiffBits& dbits
	       )
	       {
		 EXPECT_TRUE( dbits_dict.count(f.id()) > 0 );
		 auto tmp_dbits = dbits;
		 tmp_dbits.sort();
		 EXPECT_EQ( dbits_dict.at(f.id()), tmp_dbits );
	       });
  }
}

void
FsimTest::ppsfp_test(
  bool multi,
  FaultType fault_type
)
{
  auto filename = string{TESTDATA_DIR} + "/" + GetParam();
  auto tpg_network = TpgNetwork::read_blif(filename);

  Fsim fsim;
  bool has_prev = fault_type == FaultType::TransitionDelay;

  fsim.initialize(tpg_network, has_prev, false, multi);

  TpgFaultMgr fmgr;
  fmgr.gen_fault_list(tpg_network, fault_type);

  auto fault_list = fmgr.fault_list();
  fsim.set_fault_list(fault_list);

  SizeType input_num = tpg_network.input_num();
  SizeType dff_num = tpg_network.dff_num();

  std::mt19937 randgen;
  SizeType nv = PAT_NUM;

  vector<TestVector> tv_list(nv);
  for ( SizeType i = 0; i < nv; ++ i ) {
    TestVector tv(input_num, dff_num, has_prev);
    tv.set_from_random(randgen);
    tv_list[i] = tv;
  }

  vector<TestVector> tv_buff;
  tv_buff.reserve(PV_BITLEN);
  SizeType base = 0;
  for ( auto& tv: tv_list ) {
    tv_buff.push_back(tv);
    if ( tv_buff.size() == PV_BITLEN || tv_buff.size() + base == nv ) {
      unordered_map<SizeType, DiffBitsArray> tv_fault_dict;
      for ( auto fault: fault_list ) {
	DiffBitsArray dbits_array;
	for ( SizeType i = 0; i < tv_buff.size(); ++ i ) {
	  auto& tv = tv_buff[i];
	  DiffBits dbits;
	  if ( fsim.spsfp(tv, fault, dbits) ) {
	    dbits_array.add_pat(dbits, i);
	  }
	}
	if ( dbits_array.dbits_union() != PV_ALL0 ) {
	  dbits_array.sort();
	  tv_fault_dict.emplace(fault.id(), dbits_array);
	}
      }
      unordered_map<SizeType, DiffBitsArray> tv_fault_dict2;
      fsim.ppsfp(tv_buff,
		 [&](
		   const TpgFault& f,
		   const DiffBitsArray& dbits_array
		 )
		 {
		   auto tmp_dbits_array = dbits_array;
		   tmp_dbits_array.sort();
		   tv_fault_dict2.emplace(f.id(), tmp_dbits_array);
		 });
      base += tv_buff.size();
      tv_buff.clear();
      compare_results(tv_fault_dict, tv_fault_dict2);
    }
  }
}

void
FsimTest::compare_results(
  const unordered_map<SizeType, DiffBitsArray>& result_1,
  const unordered_map<SizeType, DiffBitsArray>& result_2
)
{
  EXPECT_EQ( result_1.size(), result_2.size() );
  for ( auto& p: result_1 ) {
    auto id = p.first;
    EXPECT_TRUE( result_2.count(id) > 0 );
    if ( result_2.count(id) > 0 ) {
      auto& dbits1 = p.second;
      auto& dbits2 = result_2.at(id);
      EXPECT_EQ( dbits1, dbits2 );
      if ( dbits1 != dbits2 ) {
	cout << "Fault#" << id
	     << ": dbits_array1 " << dbits1 << endl
	     << "      dbits_array2 " << dbits2 << endl;
      }
    }
    else {
      cout << "Fault#" << id << ": not found in result_2" << endl;
    }
  }
  for ( auto& p: result_2 ) {
    auto id = p.first;
    EXPECT_TRUE( result_1.count(id) > 0 );
    if ( result_1.count(id) == 0 ) {
      cout << "Fault#" << id << ": not found in result_1"  << endl;
    }
  }
}

TEST_P(FsimTest, spsfp_sa_test)
{
  spsfp_test(FaultType::StuckAt);
}

TEST_P(FsimTest, spsfp_td_test)
{
  spsfp_test(FaultType::TransitionDelay);
}

TEST_P(FsimTest, sppfp_single_sa_test)
{
  sppfp_test(false, FaultType::StuckAt);
}

TEST_P(FsimTest, sppfp_single_td_test)
{
  sppfp_test(false, FaultType::TransitionDelay);
}

TEST_P(FsimTest, ppsfp_single_sa_test)
{
  ppsfp_test(false, FaultType::StuckAt);
}

TEST_P(FsimTest, ppsfp_single_td_test)
{
  ppsfp_test(false, FaultType::TransitionDelay);
}

TEST_P(FsimTest, sppfp_multi_sa_test)
{
  sppfp_test(true, FaultType::StuckAt);
}

TEST_P(FsimTest, sppfp_multi_td_test)
{
  sppfp_test(true, FaultType::TransitionDelay);
}

TEST_P(FsimTest, ppsfp_multi_sa_test)
{
  ppsfp_test(true, FaultType::StuckAt);
}

TEST_P(FsimTest, ppsfp_multi_td_test)
{
  ppsfp_test(true, FaultType::TransitionDelay);
}

INSTANTIATE_TEST_SUITE_P(FsimTest, FsimTest,
			 ::testing::Values("s27.blif", "s1196.blif"));

END_NAMESPACE_DRUID
