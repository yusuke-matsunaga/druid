
/// @file FsimTest.cc
/// @brief FsimTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include <gtest/gtest.h>
#include "fsim/Fsim.h"
#include "RefSim.h"
#include "dtpg/DtpgMgr.h"
#include "dtpg/SuffCond.h"
#include "types/TpgNetwork.h"
#include "types/TpgFault.h"
#include "types/TpgFaultList.h"
#include "types/TestVector.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE
const bool debug = false;
END_NONAMESPACE

class FsimTest :
public ::testing::TestWithParam<std::string>
{
public:

  TpgNetwork
  read_network(
    FaultType fault_type
  )
  {
    auto data_dir = std::filesystem::path{TESTDATA_DIR};
    auto filename = data_dir / GetParam();
    auto tpg_network = TpgNetwork::read_blif(filename, fault_type);
    return tpg_network;
  }

  /// @brief spsfp のテストを行う．
  void
  spsfp_test(
    const TpgNetwork& network
  );

  /// @brief spsfp のテストを行う．
  void
  xspsfp_test(
    const TpgNetwork& network
  );

  /// @brief sppfp テストを行う．
  void
  sppfp_test(
    const TpgNetwork& network,
    bool multi,
    int thread_num = 0
  );

  /// @brief sppfp テストを行う．
  void
  xsppfp_test(
    const TpgNetwork& network,
    bool multi,
    int thread_num = 0
  );

  /// @brief ppsfp のテストを行う．
  void
  ppsfp_test(
    const TpgNetwork& network,
    bool multi,
    int thread_num = 0
  );


private:

  const SizeType PAT_NUM{100};

};

void
FsimTest::spsfp_test(
  const TpgNetwork& network
)
{
  auto fault_list = network.rep_fault_list();
  auto fsim = Fsim(fault_list);

  SizeType input_num = network.input_num();
  SizeType dff_num = network.dff_num();

  std::mt19937 randgen;
  SizeType nv = PAT_NUM;

  RefSim refsim{network};

  if ( debug ) {
    network.print(std::cout);
  }
  TestVector tv(input_num, dff_num, network.fault_type() == FaultType::TransitionDelay);
  for ( SizeType i = 0; i < nv; ++ i ) {
    tv.set_from_random(randgen);
    for ( auto fault: fault_list ) {
      if ( debug ) {
	std::cout << fault.str() << std::endl;
      }
      auto ref_dbits = refsim.simulate(tv, fault.id());
      DiffBits dbits;
      auto diff = fsim.spsfp(tv, fault, dbits);
      EXPECT_EQ( dbits.elem_num() > 0, diff );
      EXPECT_EQ( ref_dbits, dbits );
    }
  }
}

// @brief spsfp のテストを行う．
void
FsimTest::xspsfp_test(
  const TpgNetwork& network
)
{
  auto fault_list = network.rep_fault_list();
  auto option = JsonValue::object();
  option.add("has_x", true);
  auto fsim = Fsim(fault_list, option);

  SizeType input_num = network.input_num();
  SizeType dff_num = network.dff_num();

  RefSim refsim{network};

  if ( debug ) {
    network.print(std::cout);
  }
  for ( auto fault: fault_list ) {
    auto results = DtpgMgr::run(TpgFaultList({fault}));
    if ( results.status(fault) != FaultStatus::Detected ) {
      continue;
    }
    auto cond = results.cond(fault);
    auto assign_list = cond.main_cond();
    assign_list.merge(cond.aux_cond());
    auto ref_dbits = refsim.simulate(assign_list, fault.id());
    DiffBits dbits;
    auto diff = fsim.xspsfp(assign_list, fault, dbits);
    EXPECT_EQ( dbits.elem_num() > 0, diff );
    dbits.sort();
    ref_dbits.sort();
    EXPECT_EQ( ref_dbits, dbits );
  }
}

