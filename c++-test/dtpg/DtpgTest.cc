
/// @file DtpgTest.cc
/// @brief DtpgTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "DtpgTest.h"
#include "Dtpg_se.h"
#include "DtpgFFR.h"
#include "DtpgMFFC.h"
#include "MF_Dtpg.h"

#include "TpgMFFC.h"
#include "TpgFFR.h"
#include "TpgFault.h"
#include "TestVector.h"
#include "ym/StopWatch.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
// @param[in] network 対象のネットワーク
// @param[in] fault_type 故障の種類
// @param[in] just_type Justifier の種類を表す文字列
// @param[in] solver_type SATソルバのタイプ
DtpgTest::DtpgTest(const TpgNetwork& network,
		   FaultType fault_type,
		   const string& just_type,
		   const SatSolverType& solver_type) :
  mSolverType(solver_type),
  mNetwork(network),
  mFaultType(fault_type),
  mJustType(just_type),
  mFaultMgr(network)
{
  mFsim.init_fsim3(network, fault_type);
  mDop.add(new_DopVerify(mFsim, mVerifyResult));
}

// @brief デストラクタ
DtpgTest::~DtpgTest()
{
}

// @brief FFRモードのテストを行う．
// @return 検出故障数と冗長故障数を返す．
pair<int, int>
DtpgTest::ffr_test()
{
  mTimer.reset();
  mTimer.start();

  int detect_num = 0;
  int untest_num = 0;
  for ( auto& ffr: mNetwork.ffr_list() ) {
    Dtpg_se dtpg(mNetwork, mFaultType, ffr, mJustType, mSolverType);
    for ( auto fault: ffr.fault_list() ) {
      if ( mFaultMgr.get(fault) == FaultStatus::Undetected ) {
	TestVector testvect(mNetwork.input_num(), mNetwork.dff_num(), mFaultType);
	SatBool3 ans = dtpg.dtpg(fault, testvect);
	if ( ans == SatBool3::True ) {
	  ++ detect_num;
	  mDop(fault, testvect);
	}
	else if ( ans == SatBool3::False ) {
	  ++ untest_num;
	}
      }
    }
    mStats.merge(dtpg.stats());
  }

  mTimer.stop();

  int n = mVerifyResult.error_count();
  for ( int i = 0; i < n; ++ i ) {
    const TpgFault* f = mVerifyResult.error_fault(i);
    TestVector tv = mVerifyResult.error_testvector(i);
    cout << "Error: " << f->str() << " is not detected with "
	 << tv << endl;
  }

  return make_pair(detect_num, untest_num);
}

// @brief MFFCモードのテストを行う．
// @return 検出故障数と冗長故障数を返す．
pair<int, int>
DtpgTest::mffc_test()
{
  mTimer.reset();
  mTimer.start();

  int detect_num = 0;
  int untest_num = 0;
  for ( auto& mffc: mNetwork.mffc_list() ) {
    Dtpg_se dtpg(mNetwork, mFaultType, mffc, mJustType, mSolverType);
    for ( auto fault: mffc.fault_list() ) {
      if ( mFaultMgr.get(fault) == FaultStatus::Undetected ) {
	// 故障に対するテスト生成を行なう．
	TestVector testvect(mNetwork.input_num(), mNetwork.dff_num(), mFaultType);
	SatBool3 ans = dtpg.dtpg(fault, testvect);
	if ( ans == SatBool3::True ) {
	  ++ detect_num;
	  mDop(fault, testvect);
	}
	else if ( ans == SatBool3::False ) {
	  ++ untest_num;
	}
      }
    }
    mStats.merge(dtpg.stats());
  }

  mTimer.stop();

  int n = mVerifyResult.error_count();
  for ( int i = 0; i < n; ++ i ) {
    const TpgFault* f = mVerifyResult.error_fault(i);
    TestVector tv = mVerifyResult.error_testvector(i);
    cout << "Error: " << f->str() << " is not detected with "
	 << tv << endl;
  }

  return make_pair(detect_num, untest_num);
}

// @brief FFRモードのテストを行う．
// @return 検出故障数と冗長故障数を返す．
pair<int, int>
DtpgTest::ffr_new_test()
{
  mTimer.reset();
  mTimer.start();

  mDetectNum = 0;
  mUntestNum = 0;
  for ( auto& ffr: mNetwork.ffr_list() ) {
    DtpgFFR dtpg(mNetwork, mFaultType, ffr, mJustType, mSolverType);
    for ( auto fault: ffr.fault_list() ) {
      if ( mFaultMgr.get(fault) == FaultStatus::Undetected ) {
	DtpgResult result = dtpg.gen_pattern(fault);
	update_result(fault, result);
      }
    }
    mStats.merge(dtpg.stats());
  }

  mTimer.stop();

  int n = mVerifyResult.error_count();
  for ( int i = 0; i < n; ++ i ) {
    const TpgFault* f = mVerifyResult.error_fault(i);
    TestVector tv = mVerifyResult.error_testvector(i);
    cout << "Error: " << f->str() << " is not detected with "
	 << tv << endl;
  }

  return make_pair(mDetectNum, mUntestNum);
}

