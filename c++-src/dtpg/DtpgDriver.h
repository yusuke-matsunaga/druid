#ifndef DTPGDRIVER_H
#define DTPGDRIVER_H

/// @file DtpgDriver.h
/// @brief DtpgDriver のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "TpgMgr.h"
#include "DtpgStats.h"
#include "ym/Timer.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DtpgDriver DtpgDriver.h "DtpgDriver.h"
/// @brief テストパタン生成を行うクラス
///
/// インターフェイスを定義するだけの純粋仮想クラス
//////////////////////////////////////////////////////////////////////
class DtpgDriver
{
public:
  //////////////////////////////////////////////////////////////////////
  // 派生クラスを作るクラスメソッド
  //////////////////////////////////////////////////////////////////////

  static
  DtpgDriver*
  new_driver(
    TpgMgr& mgr,                     ///< [in] 親のマネージャ
    const string& dtpg_type,         ///< [in] DTPGのタイプ
    const TpgNetwork& network,       ///< [in] 対象のネットワーク
    FaultType fault_type,            ///< [in] 故障の種類
    const string& just_type,         ///< [in] 正当化のタイプ
    const SatSolverType& solver_type ///< [in] SATソルバのタイプ
  );


public:
  //////////////////////////////////////////////////////////////////////
  // コンストラクタ/デストラクタ
  //////////////////////////////////////////////////////////////////////

  /// @brief コンストラクタ
  DtpgDriver(
    TpgMgr& mgr ///< [in] 親のマネージャ
  ) : mMgr{mgr}
  {
  }

  /// @brief デストラクタ
  virtual
  ~DtpgDriver() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テストパタン生成を行う．
  virtual
  void
  run() = 0;


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 親のマネージャを返す．
  TpgMgr&
  mgr()
  {
    return mMgr;
  }

  /// @brief 故障マネージャを返す．
  FaultStatusMgr&
  fault_status_mgr()
  {
    return mMgr.fault_status_mgr();
  }

  /// @brief CNF の生成開始
  void
  cnf_begin()
  {
    mTimer.reset();
    mTimer.start();
  }

  /// @brief CNF の生成終了
  void
  cnf_end()
  {
    mTimer.stop();
    mMgr.update_cnf(mTimer.get_time());
  }

  /// @brief テストパタン生成が成功した時の結果を更新する．
  void
  update_det(
    const TpgFault* fault, ///< [in] 対象の故障
    const TestVector& tv,  ///< [in] テストパタン
    double sat_time,       ///< [in] SATにかかった時間
    double backtrace_time  ///< [in] バックトレースにかかった時間
  );

  /// @brief 冗長故障の特定が行えた時の結果を更新する．
  void
  update_untest(
    const TpgFault* fault, ///< [in] 対象の故障
    double sat_time        ///< [in] SATにかかった時間
  );

  /// @brief アボートした時の結果を更新する．
  void
  update_abort(
    const TpgFault* fault, ///< [in] 対象の故障
    double sat_time        ///< [in] SATにかかった時間
  );

  /// @brief SATの統計情報を更新する．
  void
  update_sat_stats(
    const SatStats& sat_stats ///< [in] 統計情報
  );

  void
  _update(
    const TpgFault* fault,
    const DtpgResult& result
  );

  /// @breif DTPG の統計情報をマージする．
  void
  _merge_stats(
    const DtpgStats& stats
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 親の TpgMgr
  TpgMgr& mMgr;

  // 時間計測用のタイマー
  Timer mTimer;

};

END_NAMESPACE_DRUID

#endif // DTPGDRIVER_H
