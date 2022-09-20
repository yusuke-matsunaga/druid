#ifndef DTPG_SE_H
#define DTPG_SE_H

/// @file Dtpg_se.h
/// @brief Dtpg_se のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"

#include "FaultType.h"
#include "Justifier.h"
#include "DtpgResult.h"
#include "DtpgStats.h"
#include "FaultStatus.h"
#include "StructEnc.h"
#include "ym/SatBool3.h"
#include "ym/Timer.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class Dtpg_se Dtpg_se.h "Dtpg_se.h"
/// @brief StructEnc を用いた DTPG エンジン
//////////////////////////////////////////////////////////////////////
class Dtpg_se
{
public:

  /// @brief コンストラクタ(ノードモード)
  Dtpg_se(
    const TpgNetwork& network,       ///< [in] 対象のネットワーク
    FaultType fault_type,	     ///< [in] 故障の種類
    const TpgNode* node,	     ///< [in] 故障のあるノード
    const string& just_type,	     ///< [in] Justifier の種類を表す文字列
    const SatSolverType& solver_type ///< [in] SATソルバの実装タイプ
    = SatSolverType()
  );

  /// @brief コンストラクタ(ffrモード)
  Dtpg_se(
    const TpgNetwork& network,       ///< [in] 対象のネットワーク
    FaultType fault_type,	     ///< [in] 故障の種類
    const TpgFFR& ffr,		     ///< [in] 故障伝搬の起点となる FFR
    const string& just_type,	     ///< [in] Justifier の種類を表す文字列
    const SatSolverType& solver_type ///< [in] SATソルバの実装タイプ
    = SatSolverType()
  );

  /// @brief コンストラクタ(mffcモード)
  ///
  /// この MFFC に含まれるすべての FFR が対象となる．
  /// FFR と MFFC が一致している場合は ffr モードと同じことになる．
  Dtpg_se(
    const TpgNetwork& network,       ///< [in] 対象のネットワーク
    FaultType fault_type,	     ///< [in] 故障の種類
    const TpgMFFC& mffc,	     ///< [in] 故障伝搬の起点となる MFFC
    const string& just_type,	     ///< [in] Justifier の種類を表す文字列
    const SatSolverType& solver_type ///< [in] SATソルバの実装タイプ
    = SatSolverType()
  );

  /// @brief デストラクタ
  ~Dtpg_se();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テスト生成を行なう．
  /// @return 結果を返す．
  DtpgResult
  gen_pattern(
    const TpgFault* fault ///< [in] 対象の故障
  );

  /// @brief DTPG の統計情報を返す．
  const DtpgStats&
  stats() const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief CNF 作成を開始する．
  void
  cnf_begin();

  /// @brief CNF 作成を終了する．
  void
  cnf_end();

  /// @brief 時間計測を開始する．
  void
  timer_start();

  /// @brief 時間計測を終了する．
  double
  timer_stop();


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 統計情報
  DtpgStats mStats;

  // StructEnc の本体
  StructEnc mStructEnc;

  // 故障の種類
  FaultType mFaultType;

  // バックトレーサー
  Justifier mJustifier;

  // 時間計測を行なうかどうかの制御フラグ
  bool mTimerEnable;

  // 時間計測用のタイマー
  Timer mTimer;

};

END_NAMESPACE_DRUID

#endif // DTPG_SE_H
