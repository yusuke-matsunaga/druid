
/// @file DtpgTest.cc
/// @brief DtpgTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "gtest/gtest.h"

#include "DtpgTest.h"


BEGIN_NAMESPACE_YM_SATPG

struct TestData
{
  TestData()
  {
  }

  // コンストラクタ
  TestData(const string& filename,
	   int total_num,
	   int sa_detect_num,
	   int td_detect_num,
	   int sa_untest_num,
	   int td_untest_num) :
    mFileName(filename),
    mTotalFaultNum(total_num),
    mSaDetectFaultNum(sa_detect_num),
    mTdDetectFaultNum(td_detect_num),
    mSaUntestFaultNum(sa_untest_num),
    mTdUntestFaultNum(td_untest_num)
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
operator<<(ostream& s,
	   TestData tdata)
{
  s << tdata.mFileName;
  return s;
}

TestData mydata[] = {
  TestData("s27.blif",     32,   32,   32,  0,   0),
  TestData("s1196.blif", 1242, 1242, 1241,  0,   1),
  TestData("s5378.blif", 4603, 4563, 4253, 40, 350)
};

class DtpgTestWithParam :
public ::testing::TestWithParam<std::tuple<TestData, string, FaultType, string> >
{
public:

  /// @brief コンストラクタ
  DtpgTestWithParam();

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

  /// @brief テストパラメータからテストモードを取り出す．
  string
  test_mode();

  /// @brief テストパラメータから FaultType を取り出す．
  FaultType
  fault_type();

  /// @brief テストパラメータから just_type を取り出す．
  string
  just_type();


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  string mSatType;

  string mSatOption;

  ostream* mSatOutp;

  TpgNetwork mNetwork;

  DtpgTest* mDtpgTest;

};


DtpgTestWithParam::DtpgTestWithParam() :
  mSatType(""),
  mSatOption(""),
  mSatOutp(nullptr),
  mDtpgTest(nullptr)
{
}

// @brief 初期化を行う．
void
DtpgTestWithParam::SetUp()
{
  bool stat = mNetwork.read_blif(filename());
  ASSERT_COND( stat );

  mDtpgTest = new DtpgTest(mSatType, mSatOption, mSatOutp, fault_type(), just_type(), mNetwork);
}

// @brief 終了処理を行う．
void
DtpgTestWithParam::TearDown()
{
  delete mDtpgTest;
  mDtpgTest = nullptr;
}

void
DtpgTestWithParam::do_test()
{
  string mode = test_mode();
  pair<int, int> num_pair;
  if ( mode == "ffr" ) {
    num_pair = mDtpgTest->ffr_test();
  }
  else if ( mode == "mffc" ) {
    num_pair = mDtpgTest->mffc_test();
  }
  else if ( mode == "ffr_new" ) {
    num_pair = mDtpgTest->ffr_new_test();
  }
  else if ( mode == "mffc_new" ) {
    num_pair = mDtpgTest->mffc_new_test();
  }
  else {
    ASSERT_NOT_REACHED;
  }

  EXPECT_EQ( total_fault_num(), mNetwork.rep_fault_num() );
  EXPECT_EQ( detect_fault_num(), num_pair.first );
  EXPECT_EQ( untest_fault_num(), num_pair.second );

  const DopVerifyResult& result = mDtpgTest->verify_result();
  EXPECT_EQ( 0, result.error_count() );
}

// @brief テストパラメータからファイル名を取り出す．
string
DtpgTestWithParam::filename()
{
  const TestData& data = std::get<0>(GetParam());
  return DATAPATH + data.mFileName;
}

// @brief テストパラメータから総故障数を取り出す．
int
DtpgTestWithParam::total_fault_num()
{
  const TestData& data = std::get<0>(GetParam());
  return data.mTotalFaultNum;
}

// @brief テストパラメータから検出可能故障数を取り出す．
int
DtpgTestWithParam::detect_fault_num()
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
DtpgTestWithParam::untest_fault_num()
{
  const TestData& data = std::get<0>(GetParam());
  if ( fault_type() == FaultType::StuckAt ) {
    return data.mSaUntestFaultNum;
  }
  else {
    return data.mTdUntestFaultNum;
  }
}

// @brief テストパラメータからテストモードを取り出す．
string
DtpgTestWithParam::test_mode()
{
  return std::get<1>(GetParam());
}

// @brief テストパラメータから FaultType を取り出す．
FaultType
DtpgTestWithParam::fault_type()
{
  return std::get<2>(GetParam());
}

// @brief テストパラメータから just_type を取り出す．
string
DtpgTestWithParam::just_type()
{
  return std::get<3>(GetParam());
}

TEST_P(DtpgTestWithParam, test1)
{
  do_test();
}

INSTANTIATE_TEST_CASE_P(DtpgTest, DtpgTestWithParam,
			::testing::Combine(::testing::ValuesIn(mydata),
					   ::testing::Values("ffr",    "ffr_new",
							     "mffc",   "mffc_new"),
					   ::testing::Values(FaultType::StuckAt, FaultType::TransitionDelay),
					   ::testing::Values("just1", "just2")));

END_NAMESPACE_YM_SATPG
