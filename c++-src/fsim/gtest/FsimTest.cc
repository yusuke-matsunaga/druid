
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

#define SPSFP_TEST 1
#define SPPFP_TEST 1
#define PPSFP_TEST 1
#define SINGLE_TEST 1
#define MULTI_TEST 1
#define DETLIST_TEST 1
#define DIFFBITS_TEST 1


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

  /// @brief sppfp テストを行う．
  void
  sppfp_test(
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

  /// @brief spsfp のテストを行う．
  void
  spsfp2_test(
    const TpgNetwork& network
  );

  /// @brief sppfp テストを行う．
  void
  sppfp2_test(
    const TpgNetwork& network,
    bool multi,
    int thread_num = 0
  );

  /// @brief ppsfp のテストを行う．
  void
  ppsfp2_test(
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
      auto ref_res = ref_dbits.elem_num() > 0;
      auto res = fsim.spsfp(tv, fault);
      EXPECT_EQ( ref_res, res );
    }
  }
}

void
FsimTest::spsfp2_test(
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
      auto dbits = fsim.spsfp2(tv, fault);
      EXPECT_EQ( ref_dbits, dbits );
    }
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

  RefSim refsim{tpg_network};

  std::vector<TestVector> tv_list;
  tv_list.reserve(nv);
  {
    auto has_prev = tpg_network.fault_type() == FaultType::TransitionDelay;
    TestVector tv(input_num, dff_num, has_prev);
    for ( SizeType i = 0; i < nv; ++ i ) {
      tv.set_from_random(randgen);
      tv_list.push_back(tv);
    }
  }

  auto det_list_array = fsim.sppfp(tv_list);

  for ( SizeType i = 0; i < nv; ++ i ) {
    auto& tv = tv_list[i];
    std::unordered_set<SizeType> det_dict;
    SizeType ndet = 0;
    for ( auto fault: fault_list ) {
      auto dbits = refsim.simulate(tv, fault.id());
      if ( dbits.elem_num() > 0 ) {
	det_dict.insert(fault.id());
	++ ndet;
      }
    }
    auto& det_list = det_list_array[i];
    EXPECT_EQ( ndet, det_list.size() );
    SizeType prev_fid = 0;
    bool first = true;
    for ( auto fault: det_list ) {
      auto fid = fault.id();
      if ( det_dict.count(fid) == 0 ) {
	refsim.debug = true;
	refsim.simulate(tv, fid);
	refsim.debug = false;
      }
      EXPECT_TRUE( det_dict.count(fid) > 0 ) << fault.str();
      if ( first ) {
	first = false;
      }
      else {
	// det_list がソートされているかチェック
	EXPECT_TRUE( prev_fid < fid );
      }
      prev_fid = fid;
    }
  }
}

