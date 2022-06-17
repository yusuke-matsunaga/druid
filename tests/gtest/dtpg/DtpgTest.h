#ifndef DTPGTEST_H
#define DTPGTEST_H

/// @file DtpgTest.h
/// @brief DtpgTest のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "druid.h"
#include "TpgNetwork.h"
#include "FaultStatusMgr.h"
#include "Fsim.h"
#include "Justifier.h"
#include "DopList.h"
#include "DopVerifyResult.h"
#include "DtpgStats.h"
#include "ym/SatSolverType.h"
#include "ym/Timer.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DtpgTest DtpgTest.h "DtpgTest.h"
/// @brief Dtpg のテスト用クラス
//////////////////////////////////////////////////////////////////////
class DtpgTest
{
public:

  /// @brief コンストラクタ
  /// @param[in] network 対象のネットワーク
  /// @param[in] fault_type 故障の種類
  /// @param[in] just_type Justifier の種類を表す文字列
  /// @param[in] solver_type SATソルバのタイプ
  DtpgTest(const TpgNetwork& network,
	   FaultType fault_type,
	   const string& just_type,
	   const SatSolverType& solver_type = SatSolverType());

  /// @brief デストラクタ
  ~DtpgTest();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief FFRモードのテストを行う．
  /// @return 検出故障数と冗長故障数を返す．
  pair<int, int>
  ffr_test();

  /// @brief MFFCモードのテストを行う．
  /// @return 検出故障数と冗長故障数を返す．
  pair<int, int>
  mffc_test();

  /// @brief FFRモードのテストを行う．
  /// @return 検出故障数と冗長故障数を返す．
  pair<int, int>
  ffr_new_test();

  /// @brief MFFCモードのテストを行う．
  /// @return 検出故障数と冗長故障数を返す．
  pair<int, int>
  mffc_new_test();

  /// @brief 多重故障用のエンジンを用いてテストを行う．
  /// @return 検出故障数と冗長故障数を返す．
  pair<int, int>
  mf_test();

  /// @brief 検証結果を得る．
  const DopVerifyResult&
  verify_result() const;

  /// @brief 統計情報を出力する．
  void
  print_stats(int detect_num,
	      int untest_num);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 一つの故障に対する処理
  void
  update_result(const TpgFault* fault,
		const DtpgResult& result);


private:
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

  // 検証結果
  DopVerifyResult mVerifyResult;

  // 検出故障数
  int mDetectNum;

  // 検出不能故障数
  int mUntestNum;

  // 統計情報
  DtpgStats mStats;

  // タイマー
  Timer mTimer;

};

END_NAMESPACE_DRUID

#endif // DTPGTEST_H
