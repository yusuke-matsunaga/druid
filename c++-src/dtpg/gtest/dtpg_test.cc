
/// @file DtpgTest.cc
/// @brief DtpgTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2023, 2024 Yusuke Matsunaga
/// All rights reserved.

#include "gtest/gtest.h"
#include "DtpgMgr.h"
#include "TpgNetwork.h"
#include "ym/SatInitParam.h"


BEGIN_NAMESPACE_DRUID

struct TestData
{
  TestData()
  {
  }

  // コンストラクタ
  TestData(
    const string& filename,
    int total_num,
    int sa_detect_num,
    int td_detect_num,
    int sa_untest_num,
    int td_untest_num
  ) : mFileName{filename},
      mTotalFaultNum{total_num},
      mSaDetectFaultNum{sa_detect_num},
      mTdDetectFaultNum{td_detect_num},
      mSaUntestFaultNum{sa_untest_num},
      mTdUntestFaultNum{td_untest_num}
  {
  };

  // ファイル名
  string mFileName;

  // 総故障数
  int mTotalFaultNum;

  // 検出可能故障数(縮退故障)
  int mSaDetectFaultNum;

  // 検出可能故障数(遷移故障)
  int mTdDetectFaultNum;

  // 検出不能故障数(縮退故障)
  int mSaUntestFaultNum;

  // 検出不能故障数(遷移故障)
  int mTdUntestFaultNum;

};

ostream&
operator<<(
  ostream& s,
  TestData tdata
)
{
  s << tdata.mFileName;
  return s;
}

TestData mydata1[] = {
  TestData{"s5378.blif", 4603, 4563, 4253, 40, 350}
};

TestData mydata2[] = {
  TestData{"s27.blif",     32,   32,   32,   0,    0},
  TestData{"s1196.blif", 1242, 1242, 1241,   0,    1},
  TestData{"s5378.blif", 4603, 4563, 4253,  40,  350},
  TestData{"s9234.blif", 6927, 6475, 5844, 452, 1083}
};


class DtpgTestWithParam2 :
  public ::testing::TestWithParam<std::tuple<TestData, string, string, string, FaultType, string>>
{
public:

  /// @brief コンストラクタ
  DtpgTestWithParam2();

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

  /// @brief テストパラメータから総故障数を取り出す．
  int
  total_fault_num();

  /// @brief テストパラメータから検出可能故障数を取り出す．
  int
  detect_fault_num();

  /// @brief テストパラメータから検出不能故障数を取り出す．
  int
  untest_fault_num();

  /// @brief テストパラメータから SATタイプを取り出す．
  string
  sat_type();

  /// @brief テストパラメータからグループモードを取り出す．
  string
  group_mode();

  /// @brief テストパラメータからドライバタイプを取り出す．
  string
  driver_type();

  /// @brief テストパラメータから FaultType を取り出す．
  FaultType
  fault_type();

  /// @brief テストパラメータから just_type を取り出す．
  string
  just_type();

};

struct DtpgTestResult
{
  /// @brief 検出故障数
  SizeType DetCount;
  /// @brief 検出不能故障数
  SizeType UntestCount;
  /// @brief アボート故障数
  SizeType AbortCount;

  void
  det_func(
    const TpgFault* f,
    TestVector tv
  )
  {
    ++ DetCount;
  }

  void
  untest_func(
    const TpgFault* f
  )
  {
    ++ UntestCount;
  }

  void
  abort_func(
    const TpgFault* f
  )
  {
    ++ AbortCount;
  }
};


DtpgTestWithParam2::DtpgTestWithParam2()
{
}

// @brief 初期化を行う．
void
DtpgTestWithParam2::SetUp()
{
}

// @brief 終了処理を行う．
void
DtpgTestWithParam2::TearDown()
{
}

