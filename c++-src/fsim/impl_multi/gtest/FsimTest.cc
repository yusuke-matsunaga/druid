
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
  sppfp_sa_test(
    bool multi
  );

  /// @brief 遷移故障の sppfp テストを行う．
  void
  sppfp_td_test(
    bool multi
  );

  /// @brief 縮退故障の ppsfp のテストを行う．
  void
  ppsfp_sa_test(
    bool multi
  );

  /// @brief 遷移故障の ppsfp のテストを行う．
  void
  ppsfp_td_test(
    bool multi
  );


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
      EXPECT_EQ( dbits.elem_num() > 0, diff );
      auto ref_dbits = refsim.simulate_sa(tv, fault);
      dbits.sort();
      ref_dbits.sort();
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
      EXPECT_EQ( dbits.elem_num() > 0, diff );
      auto ref_dbits = refsim.simulate_td(tv, fault);
      dbits.sort();
      ref_dbits.sort();
      EXPECT_EQ( ref_dbits, dbits );
    }
  }
}

void
FsimTest::sppfp_sa_test(
  bool multi
)
{
  auto filename = string{TESTDATA_DIR} + "/" + GetParam();
  auto tpg_network = TpgNetwork::read_blif(filename);

  Fsim fsim;

  if ( multi ) {
    fsim.initialize(tpg_network, false, false);
  }
  else {
    fsim.initialize_naive(tpg_network, false, false);
  }

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
FsimTest::sppfp_td_test(
  bool multi
)
{
  auto filename = string{TESTDATA_DIR} + "/" + GetParam();
  auto tpg_network = TpgNetwork::read_blif(filename);

  Fsim fsim;

  if ( multi ) {
    fsim.initialize(tpg_network, true, false);
  }
  else {
    fsim.initialize_naive(tpg_network, true, false);
  }

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
		 EXPECT_TRUE( dbits_dict.count(f.id()) );
		 auto tmp_dbits = dbits;
		 tmp_dbits.sort();
		 EXPECT_EQ( dbits_dict.at(f.id()), tmp_dbits);
	       });
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
FsimTest::ppsfp_sa_test(
  bool multi
)
{
  auto filename = string{TESTDATA_DIR} + "/" + GetParam();
  auto tpg_network = TpgNetwork::read_blif(filename);

  Fsim fsim;

  if ( multi ) {
    fsim.initialize(tpg_network, false, false);
  }
  else {
    fsim.initialize_naive(tpg_network, false, false);
  }

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
	tv_fault_dict.emplace(fault.id(), dbits_array);
      }
      unordered_map<SizeType, DiffBitsArray> tv_fault_dict2;
      fsim.ppsfp(tv_buff,
		 [&](
		   const TpgFault& f,
		   const DiffBitsArray& dbits_array
		 ) {
		   tv_fault_dict2.emplace(f.id(), dbits_array);
		 });
      EXPECT_EQ( tv_fault_dict.size(), tv_fault_dict2.size() );
      for ( auto& p: tv_fault_dict ) {
	auto id = p.first;
	EXPECT_TRUE( tv_fault_dict2.count(id) > 0 );
	if ( tv_fault_dict2.count(id) > 0 ) {
	  auto dbits_array = p.second;
	  EXPECT_EQ( dbits_array, tv_fault_dict2.at(id) );
	}
	else {
	  cout << id << ": not found in tv_fault_dict2" << endl;
	}
      }
      for ( auto& p: tv_fault_dict2 ) {
	auto id = p.first;
	EXPECT_TRUE( tv_fault_dict.count(id) > 0 );
	if ( tv_fault_dict.count(id) > 0 ) {
	  auto dbits_array = p.second;
	  EXPECT_EQ( dbits_array, tv_fault_dict.at(id) );
	}
	else {
	  cout << id << ": not found in tv_fault_dict" << endl;
	}
      }
      base += tv_buff.size();
    }
  }
}

void
FsimTest::ppsfp_td_test(
  bool multi
)
{
  auto filename = string{TESTDATA_DIR} + "/" + GetParam();
  auto tpg_network = TpgNetwork::read_blif(filename);

  Fsim fsim;

  if ( multi ) {
    fsim.initialize(tpg_network, true, false);
  }
  else {
    fsim.initialize_naive(tpg_network, true, false);
  }

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
	tv_fault_dict.emplace(fault.id(), dbits_array);
      }
      unordered_map<SizeType, DiffBitsArray> tv_fault_dict2;
      fsim.ppsfp(tv_list,
		 [&](
		   const TpgFault& f,
		   const DiffBitsArray& dbits_array
		 ) {
		   tv_fault_dict2.emplace(f.id(), dbits_array);
		 });
      EXPECT_EQ( tv_fault_dict.size(), tv_fault_dict2.size() );
      for ( auto& p: tv_fault_dict ) {
	auto id = p.first;
	EXPECT_TRUE( tv_fault_dict2.count(id) > 0 );
	if ( tv_fault_dict2.count(id) > 0 ) {
	  auto dbits_array = p.second;
	  EXPECT_EQ( dbits_array, tv_fault_dict2.at(id) );
	}
	else {
	  cout << id << ": not found in tv_fault_dict2" << endl;
	}
      }
      for ( auto& p: tv_fault_dict2 ) {
	auto id = p.first;
	EXPECT_TRUE( tv_fault_dict.count(id) > 0 );
	if ( tv_fault_dict.count(id) > 0 ) {
	  auto dbits_array = p.second;
	  EXPECT_EQ( dbits_array, tv_fault_dict.at(id) );
	}
	else {
	  cout << id << ": not found in tv_fault_dict"  << endl;
	}
      }
      base += tv_buff.size();
    }
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

TEST_P(FsimTest, sppfp_single_sa_test)
{
  sppfp_sa_test(false);
}

TEST_P(FsimTest, sppfp_single_td_test)
{
  sppfp_td_test(false);
}

TEST_P(FsimTest, ppsfp_single_sa_test)
{
  ppsfp_sa_test(false);
}

TEST_P(FsimTest, ppsfp_single_td_test)
{
  ppsfp_td_test(false);
}

TEST_P(FsimTest, sppfp_multi_sa_test)
{
  sppfp_sa_test(true);
}

TEST_P(FsimTest, sppfp_multi_td_test)
{
  sppfp_td_test(true);
}

TEST_P(FsimTest, ppsfp_multi_sa_test)
{
  ppsfp_sa_test(true);
}

TEST_P(FsimTest, ppsfp_multi_td_test)
{
  ppsfp_td_test(true);
}

INSTANTIATE_TEST_SUITE_P(FsimTest, FsimTest,
			 ::testing::Values("s27.blif", "s1196.blif"));

END_NAMESPACE_DRUID
