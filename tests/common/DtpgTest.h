#ifndef DTPGTEST_H
#define DTPGTEST_H

/// @file DtpgTest.h
/// @brief DtpgTest のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "TpgNetwork.h"
#include "FaultStatusMgr.h"
#include "Fsim.h"
#include "Justifier.h"
#include "DopList.h"
#include "DopVerifyResult.h"
#include "DtpgResult.h"
#include "DtpgStats.h"
#include "ym/SatSolverType.h"
#include "ym/Timer.h"


BEGIN_NAMESPACE_DRUID

struct DtpgCount
{
  int mDetCount{0};
  int mUntestCount{0};
  int mAbortCount{0};
};


//////////////////////////////////////////////////////////////////////
/// @class DtpgTest DtpgTest.h "DtpgTest.h"
/// @brief Dtpg のテスト用クラス
//////////////////////////////////////////////////////////////////////
class DtpgTest
{
public:

  /// @brief コンストラクタ
  DtpgTest(
    const TpgNetwork& network,       ///< [in] network 対象のネットワーク
    FaultType fault_type,	     ///< [in] fault_type 故障の種類
    const string& just_type,	     ///< [in] just_type Justifier の種類を表す文字列
    const SatSolverType& solver_type ///< [in] solver_type SATソルバのタイプ
    = SatSolverType()
  );

  /// @brief デストラクタ
  virtual
  ~DtpgTest();

  /// @brief テスト用のインスタンスを作る．
  static
  DtpgTest*
  new_test(
    const string& mode,              ///< [in] モード名
    const TpgNetwork& network,       ///< [in] network 対象のネットワーク
    FaultType fault_type,	     ///< [in] fault_type 故障の種類
    const string& just_type,	     ///< [in] just_type Justifier の種類を表す文字列
    const SatSolverType& solver_type ///< [in] solver_type SATソルバのタイプ
    = SatSolverType()
  );


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テストを行う．
  /// @return 検出故障数と冗長故障数を返す．
  DtpgCount
  do_test(
    bool verbose ///< [n] verboseフラグ
  );

  /// @brief 検証結果を得る．
  const DopVerifyResult&
  verify_result() const
  {
    return mVerifyResult;
  }


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief テスト生成の結果を更新する．
  void
  _update(
    const TpgFault* fault,
    const DtpgResult& result
  )
  {
    switch ( result.status() ) {
    case FaultStatus::Detected:
      mDop(fault, result.testvector());
      ++ mCount.mDetCount;
      break;

    case FaultStatus::Untestable:
      ++ mCount.mUntestCount;
      break;

    case FaultStatus::Undetected:
      ++ mCount.mAbortCount;
      break;
    }
  }

  /// @brief 統計情報をマージする．
  void
  _merge_stats(
    const DtpgStats& stats
  )
  {
    mStats.merge(stats);
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 実際にテスト生成を行う本体
  virtual
  void
  _main_body() = 0;

  /// @brief 統計情報を出力する．
  void
  print_stats(
    const DtpgCount& count
  );


protected:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // SAT ソルバの種類
  SatSolverType mSolverType;

  // 対象のネットワーク
  const TpgNetwork& mNetwork;

  // 故障の種類
  FaultType mFaultType;

  // Justifier の種類
  string mJustType;

  // 故障マネージャ
  FaultStatusMgr mFaultMgr;

  // 故障シミュレータ
  Fsim mFsim;

  // バックトレーサー
  Justifier* mJustifier;

  // 故障検出時に起動されるファンクタのリスト
  DopList mDop;

  // 故障数のカウンタ
  DtpgCount mCount;

  // 検証結果
  DopVerifyResult mVerifyResult;

  // 統計情報
  DtpgStats mStats;

  // タイマー
  Timer mTimer;

};

END_NAMESPACE_DRUID

#endif // DTPGTEST_H
