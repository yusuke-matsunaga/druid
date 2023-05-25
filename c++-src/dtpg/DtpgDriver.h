#ifndef DTPGDRIVER_H
#define DTPGDRIVER_H

/// @file DtpgDriver.h
/// @brief DtpgDriver のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "DtpgMgr.h"
#include "DtpgStats.h"
#include "Justifier.h"
#include "ym/SatInitParam.h"
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
    DtpgMgr& mgr,                  ///< [in] 親のマネージャ
    const string& dtpg_type,       ///< [in] DTPGのタイプ
    const TpgNetwork& network,     ///< [in] 対象のネットワーク
    bool has_prev_state,	   ///< [in] 1時刻前の回路を持つ時 true
    const string& just_type,       ///< [in] 正当化のタイプ
    const SatInitParam& init_param ///< [in] SATソルバのタイプ
  );


public:
  //////////////////////////////////////////////////////////////////////
  // コンストラクタ/デストラクタ
  //////////////////////////////////////////////////////////////////////

  /// @brief コンストラクタ
  DtpgDriver(
    DtpgMgr& mgr,                  ///< [in] 親のマネージャ
    const TpgNetwork& network,     ///< [in] 対象のネットワーク
    bool has_prev_state,	   ///< [in] 1時刻前の回路を持つ時 true
    const string& just_type,       ///< [in] 正当化のタイプ
    const SatInitParam& init_param ///< [in] SATソルバのタイプ
  ) : mMgr{mgr},
      mNetwork{network},
      mHasPrevState{has_prev_state},
      mJustifier{just_type, network},
      mInitParam{init_param}
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
  DtpgMgr&
  mgr()
  {
    return mMgr;
  }

  /// @brief 故障マネージャを返す．
  TpgFaultMgr&
  fault_mgr()
  {
    return mMgr.fault_mgr();
  }

  /// @brief ネットワークを返す．
  const TpgNetwork&
  network() const
  {
    return mNetwork;
  }

  /// @brief 1時刻前の回路を持つ時 true を返す．
  bool
  has_prev_state() const
  {
    return mHasPrevState;
  }

  /// @brief 正当化を行う．
  TestVector
  justify(
    const NodeValList& assign_list, ///< [in] 割り当てリスト
    const VidMap& hvar_map,         ///< [in] 1時刻前の変数マップ
    const VidMap& gvar_map,         ///< [in] 現時刻の変数マップ
    const SatModel& sat_model       ///< [in] SATのモデル
  );

  /// @brief SATソルバの初期化パラメータを返す．
  SatInitParam
  sat_init_param() const
  {
    return mInitParam;
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
    const TpgFault& fault, ///< [in] 対象の故障
    const TestVector& tv,  ///< [in] テストパタン
    double sat_time,       ///< [in] SATにかかった時間
    double backtrace_time  ///< [in] バックトレースにかかった時間
  )
  {
    mgr().update_det(fault, tv, sat_time, backtrace_time);
  }

  /// @brief 冗長故障の特定が行えた時の結果を更新する．
  void
  update_untest(
    const TpgFault& fault, ///< [in] 対象の故障
    double sat_time        ///< [in] SATにかかった時間
  )
  {
    mgr().update_untest(fault, sat_time);
  }

  /// @brief アボートした時の結果を更新する．
  void
  update_abort(
    const TpgFault& fault, ///< [in] 対象の故障
    double sat_time        ///< [in] SATにかかった時間
  )
  {
    mgr().update_abort(fault, sat_time);
  }

  /// @brief SATの統計情報を更新する．
  void
  update_sat_stats(
    const SatStats& sat_stats ///< [in] 統計情報
  )
  {
    mgr().update_sat_stats(sat_stats);
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 親の DtpgMgr
  DtpgMgr& mMgr;

  // 対象のネットワーク
  const TpgNetwork& mNetwork;

  // 1時刻前のの回路を持つ時 true にするフラグ
  bool mHasPrevState;

  // 正当化を行うファンクタ
  Justifier mJustifier;

  // SATソルバの初期化パラメータ
  SatInitParam mInitParam;

  // 時間計測用のタイマー
  Timer mTimer;

};

END_NAMESPACE_DRUID

#endif // DTPGDRIVER_H