// @brief MFFCモードのテストを行う．
// @return 検出故障数と冗長故障数を返す．
pair<int, int>
DtpgTest::mffc_new_test()
{
  mTimer.reset();
  mTimer.start();

  mDetectNum = 0;
  mUntestNum = 0;
  for ( auto& mffc: mNetwork.mffc_list() ) {
    DtpgMFFC dtpg(mNetwork, mFaultType, mffc, mJustType, mSolverType);
    for ( auto fault: mffc.fault_list() ) {
      if ( mFaultMgr.get(fault) == FaultStatus::Undetected ) {
	// 故障に対するテスト生成を行なう．
	DtpgResult result = dtpg.gen_pattern(fault);
	update_result(fault, result);
      }
    }
    mStats.merge(dtpg.stats());
  }

  mTimer.stop();

  int n = mVerifyResult.error_count();
  for ( int i = 0; i < n; ++ i ) {
    const TpgFault* f = mVerifyResult.error_fault(i);
    TestVector tv = mVerifyResult.error_testvector(i);
    cout << "Error: " << f->str() << " is not detected with "
	 << tv << endl;
  }

  return make_pair(mDetectNum, mUntestNum);
}

// @brief テストを行う．
// @return 検出故障数と冗長故障数を返す．
pair<int, int>
DtpgTest::mf_test()
{
  mTimer.reset();
  mTimer.start();

  mDetectNum = 0;
  mUntestNum = 0;
  for ( auto fault: mNetwork.rep_fault_list() ) {
    if ( mFaultMgr.get(fault) == FaultStatus::Undetected ) {
      MF_Dtpg dtpg(mNetwork, mFaultType, mJustType, mSolverType);
      DtpgResult result = dtpg.gen_pattern(vector<const TpgFault*>{fault});
      update_result(fault, result);
      mStats.merge(dtpg.stats());
    }
  }

  mTimer.stop();

  int n = mVerifyResult.error_count();
  for ( int i = 0; i < n; ++ i ) {
    const TpgFault* f = mVerifyResult.error_fault(i);
    TestVector tv = mVerifyResult.error_testvector(i);
    cout << "Error: " << f->str() << " is not detected with "
	 << tv << endl;
  }

  return make_pair(mDetectNum, mUntestNum);
}

// @brief 一つの故障に対する処理
void
DtpgTest::update_result(const TpgFault* fault,
			const DtpgResult& result)
{
  switch ( result.status() ) {
  case FaultStatus::Detected:
    ++ mDetectNum;
    mDop(fault, result.testvector());
    break;

  case FaultStatus::Untestable:
    ++ mUntestNum;
    break;

  case FaultStatus::Undetected:
    break;
  }
}

// @brief 検証結果を得る．
const DopVerifyResult&
DtpgTest::verify_result() const
{
  return mVerifyResult;
}

// @brief 統計情報を出力する．
void
DtpgTest::print_stats(int detect_num,
		      int untest_num)
{
  USTime time = mTimer.time();

  cout << "# of inputs             = " << mNetwork.input_num() << endl
       << "# of outputs            = " << mNetwork.output_num() << endl
       << "# of DFFs               = " << mNetwork.dff_num() << endl
       << "# of logic gates        = " << mNetwork.node_num() - mNetwork.ppi_num() << endl
       << "# of MFFCs              = " << mNetwork.mffc_num() << endl
       << "# of FFRs               = " << mNetwork.ffr_num() << endl
       << "# of total faults       = " << mNetwork.rep_fault_num() << endl
       << "# of detected faults    = " << detect_num << endl
       << "# of untestable faults  = " << untest_num << endl
       << "Total CPU time          = " << time << endl;

  ios::fmtflags save = cout.flags();
  cout.setf(ios::fixed, ios::floatfield);
  if ( mStats.mCnfGenCount > 0 ) {
    cout << "CNF generation" << endl
	 << "  " << setw(10) << mStats.mCnfGenCount
	 << "  " << mStats.mCnfGenTime
	 << "  " << setw(8) << mStats.mCnfGenTime.usr_time_usec() / mStats.mCnfGenCount
	 << "u usec"
	 << "  " << setw(8) << mStats.mCnfGenTime.sys_time_usec() / mStats.mCnfGenCount
	 << "s usec" << endl;
  }
  if ( mStats.mDetCount > 0 ) {
    cout << endl
	 << "*** SAT instances (" << mStats.mDetCount << ") ***" << endl
	 << "Total CPU time  (s)            = " << setw(10) << mStats.mDetTime.usr_time() << "u"
	 << " " << setw(8) << mStats.mDetTime.sys_time() << "s" << endl
	 << "Ave. CPU time (usec)           = "
	 << setw(10) << mStats.mDetTime.usr_time_usec() / mStats.mDetCount
	 << "u"
	 << " " << setw(8) << mStats.mDetTime.sys_time_usec() / mStats.mDetCount
	 << "s" << endl

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
	 << "Total CPU time  (s)            = " << setw(10) << mStats.mRedTime.usr_time() << "u"
	 << " " << setw(8) << mStats.mRedTime.sys_time() << "s" << endl
	 << "Ave. CPU time (usec)           = "
	 << setw(10) << mStats.mRedTime.usr_time_usec() / mStats.mRedCount
	 << "u"
	 << " " << setw(8) << mStats.mRedTime.sys_time_usec() / mStats.mRedCount
	 << "s" << endl

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
	 << "  " << setw(8) << mStats.mAbortTime.usr_time_usec() / mStats.mAbortCount
	 << "u usec"
	 << "  " << setw(8) << mStats.mAbortTime.sys_time_usec() / mStats.mAbortCount
	 << "s usec" << endl;
  }
  cout << endl
       << "*** backtrace time ***" << endl
       << "  " << mStats.mBackTraceTime
       << "  " << setw(8) << mStats.mBackTraceTime.usr_time_usec() / mStats.mDetCount
       << "u usec"
       << "  " << setw(8) << mStats.mBackTraceTime.sys_time_usec() / mStats.mDetCount
       << "s usec" << endl;
  cout.flags(save);
}

END_NAMESPACE_DRUID