void
FsimTest::sppfp_test(
  const TpgNetwork& tpg_network,
  bool multi,
  int thread_num
)
{
  auto fault_list = tpg_network.rep_fault_list();
  auto fsim_option = JsonValue::object();
  fsim_option.add("multi_thread", multi);
  fsim_option.add("thread_num", thread_num);
  auto fsim = Fsim(fault_list, fsim_option);

  SizeType input_num = tpg_network.input_num();
  SizeType dff_num = tpg_network.dff_num();

  std::mt19937 randgen;
  SizeType nv = PAT_NUM;

  auto has_prev = tpg_network.fault_type() == FaultType::TransitionDelay;
  TestVector tv(input_num, dff_num, has_prev);
  for ( SizeType i = 0; i < nv; ++ i ) {
    tv.set_from_random(randgen);
    std::unordered_map<SizeType, DiffBits> dbits_dict;
    SizeType ndet = 0;
    for ( auto fault: fault_list ) {
      DiffBits dbits;
      if ( fsim.spsfp(tv, fault, dbits) ) {
	dbits_dict.emplace(fault.id(), dbits);
	++ ndet;
      }
    }
    auto res = fsim.sppfp(tv);
    EXPECT_EQ( ndet, res.fault_list(0).size() );
    SizeType prev_fid = 0;
    bool first = true;
    for ( auto fault: res.fault_list(0) ) {
      auto fid = fault.id();
      EXPECT_TRUE( dbits_dict.count(fid) > 0 );
      auto tmp_dbits = res.diffbits(0, fid);
      EXPECT_EQ( dbits_dict.at(fid), tmp_dbits );
      if ( first ) {
	first = false;
      }
      else {
	// res.fault_list(0) がソートされているかチェック
	EXPECT_TRUE( prev_fid < fid );
      }
      prev_fid = fid;
    }
  }
}

void
FsimTest::xsppfp_test(
  const TpgNetwork& tpg_network,
  bool multi,
  int thread_num
)
{
  auto fault_list = tpg_network.rep_fault_list();
  auto fsim_option = JsonValue::object();
  fsim_option.add("has_x", true);
  fsim_option.add("multi_thread", multi);
  fsim_option.add("thread_num", thread_num);
  auto fsim = Fsim(fault_list, fsim_option);

  SizeType input_num = tpg_network.input_num();
  SizeType dff_num = tpg_network.dff_num();

  RefSim refsim{tpg_network};

  if ( debug ) {
    tpg_network.print(std::cout);
  }

  for ( auto fault: fault_list ) {
    auto results = DtpgMgr::run(TpgFaultList({fault}));
    if ( results.status(fault) != FaultStatus::Detected ) {
      continue;
    }
    auto cond = results.cond(fault);
    auto assign_list = cond.main_cond();
    assign_list.merge(cond.aux_cond());
    std::unordered_map<SizeType, DiffBits> dbits_dict;
    std::vector<TpgFault> ref_fault_list;
    for ( auto fault1: fault_list ) {
      DiffBits dbits1 = refsim.simulate(assign_list, fault1.id());
      if ( dbits1.elem_num() > 0 ) {
	dbits_dict.emplace(fault1.id(), dbits1);
	ref_fault_list.push_back(fault1);
      }
    }
    auto res = fsim.xsppfp(assign_list);
    if ( ref_fault_list.size() != res.fault_list(0).size() ) {
      if ( debug ) {
	std::cout << "original fault: " << fault.str() << std::endl;
	std::cout << "assign list = " << assign_list << std::endl;
	std::cout << "ref fault_list:";
	for ( auto fault: ref_fault_list ) {
	  std::cout << " " << fault.str();
	}
	std::cout << std::endl;
	std::cout << "fsim fault_list:";
	for ( auto fault: res.fault_list(0) ) {
	  std::cout << " " << fault.str();
	}
	std::cout << std::endl;
      }
    }
    ASSERT_EQ( ref_fault_list.size(), res.fault_list(0).size() );
    SizeType prev_fid = 0;
    bool first = true;
    for ( auto fault: res.fault_list(0) ) {
      auto fid = fault.id();
      EXPECT_TRUE( dbits_dict.count(fid) > 0 );
      auto tmp_dbits = res.diffbits(0, fid);
      EXPECT_EQ( dbits_dict.at(fid), tmp_dbits );
      if ( first ) {
	first = false;
      }
      else {
	// res.fault_list(0) がソートされているかチェック
	ASSERT_TRUE( prev_fid < fid );
      }
      prev_fid = fid;
    }
  }
}

