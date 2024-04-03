#ifndef DTPGMGR_H
#define DTPGMGR_H

/// @file DtpgMgr.h
/// @brief DtpgMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023, 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "Fsim.h"
#include "TestVector.h"
#include "TpgFault.h"
#include "DtpgStats.h"
#include "DopVerifyResult.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DtpgMgr DtpgMgr.h "DtpgMgr.h"
/// @brief テストパタン生成を行う本体
///
/// 基本的には与えられた全ての故障を検出するためのテストベクタの生成を行う
/// だけの関数．
/// ただし，ある故障に対するテストベクタが求まった時点でそのテストベクタで
/// 故障シミュレーションを行って検出できる故障を除外するなどの追加の処理
/// を行えるように故障検出時などに呼び出されるコールバック関数を引数にとる．
/// また，検出済みとなった故障の状態を保持するためにクラスの形を取っている．
//////////////////////////////////////////////////////////////////////
class DtpgMgr
{
public:

  /// @brief 故障とテストベクタを引数にとる関数
  using FaultTvCallback = std::function<void(const TpgFault*, TestVector)>;

  /// @brief 故障を引数にとる関数
  using FaultCallback = std::function<void(const TpgFault*)>;


public:
  //////////////////////////////////////////////////////////////////////
  // コンストラクタ/デストラクタ
  //////////////////////////////////////////////////////////////////////

  /// @brief コンストラクタ
  DtpgMgr(
  );

  /// @brief デストラクタ
  ~DtpgMgr() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テスト生成を行う．
  ///
  /// 基本的には fault_list に含まれる故障を対象とするが，
  /// status_mgr 上で Detected/Untestable とマークされている故障は
  /// スキップする．
  /// status_mgr は const ではないのでテスト生成の途中で故障の状態は
  /// 変化しうる(生成されたテストベクタを用いた故障シミュレーションに
  /// よる故障ドロップなど)
  static
  DtpgStats
  run(
    const TpgNetwork& network,                 ///< [in] 対象のネットワーク
    const vector<const TpgFault*>& fault_list, ///< [in] 対象の故障リスト
    TpgFaultStatusMgr& status_mgr,             ///< [inout] 故障の状態を表すオブジェクト
    const JsonValue& option,                   ///< [in] オプションを表す JSON オブジェクト
    FaultTvCallback det_func,                  ///< [in] 検出時に呼ばれる関数
    FaultCallback untest_func,                 ///< [in] 検出不能の判定時に呼ばれる関数
    FaultCallback abort_func                   ///< [in] アボート時に呼ばれる関数
  );

#if 0
  /// @brief テストパタンのリストを返す．
  vector<TestVector>&
  tv_list()
  {
    return mTVList;
  }

  /// @brief 検証結果を返す．
  const DopVerifyResult
  verify_result() const
  {
    return mVerifyResult;
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

  /// @brief CNF 生成に関する情報を更新する．
  void
  update_cnf(
    double time ///< [in] 計算時間
  );

  /// @brief SATの統計情報を更新する．
  void
  update_sat_stats(
    const SatStats& sat_stats ///< [in] 統計情報
  );
#endif

};

END_NAMESPACE_DRUID

#endif // DTPGMGR_H