void
FsimTest::sppfp2_test(
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

  RefSim refsim{tpg_network};

  std::vector<TestVector> tv_list;
  {
    tv_list.reserve(nv);
    auto has_prev = tpg_network.fault_type() == FaultType::TransitionDelay;
    TestVector tv(input_num, dff_num, has_prev);
    for ( SizeType tv_id = 0; tv_id < nv; ++ tv_id ) {
      tv.set_from_random(randgen);
      tv_list.push_back(tv);
    }
  }

  auto res = fsim.sppfp2(tv_list);

  for ( SizeType tv_id = 0; tv_id < nv; ++ tv_id ) {
    auto& tv = tv_list[tv_id];
    std::unordered_map<SizeType, DiffBits> dbits_dict;
    SizeType ndet = 0;
    for ( auto fault: fault_list ) {
      auto dbits = refsim.simulate(tv, fault.id());
      if ( dbits.elem_num() > 0 ) {
	dbits_dict.emplace(fault.id(), dbits);
	++ ndet;
      }
    }
    if ( ndet != res.det_num(tv_id) ) {
      std::cout << "ref fault_list:" << std::endl;
      for ( auto fault: fault_list ) {
	if ( dbits_dict.count(fault.id()) > 0 ) {
	  auto& dbits = dbits_dict.at(fault.id());
	  std::cout << " " << fault.str()
		    << ": " << dbits
		    << std::endl;
	}
      }
      std::cout << std::endl;
      std::cout << "fault_list:" << std::endl;
      auto n = res.det_num(tv_id);
      for ( SizeType i = 0; i < n; ++ i ) {
	auto fault = res.fault(tv_id, i);
	auto dbits = res.diffbits(tv_id, i);
	std::cout << " " << fault.str()
		  << ": " << dbits
		  << std::endl;
      }
      std::cout << std::endl;
    }
    ASSERT_EQ( ndet, res.det_num(tv_id) );
    SizeType prev_fid = 0;
    for ( SizeType i = 0; i < ndet; ++ i ) {
      auto fault = res.fault(tv_id, i);
      auto fid = fault.id();
      EXPECT_TRUE( dbits_dict.count(fid) > 0 );
      auto dbits = res.diffbits(tv_id, i);
      EXPECT_EQ( dbits_dict.at(fid), dbits );
      if ( i > 0 ) {
	// res.fault(tv_id, i) がソートされているかチェック
	EXPECT_TRUE( prev_fid < fid );
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

  auto has_prev = tpg_network.fault_type() == FaultType::TransitionDelay;
  TestVector tv(input_num, dff_num, has_prev);

  std::vector<TpgFaultList> ref_det_list_array;
  ref_det_list_array.reserve(nv);
  for ( SizeType i = 0; i < nv; ++ i ) {
    tv.set_from_random(randgen);
    tv_list.push_back(tv);
    ref_det_list_array.push_back(fsim.sppfp(tv));
  }

  auto det_list_array = fsim.ppsfp(tv_list);

  ASSERT_EQ( nv, det_list_array.size() );
  for ( SizeType i = 0; i < nv; ++ i ) {
    auto f_list1 = ref_det_list_array[i];
    auto& f_list2 = det_list_array[i];
    EXPECT_EQ( f_list1, f_list2 );
  }
}

void
FsimTest::ppsfp2_test(
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

  std::vector<TestVector> tv_list;
  tv_list.reserve(nv);
  for ( SizeType i = 0; i < nv; ++ i ) {
    tv.set_from_random(randgen);
    tv_list.push_back(tv);
  }

  auto res = fsim.ppsfp2(tv_list);

  ASSERT_EQ( nv, res.tv_num() );
  for ( SizeType i = 0; i < nv; ++ i ) {
    auto ref_res = fsim.sppfp2(tv_list[i]);
    auto ndet = ref_res.det_num(0);
    ASSERT_EQ( ndet, res.det_num(i) );
    for ( SizeType j = 0; j < ndet; ++ j ) {
      auto fault1 = ref_res.fault(0, j);
      auto fault2 = res.fault(i, j);
      EXPECT_EQ( fault1, fault2 );
      auto dbits1 = ref_res.diffbits(0, j);
      auto dbits2 = res.diffbits(i, j);
      EXPECT_EQ( dbits1, dbits2 );
    }
  }
}

#if SPSFP_TEST
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

#if DIFFBITS_TEST
TEST_P(FsimTest, spsfp2_sa_test)
{
  auto tpg_network = read_network(FaultType::StuckAt);
  spsfp2_test(tpg_network);
}

TEST_P(FsimTest, spsfp2_td_test)
{
  auto tpg_network = read_network(FaultType::TransitionDelay);
  spsfp2_test(tpg_network);
}
#endif // DIFFBITS_TEST
#endif // SPSFP_TEST

#if SPPFP_TEST
#if SINGLE_TEST
#if DETLIST_TEST
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
#endif

#if DIFFBITS_TEST
TEST_P(FsimTest, sppfp2_single_sa_test)
{
  auto tpg_network = read_network(FaultType::StuckAt);
  sppfp2_test(tpg_network, false);
}

TEST_P(FsimTest, sppfp2_single_td_test)
{
  auto tpg_network = read_network(FaultType::TransitionDelay);
  sppfp2_test(tpg_network, false);
}
#endif // DIFFBITS_TEST
#endif // SINGLE_TEST

#if MULTI_TEST
#if DETLIST_TEST
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
#endif

#if DIFFBITS_TEST
TEST_P(FsimTest, sppfp2_multi_sa_test)
{
  auto tpg_network = read_network(FaultType::StuckAt);
  sppfp2_test(tpg_network, true);
}

TEST_P(FsimTest, sppfp2_multi_td_test)
{
  auto tpg_network = read_network(FaultType::TransitionDelay);
  sppfp2_test(tpg_network, true);
}
#endif // DIFFBITS_TEST
#endif // MULTI_TEST
#endif // SPPFP_TEST

#if PPSFP_TEST
#if SINGLE_TEST
#if DETLIST_TEST
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
#endif

#if DIFFBITS_TEST
TEST_P(FsimTest, ppsfp2_single_sa_test)
{
  auto tpg_network = read_network(FaultType::StuckAt);
  ppsfp2_test(tpg_network, false);
}

TEST_P(FsimTest, ppsfp2_single_td_test)
{
  auto tpg_network = read_network(FaultType::TransitionDelay);
  ppsfp2_test(tpg_network, false);
}
#endif // DIFFBITS_TEST
#endif // SINGLE_TEST

#if MULTI_TEST
#if DETLIST_TEST
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
#endif

#if DIFFBITS_TEST
TEST_P(FsimTest, ppsfp2_multi_sa_test)
{
  auto tpg_network = read_network(FaultType::StuckAt);
  ppsfp2_test(tpg_network, true, 2);
}

TEST_P(FsimTest, ppsfp2_multi_td_test)
{
  auto tpg_network = read_network(FaultType::TransitionDelay);
  ppsfp2_test(tpg_network, true);
}
#endif // DIFFBITS_TEST
#endif // MULTI_TEST
#endif // PPSFP_TEST


#if 0
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

#if 1
INSTANTIATE_TEST_SUITE_P(FsimTest3, FsimTest,
			 ::testing::Values("C432.blif"));
#endif

#if 0
INSTANTIATE_TEST_SUITE_P(FsimTest4, FsimTest,
			 ::testing::Values("and2.blif"));
#endif

#if 0
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
#endif

END_NAMESPACE_DRUID