void
FsimTest::ppsfp_test(
  const TpgNetwork& tpg_network,
  bool multi,
  int thread_num
)
{
  auto fault_list = tpg_network.rep_fault_list();
  auto fsim_option = JsonValue::object();
  fsim_option.add("multi_thread", multi);
  fsim_option.add("thread_num", thread_num);
  auto fsim = Fsim(fault_list, fsim_option);

  SizeType input_num = tpg_network.input_num();
  SizeType dff_num = tpg_network.dff_num();

  std::mt19937 randgen;
  SizeType nv = PAT_NUM;

  std::vector<TestVector> tv_list;
  tv_list.reserve(nv);

  // sppfp を用いた結果
  FsimResults ref_res;

  auto has_prev = tpg_network.fault_type() == FaultType::TransitionDelay;
  TestVector tv(input_num, dff_num, has_prev);

  std::vector<FsimResults> ref_res_list;
  ref_res_list.reserve(nv);
  for ( SizeType i = 0; i < nv; ++ i ) {
    tv.set_from_random(randgen);
    tv_list.push_back(tv);
    ref_res_list.push_back(fsim.sppfp(tv));
  }

  auto res = fsim.ppsfp(tv_list);

  ASSERT_EQ( nv, res.tv_num() );
  for ( SizeType i = 0; i < nv; ++ i ) {
    auto& ref_res = ref_res_list[i];
    auto f_list1 = ref_res.fault_list(0);
    auto f_list2 = res.fault_list(i);
    ASSERT_EQ( f_list1, f_list2 );
    for ( auto fault: f_list1 ) {
      auto fid = fault.id();
      auto ref_dbits = ref_res.diffbits(0, fid);
      auto dbits = res.diffbits(i, fid);
      EXPECT_EQ( ref_dbits, dbits );
    }
    SizeType prev_fid = 0;
    bool first = true;
    for ( auto fault: f_list2 ) {
      auto fid = fault.id();
      if ( first ) {
	first = false;
      }
      else {
	ASSERT_TRUE( prev_fid < fid );
      }
      prev_fid = fid;
    }
  }
}

TEST_P(FsimTest, spsfp_sa_test)
{
  auto tpg_network = read_network(FaultType::StuckAt);
  spsfp_test(tpg_network);
}

TEST_P(FsimTest, spsfp_td_test)
{
  auto tpg_network = read_network(FaultType::TransitionDelay);
  spsfp_test(tpg_network);
}

TEST_P(FsimTest, xspsfp_sa_test)
{
  auto tpg_network = read_network(FaultType::StuckAt);
  xspsfp_test(tpg_network);
}

TEST_P(FsimTest, sppfp_single_sa_test)
{
  auto tpg_network = read_network(FaultType::StuckAt);
  sppfp_test(tpg_network, false);
}

TEST_P(FsimTest, sppfp_single_td_test)
{
  auto tpg_network = read_network(FaultType::TransitionDelay);
  sppfp_test(tpg_network, false);
}

TEST_P(FsimTest, xsppfp_single_sa_test)
{
  auto tpg_network = read_network(FaultType::StuckAt);
  xsppfp_test(tpg_network, false);
}

TEST_P(FsimTest, ppsfp_single_sa_test)
{
  auto tpg_network = read_network(FaultType::StuckAt);
  ppsfp_test(tpg_network, false);
}

TEST_P(FsimTest, ppsfp_single_td_test)
{
  auto tpg_network = read_network(FaultType::TransitionDelay);
  ppsfp_test(tpg_network, false);
}

TEST_P(FsimTest, sppfp_multi_sa_test)
{
  auto tpg_network = read_network(FaultType::StuckAt);
  sppfp_test(tpg_network, true);
}

TEST_P(FsimTest, sppfp_multi_td_test)
{
  auto tpg_network = read_network(FaultType::TransitionDelay);
  sppfp_test(tpg_network, true);
}

TEST_P(FsimTest, ppsfp_multi_sa_test)
{
  auto tpg_network = read_network(FaultType::StuckAt);
  ppsfp_test(tpg_network, true, 2);
}

TEST_P(FsimTest, ppsfp_multi_td_test)
{
  auto tpg_network = read_network(FaultType::TransitionDelay);
  ppsfp_test(tpg_network, true);
}

#if 1
INSTANTIATE_TEST_SUITE_P(FsimTest1, FsimTest,
			 ::testing::Values("s27.blif", "s1196.blif"));
#endif

#if 0
INSTANTIATE_TEST_SUITE_P(FsimTest2, FsimTest,
			 ::testing::Values("s1196.blif"));
#endif

#if 0
INSTANTIATE_TEST_SUITE_P(FsimTest3, FsimTest,
			 ::testing::Values("s27.blif"));
#endif

#if 0
INSTANTIATE_TEST_SUITE_P(FsimTest4, FsimTest,
			 ::testing::Values("and2.blif"));
#endif

