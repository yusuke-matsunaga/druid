#ifndef DTPG_SE_H
#define DTPG_SE_H

/// @file Dtpg_se.h
/// @brief Dtpg_se のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "druid.h"

#include "FaultType.h"
#include "Justifier.h"
#include "DtpgStats.h"
#include "FaultStatus.h"
#include "StructEnc.h"
#include "ym/SatBool3.h"
#include "ym/StopWatch.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class Dtpg_se Dtpg_se.h "Dtpg_se.h"
/// @brief StructEnc を用いた DTPG エンジン
//////////////////////////////////////////////////////////////////////
class Dtpg_se
{
public:

  /// @brief コンストラクタ(ノードモード)
  /// @param[in] network 対象のネットワーク
  /// @param[in] fault_type 故障の種類
  /// @param[in] node 故障のあるノード
  /// @param[in] just_type Justifier の種類を表す文字列
  /// @param[in] solver_type SATソルバの実装タイプ
  Dtpg_se(const TpgNetwork& network,
	  FaultType fault_type,
	  const TpgNode* node,
	  const string& just_type,
	  const SatSolverType& solver_type = SatSolverType());

  /// @brief コンストラクタ(ffrモード)
  /// @param[in] network 対象のネットワーク
  /// @param[in] fault_type 故障の種類
  /// @param[in] ffr 故障伝搬の起点となる FFR
  /// @param[in] just_type Justifier の種類を表す文字列
  /// @param[in] solver_type SATソルバの実装タイプ
  Dtpg_se(const TpgNetwork& network,
	  FaultType fault_type,
	  const TpgFFR& ffr,
	  const string& just_type,
	  const SatSolverType& solver_type = SatSolverType());

  /// @brief コンストラクタ(mffcモード)
  /// @param[in] network 対象のネットワーク
  /// @param[in] fault_type 故障の種類
  /// @param[in] mffc 故障伝搬の起点となる MFFC
  /// @param[in] just_type Justifier の種類を表す文字列
  /// @param[in] solver_type SATソルバの実装タイプ
  ///
  /// この MFFC に含まれるすべての FFR が対象となる．
  /// FFR と MFFC が一致している場合は ffr モードと同じことになる．
  Dtpg_se(const TpgNetwork& network,
	  FaultType fault_type,
	  const TpgMFFC& mffc,
	  const string& just_type,
	  const SatSolverType& solver_type = SatSolverType());

  /// @brief デストラクタ
  ~Dtpg_se();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テスト生成を行なう．
  /// @param[in] fault 対象の故障
  /// @param[out] testvect テストパタンを格納する変数
  /// @return 結果を返す．
  SatBool3
  dtpg(const TpgFault* fault,
       TestVector& testvect);

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
  USTime
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
  StopWatch mTimer;

};

END_NAMESPACE_DRUID

#endif // DTPG_SE_H