void
DtpgTestWithParam2::do_test()
{
  unordered_map<string, JsonValue> option_dict;
  option_dict.emplace("group_mode", group_mode());
  option_dict.emplace("driver_type", driver_type());
  option_dict.emplace("justifier", just_type());
  auto sat_obj = JsonValue{sat_type()};
  option_dict.emplace("sat_param", sat_obj);
  JsonValue option{option_dict};
  auto network = TpgNetwork::read_blif(filename(), fault_type());
  auto fault_list = network.rep_fault_list();

  Fsim fsim;
  fsim.initialize(network, fault_list, true, false);

  DtpgMgr mgr{network, fault_list};

  SizeType AbortCount = 0;
  SizeType ErrorCount = 0;
  auto stats = mgr.run(
    [&](DtpgMgr& mgr, const TpgFault* f, TestVector tv) {
      DiffBits dbits;
      bool r = fsim.spsfp(tv, f, dbits);
      if ( !r ) {
	++ ErrorCount;
      }
    },
    [&](DtpgMgr& mgr, const TpgFault* f) {
    },
    [&](DtpgMgr& mgr, const TpgFault* f) {
      ++ AbortCount;
    },
    option);

  EXPECT_EQ( total_fault_num(), mgr.total_count() );
  EXPECT_EQ( detect_fault_num(), mgr.detected_count() );
  EXPECT_EQ( untest_fault_num(), mgr.untestable_count() );
  EXPECT_EQ( 0, AbortCount );
  EXPECT_EQ( 0, ErrorCount );
}

// @brief テストパラメータからファイル名を取り出す．
string
DtpgTestWithParam2::filename()
{
  const TestData& data = std::get<0>(GetParam());
  return string{TESTDATA_DIR} + "/" + data.mFileName;
}

// @brief テストパラメータから総故障数を取り出す．
int
DtpgTestWithParam2::total_fault_num()
{
  const TestData& data = std::get<0>(GetParam());
  return data.mTotalFaultNum;
}

// @brief テストパラメータから検出可能故障数を取り出す．
int
DtpgTestWithParam2::detect_fault_num()
{
  const TestData& data = std::get<0>(GetParam());
  if ( fault_type() == FaultType::StuckAt ) {
    return data.mSaDetectFaultNum;
  }
  else {
    return data.mTdDetectFaultNum;
  }
}

// @brief テストパラメータから検出不能故障数を取り出す．
int
DtpgTestWithParam2::untest_fault_num()
{
  const TestData& data = std::get<0>(GetParam());
  if ( fault_type() == FaultType::StuckAt ) {
    return data.mSaUntestFaultNum;
  }
  else {
    return data.mTdUntestFaultNum;
  }
}

// @brief テストパラメータから SATタイプを取り出す．
string
DtpgTestWithParam2::sat_type()
{
  return std::get<1>(GetParam());
}

// @brief テストパラメータからグループモードを取り出す．
string
DtpgTestWithParam2::group_mode()
{
  return std::get<2>(GetParam());
}

// @brief テストパラメータからドライバタイプを取り出す．
string
DtpgTestWithParam2::driver_type()
{
  return std::get<3>(GetParam());
}

// @brief テストパラメータから FaultType を取り出す．
FaultType
DtpgTestWithParam2::fault_type()
{
  return std::get<4>(GetParam());
}

// @brief テストパラメータから just_type を取り出す．
string
DtpgTestWithParam2::just_type()
{
  return std::get<5>(GetParam());
}

TEST_P(DtpgTestWithParam2, test1)
{
  do_test();
}

INSTANTIATE_TEST_SUITE_P(DtpgTest1, DtpgTestWithParam2,
			 ::testing::Combine(::testing::ValuesIn(mydata1),
					    ::testing::Values("lingeling",
							      "minisat2", "minisat",
							      "ymsat1", "ymsat2",
							      "ymsat1_old"),
					    ::testing::Values("ffr"),
					    ::testing::Values("engine"),
					    ::testing::Values(FaultType::StuckAt),
					    ::testing::Values("just1")));
INSTANTIATE_TEST_SUITE_P(DtpgTest2, DtpgTestWithParam2,
			 ::testing::Combine(::testing::ValuesIn(mydata2),
					    ::testing::Values("ymsat2"),
					    ::testing::Values("ffr", "mffc"),
					    ::testing::Values("engine", "struct_enc", "enc"),
					    ::testing::Values(FaultType::StuckAt, FaultType::TransitionDelay),
					    ::testing::Values("just1", "just2")));

END_NAMESPACE_DRUID
