#ifndef DTPGDRIVER_H
#define DTPGDRIVER_H

/// @file DtpgDriver.h
/// @brief DtpgDriver のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023, 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "DtpgMgr.h"
#include "DtpgStats.h"
#include "Justifier.h"
#include "ym/JsonValue.h"
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

  using FaultTvCallback = DtpgMgr::FaultTvCallback;
  using FaultCallback = DtpgMgr::FaultCallback;

public:
  //////////////////////////////////////////////////////////////////////
  // 派生クラスを作るクラスメソッド
  //////////////////////////////////////////////////////////////////////

  /// @brief FFRモード用のドライバを生成する．
  static
  DtpgDriver*
  new_driver(
    const TpgNetwork& network, ///< [in] 対象のネットワーク
    const TpgFFR* ffr,         ///< [in] 対象のFFR
    const JsonValue& option    ///< [in] オプションを表す JSON オブジェクト
  );

  /// @brief MFFCモード用のドライバを生成する．
  static
  DtpgDriver*
  new_driver(
    const TpgNetwork& network, ///< [in] 対象のネットワーク
    const TpgMFFC* mffc,       ///< [in] 対象のMFFC
    const JsonValue& option    ///< [in] オプションを表す JSON オブジェクト
  );


public:
  //////////////////////////////////////////////////////////////////////
  // コンストラクタ/デストラクタ
  //////////////////////////////////////////////////////////////////////

  /// @brief コンストラクタ
  DtpgDriver(
    const TpgNetwork& network,     ///< [in] 対象のネットワーク
    const string& just_type,       ///< [in] 正当化のタイプ
    const SatInitParam& init_param ///< [in] SATソルバのタイプ
  ) : mNetwork{network},
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
  DtpgStats
  gen_pattern(
    const TpgFault* fault,     ///< [in] 対象の故障
    FaultTvCallback det_func,  ///< [in] 検出時に呼び出される関数
    FaultCallback untest_func, ///< [in] 検出不能判定時に呼び出される関数
    FaultCallback abort_func   ///< [in] アボート時に呼び出される関数
  ) = 0;


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから用いられる関数
  //////////////////////////////////////////////////////////////////////

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

#if 0
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

  /// @brief SATの統計情報を更新する．
  void
  update_sat_stats(
    const SatStats& sat_stats ///< [in] 統計情報
  )
  {
    mgr().update_sat_stats(sat_stats);
  }
#endif

private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象のネットワーク
  const TpgNetwork& mNetwork;

  // 1時刻前のの回路を持つ時 true にするフラグ
  bool mHasPrevState;

  // 正当化を行うファンクタ
  Justifier mJustifier;

  // SATソルバの初期化パラメータ
  SatInitParam mInitParam;

#if 0
  // 時間計測用のタイマー
  Timer mTimer;

  // 統計情報
  DtpgStats* mStatsPtr{nullptr};
#endif

};

END_NAMESPACE_DRUID

#endif // DTPGDRIVER_H
