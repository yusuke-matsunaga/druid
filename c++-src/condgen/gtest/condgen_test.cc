
/// @file condgen_test.cc
/// @brief condgen_test の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2023, 2024 Yusuke Matsunaga
/// All rights reserved.

#include "gtest/gtest.h"
#include "DtpgMgr.h"
#include "TpgNetwork.h"
#include "FFRFaultList.h"
#include "CondGenMgr.h"
#include "CondGen.h"
#include "CondGenChecker.h"
#include "ym/SatInitParam.h"


BEGIN_NAMESPACE_DRUID

string mydata[] = {
  "s27.blif",
  "s1196.blif",
  "s5378.blif",
  "s9234.blif"
};


class CondGenTestWithParam :
  public ::testing::TestWithParam<std::tuple<string, FaultType>>
{
public:

  /// @brief コンストラクタ
  CondGenTestWithParam();

  /// @brief テストを行う．
  void
  do_test();


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 初期化を行う．
  void
  SetUp();

  /// @brief 終了処理を行う．
  void
  TearDown();

  /// @brief テストパラメータからファイル名を取り出す．
  string
  filename();

  /// @brief テストパラメータから FaultType を取り出す．
  FaultType
  fault_type();

};


CondGenTestWithParam::CondGenTestWithParam()
{
}

// @brief 初期化を行う．
void
CondGenTestWithParam::SetUp()
{
}

// @brief 終了処理を行う．
void
CondGenTestWithParam::TearDown()
{
}

void
CondGenTestWithParam::do_test()
{
  unordered_map<string, JsonValue> option_dict;
  //option_dict.emplace("sat_param", JsonValue{"minisat2"});
  //option_dict.emplace("sat_param", JsonValue{"ymsat1_old"});
  JsonValue option{option_dict};
  auto network = TpgNetwork::read_blif(filename(), fault_type());
  auto fault_list = network.rep_fault_list();

  DtpgMgr mgr{network, fault_list};

  vector<const TpgFault*> det_fault_list;
  auto stats = mgr.run(
    [&](DtpgMgr& mgr, const TpgFault* f, TestVector tv) {
      det_fault_list.push_back(f);
    },
    [&](DtpgMgr& mgr, const TpgFault* f) {
    },
    [&](DtpgMgr& mgr, const TpgFault* f) {
    },
    option);

  SizeType limit = 100;

  FFRFaultList ffr_fault_list{network, det_fault_list};
  for ( auto ffr: ffr_fault_list.ffr_list() ) {
    CondGen gen{network, ffr, option};
    for ( auto fault: ffr_fault_list.fault_list(ffr) ) {
      SizeType count = 0;
      auto cond = gen.fault_cond(fault, limit, count);
      ASSERT_FALSE( cond.expr().is_zero() );
      CondGenChecker checker{network, ffr, fault, cond, option};
      EXPECT_TRUE ( checker.check() );
    }
  }
}

// @brief テストパラメータからファイル名を取り出す．
string
CondGenTestWithParam::filename()
{
  auto data = std::get<0>(GetParam());
  return string{TESTDATA_DIR} + "/" + data;
}

// @brief テストパラメータから FaultType を取り出す．
FaultType
CondGenTestWithParam::fault_type()
{
  return std::get<1>(GetParam());
}

TEST_P(CondGenTestWithParam, test1)
{
  do_test();
}

INSTANTIATE_TEST_SUITE_P(CondGenTest2, CondGenTestWithParam,
			 ::testing::Combine(::testing::ValuesIn(mydata),
					    ::testing::Values(FaultType::StuckAt,
							      FaultType::TransitionDelay)));

END_NAMESPACE_DRUID
