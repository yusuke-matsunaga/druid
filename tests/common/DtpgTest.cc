
/// @file DtpgTest.cc
/// @brief DtpgTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgTest.h"
#include "DtpgTest_ffr.h"
#include "DtpgTest_ffr_new.h"
#include "DtpgTest_mffc.h"
#include "DtpgTest_mffc_new.h"
#include "TpgFault.h"
#include "TestVector.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
DtpgTest::DtpgTest(
  const TpgNetwork& network,
  FaultType fault_type,
  const string& just_type,
  const SatSolverType& solver_type
) : mSolverType{solver_type},
    mNetwork{network},
    mFaultType{fault_type},
    mJustType{just_type},
    mFaultMgr{network},
    mFsim{network, fault_type, true}
{
  mDop.add(new_DopVerify(mFsim, mVerifyResult));
}

// @brief デストラクタ
DtpgTest::~DtpgTest()
{
}

// @brief テスト用のインスタンスを作る．
DtpgTest*
DtpgTest::new_test(
  const string& mode,
  const TpgNetwork& network,
  FaultType fault_type,
  const string& just_type,
  const SatSolverType& solver_type
)
{
  if ( mode == "ffr_se" ) {
    return new DtpgTest_ffr(network, fault_type, just_type, solver_type);
  }
  if ( mode == "ffr" ) {
    return new DtpgTest_ffr_new(network, fault_type, just_type, solver_type);
  }
  if ( mode == "mffc_se" ) {
    return new DtpgTest_mffc(network, fault_type, just_type, solver_type);
  }
  if ( mode == "mffc" ) {
    return new DtpgTest_mffc_new(network, fault_type, just_type, solver_type);
  }
  ASSERT_NOT_REACHED;
  return nullptr;
}

// @brief テストを行う．
DtpgCount
DtpgTest::do_test(
  bool verbose
)
{
  mTimer.reset();
  mTimer.start();

  mCount.mDetCount = 0;
  mCount.mUntestCount = 0;

  _main_body();

  mTimer.stop();

  if ( verbose ) {
    print_stats(mCount);
  }

  SizeType n = mVerifyResult.error_count();
  for ( SizeType i = 0; i < n; ++ i ) {
    const TpgFault* f = mVerifyResult.error_fault(i);
    TestVector tv = mVerifyResult.error_testvector(i);
    cout << "Error: " << f->str() << " is not detected with "
	 << tv << endl;
  }

  return mCount;
}

// @brief 統計情報を出力する．
void
DtpgTest::print_stats(
  const DtpgCount& count
)
{
  auto time = mTimer.get_time();

  int detect_num = count.mDetCount;
  int untest_num = count.mUntestCount;

  cout << "# of inputs             = " << mNetwork.input_num() << endl
       << "# of outputs            = " << mNetwork.output_num() << endl
       << "# of DFFs               = " << mNetwork.dff_num() << endl
       << "# of logic gates        = " << mNetwork.node_num() - mNetwork.ppi_num() << endl
       << "# of MFFCs              = " << mNetwork.mffc_num() << endl
       << "# of FFRs               = " << mNetwork.ffr_num() << endl
       << "# of total faults       = " << mNetwork.rep_fault_num() << endl
       << "# of detected faults    = " << detect_num << endl
       << "# of untestable faults  = " << untest_num << endl
       << "Total CPU time(s)       = " << (time / 1000.0) << endl;

  ios::fmtflags save = cout.flags();
  cout.setf(ios::fixed, ios::floatfield);
  if ( mStats.mCnfGenCount > 0 ) {
    cout << "CNF generation" << endl
	 << "  " << setw(10) << mStats.mCnfGenCount
	 << "  " << (mStats.mCnfGenTime / 1000.0)
	 << "  " << setw(8) << mStats.mCnfGenTime / mStats.mCnfGenCount
	 <<  endl;
  }
  if ( mStats.mDetCount > 0 ) {
    cout << endl
	 << "*** SAT instances (" << mStats.mDetCount << ") ***" << endl
	 << "Total CPU time  (s)            = "
	 << setw(10) << (mStats.mDetTime / 1000.0) << endl
	 << "Average CPU time (ms)          = "
	 << setw(10) << (mStats.mDetTime / mStats.mDetCount) << endl

	 << "# of restarts (Ave./Max)       = "
	 << setw(10) << (double) mStats.mDetStats.mRestart / mStats.mDetCount
	 << " / " << setw(8) << mStats.mDetStatsMax.mRestart << endl

	 << "# of conflicts (Ave./Max)      = "
	 << setw(10) << (double) mStats.mDetStats.mConflictNum / mStats.mDetCount
	 << " / " << setw(8) << mStats.mDetStatsMax.mConflictNum << endl

	 << "# of decisions (Ave./Max)      = "
	 << setw(10) << (double) mStats.mDetStats.mDecisionNum / mStats.mDetCount
	 << " / " << setw(8) << mStats.mDetStatsMax.mDecisionNum << endl

	 << "# of implications (Ave./Max)   = "
	 << setw(10) << (double) mStats.mDetStats.mPropagationNum / mStats.mDetCount
	 << " / " << setw(8) << mStats.mDetStatsMax.mPropagationNum << endl;
  }
  if ( mStats.mRedCount > 0 ) {
    cout << endl
	 << "*** UNSAT instances (" << mStats.mRedCount << ") ***" << endl
	 << "Total CPU time  (s)            = "
	 << setw(10) << (mStats.mRedTime / 1000.0) << endl
	 << "Average CPU time (ms)          = "
	 << setw(10) << mStats.mRedTime / mStats.mRedCount << endl

	 << "# of restarts (Ave./Max)       = "
	 << setw(10) << (double) mStats.mRedStats.mRestart / mStats.mRedCount
	 << " / " << setw(8) << mStats.mRedStatsMax.mRestart << endl

	 << "# of conflicts (Ave./Max)      = "
	 << setw(10) << (double) mStats.mRedStats.mConflictNum / mStats.mRedCount
	 << " / " << setw(8) << mStats.mRedStatsMax.mConflictNum << endl

	 << "# of decisions (Ave./Max)      = "
	 << setw(10) << (double) mStats.mRedStats.mDecisionNum / mStats.mRedCount
	 << " / " << setw(8) << mStats.mRedStatsMax.mDecisionNum << endl

	 << "# of implications (Ave./Max)   = "
	 << setw(10) << (double) mStats.mRedStats.mPropagationNum / mStats.mRedCount
	 << " / " << setw(8) << mStats.mRedStatsMax.mPropagationNum << endl;
  }
  if ( mStats.mAbortCount > 0 ) {
    cout << endl
	 << "*** ABORT instances ***" << endl
	 << "  " << setw(10) << mStats.mAbortCount
	 << "  " << mStats.mAbortTime
	 << "  " << setw(8) << mStats.mAbortTime / mStats.mAbortCount << endl;
  }
  cout << endl
       << "*** backtrace time ***" << endl
       << "  " << (mStats.mBackTraceTime / 1000.0)
       << "  " << setw(8) << mStats.mBackTraceTime / mStats.mDetCount << endl;
  cout.flags(save);
}

END_NAMESPACE_DRUID