TEST(FsimTest, case1)
{
  auto data_dir = std::filesystem::path{TESTDATA_DIR};
  auto filename = data_dir / "C432.blif";
  auto network = TpgNetwork::read_blif(filename, FaultType::StuckAt);
  auto fault_list = network.rep_fault_list();
  auto option = JsonValue::object();
  option.add("has_x", true);
  auto fsim = Fsim(fault_list, option);
  RefSim refsim{network};

  for ( auto fault: fault_list ) {
    if ( fault.str() == "G#136:I0:SA0" ) {
      AssignList assign_list;
      assign_list.add(network.node(92), 1, true);
      assign_list.add(network.node(95), 1, true);
      assign_list.add(network.node(100), 1, true);
      assign_list.add(network.node(105), 1, true);
      assign_list.add(network.node(110), 1, true);
      assign_list.add(network.node(115), 1, true);
      assign_list.add(network.node(120), 1, true);
      assign_list.add(network.node(125), 1, true);
      assign_list.add(network.node(130), 1, true);
      assign_list.add(network.node(135), 1, true);
      assign_list.add(network.node(139), 1, true);
      DiffBits _;
      auto res = fsim.xspsfp(assign_list, fault, _);
      EXPECT_TRUE( res );
      auto dbits = refsim.simulate(assign_list, fault.id());
      EXPECT_TRUE( dbits.elem_num() > 0 );
    }
  }
}

TEST(FsimTest, case2)
{
  auto data_dir = std::filesystem::path{TESTDATA_DIR};
  auto filename = data_dir / "s298.blif";
  auto network = TpgNetwork::read_blif(filename, FaultType::StuckAt);
  auto fault_list = network.rep_fault_list();
  auto option = JsonValue::object();
  option.add("has_x", true);
  auto fsim = Fsim(fault_list, option);
  RefSim refsim{network};

  for ( auto fault: fault_list ) {
    if ( fault.str() == "I#5:O:SA1" ) {
      AssignList assign_list;
      assign_list.add(network.node(0), 1, false);
      assign_list.add(network.node(4), 1, true);
      assign_list.add(network.node(5), 1, false);
      assign_list.add(network.node(6), 1, true);
      assign_list.add(network.node(7), 1, false);
      assign_list.add(network.node(8), 1, true);
      assign_list.add(network.node(15), 1, false);
      auto res = fsim.xspsfp(assign_list, fault);
      EXPECT_TRUE( res );
      auto dbits = refsim.simulate(assign_list, fault.id());
      EXPECT_TRUE( dbits.elem_num() > 0 );
    }
  }
}

TEST(FsimTest, case3)
{
  auto data_dir = std::filesystem::path{TESTDATA_DIR};
  auto filename = data_dir / "b15.bench";
  auto network = TpgNetwork::read_iscas89(filename, FaultType::StuckAt);
  auto fault_list = network.rep_fault_list();
  auto option = JsonValue::object();
  option.add("has_x", true);
  auto fsim = Fsim(fault_list, option);
  RefSim refsim{network};

  TpgFault fault1;
  TpgFault fault2;
  for ( auto fault: fault_list ) {
    if ( fault.str() == "G#3464:I0:SA1" ) {
      fault1 = fault;
    }
    else if ( fault.str() == "G#3462:I0:SA1" ) {
      fault2 = fault;
    }
  }
  AssignList assign_list;
  assign_list.add(network.node(106), 1, true);
  assign_list.add(network.node(107), 1, false);
  assign_list.add(network.node(108), 1, true);
  assign_list.add(network.node(218), 1, false);
  assign_list.add(network.node(219), 1, false);
  assign_list.add(network.node(220), 1, true);
  assign_list.add(network.node(221), 1, true);
  assign_list.add(network.node(222), 1, true);
  assign_list.add(network.node(223), 1, true);
  assign_list.add(network.node(224), 1, false);
  assign_list.add(network.node(225), 1, true);
  assign_list.add(network.node(226), 1, false);
  assign_list.add(network.node(227), 1, false);
  assign_list.add(network.node(228), 1, true);
  assign_list.add(network.node(237), 1, false);
  assign_list.add(network.node(238), 1, false);
  assign_list.add(network.node(239), 1, false);
  assign_list.add(network.node(240), 1, false);
  assign_list.add(network.node(241), 1, true);
  assign_list.add(network.node(242), 1, true);
  assign_list.add(network.node(243), 1, true);
  assign_list.add(network.node(247), 1, false);
  assign_list.add(network.node(248), 1, false);
  assign_list.add(network.node(249), 1, false);
  refsim.simulate(assign_list, fault1.id());
  refsim.simulate(assign_list, fault2.id());
  auto res = fsim.spsfp(assign_list, fault2);
  EXPECT_TRUE( res );
}

END_NAMESPACE_DRUID
