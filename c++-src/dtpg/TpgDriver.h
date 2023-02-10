#ifndef TPGDRIVER_H
#define TPGDRIVER_H

/// @file TpgDriver.h
/// @brief TpgDriver のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "TpgMgr.h"
#include "DtpgStats.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgDriver TpgDriver.h "TpgDriver.h"
/// @brief テストパタン生成を行うクラス
///
/// インターフェイスを定義するだけの純粋仮想クラス
//////////////////////////////////////////////////////////////////////
class TpgDriver
{
public:
  //////////////////////////////////////////////////////////////////////
  // 派生クラスを作るクラスメソッド
  //////////////////////////////////////////////////////////////////////

  static
  TpgDriver*
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
  TpgDriver(
    TpgMgr& mgr ///< [in] 親のマネージャ
  ) : mMgr{mgr}
  {
  }

  /// @brief デストラクタ
  virtual
  ~TpgDriver() = default;


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

  /// @brief テストパタン生成の結果を更新する．
  void
  _update(
    const TpgFault* fault,   ///< [in] 対象の故障
    const DtpgResult& result ///< [in] 結果
  );

  /// @breif DTPG の統計情報をマージする．
  void
  _merge_stats(
    const DtpgStats& stats /// [in] DTPG の統計情報
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 親の TpgMgr
  TpgMgr& mMgr;

};

END_NAMESPACE_DRUID

#endif // TPGDRIVER_H
