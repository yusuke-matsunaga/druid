
/// @file FsimTest.cc
/// @brief FsimTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include <gtest/gtest.h>
#include "fsim/Fsim.h"
#include "RefSim.h"
#include "types/TpgNetwork.h"
#include "types/TpgFault.h"
#include "types/TpgFaultList.h"
#include "types/TestVector.h"


BEGIN_NAMESPACE_DRUID

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
    bool multi
  );

  /// @brief ppsfp のテストを行う．
  void
  ppsfp_test(
    const TpgNetwork& network,
    bool multi
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
  auto fsim = Fsim(network, fault_list);

  SizeType input_num = network.input_num();
  SizeType dff_num = network.dff_num();

  std::mt19937 randgen;
  SizeType nv = PAT_NUM;

  RefSim refsim{network};

  TestVector tv(input_num, dff_num, network.fault_type() == FaultType::TransitionDelay);
  for ( SizeType i = 0; i < nv; ++ i ) {
    tv.set_from_random(randgen);
    for ( auto fault: fault_list ) {
      DiffBits dbits;
      auto diff = fsim.spsfp(tv, fault, dbits);
      EXPECT_EQ( dbits.elem_num() > 0, diff );
      auto ref_dbits = refsim.simulate(tv, fault.id());
      dbits.sort();
      ref_dbits.sort();
      EXPECT_EQ( ref_dbits, dbits );
    }
  }
}

void
FsimTest::sppfp_test(
  const TpgNetwork& tpg_network,
  bool multi
)
{
  auto fault_list = tpg_network.rep_fault_list();
  auto fsim_option = JsonValue::object();
  fsim_option.add("multi_thread", multi);
  auto fsim = Fsim(tpg_network, fault_list, fsim_option);

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
	dbits.sort();
	dbits_dict.emplace(fault.id(), dbits);
	++ ndet;
      }
    }
    auto res = fsim.sppfp(tv);
    EXPECT_EQ( ndet, res.fault_list(0).size() );
    for ( auto fid: res.fault_list(0) ) {
      EXPECT_TRUE( dbits_dict.count(fid) > 0 );
      auto tmp_dbits = res.diffbits(0, fid);
      tmp_dbits.sort();
      EXPECT_EQ( dbits_dict.at(fid), tmp_dbits );
    }
  }
}

void
FsimTest::ppsfp_test(
  const TpgNetwork& tpg_network,
  bool multi
)
{
  auto fault_list = tpg_network.rep_fault_list();
  auto fsim_option = JsonValue::object();
  fsim_option.add("multi_thread", multi);
  auto fsim = Fsim(tpg_network, fault_list, fsim_option);

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
    EXPECT_EQ( f_list1, f_list2 );
    for ( auto fid: f_list1 ) {
      auto ref_dbits = ref_res.diffbits(0, fid);
      auto dbits = res.diffbits(i, fid);
      EXPECT_EQ( ref_dbits, dbits );
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
  ppsfp_test(tpg_network, true);
}

TEST_P(FsimTest, ppsfp_multi_td_test)
{
  auto tpg_network = read_network(FaultType::TransitionDelay);
  ppsfp_test(tpg_network, true);
}

INSTANTIATE_TEST_SUITE_P(FsimTest, FsimTest,
			 ::testing::Values("s27.blif", "s1196.blif"));

END_NAMESPACE_DRUID
