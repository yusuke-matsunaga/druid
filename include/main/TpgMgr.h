#ifndef TPGMGR_H
#define TPGMGR_H

/// @file TpgMgr.h
/// @brief TpgMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "FaultStatusMgr.h"
#include "Fsim.h"
#include "TestVector.h"
#include "DtpgStats.h"


BEGIN_NAMESPACE_DRUID

class DtpgDriver;

//////////////////////////////////////////////////////////////////////
/// @class TpgMgr TpgMgr.h "TpgMgr.h"
/// @brief テストパタン生成を行う本体
///
/// 実際の処理は個々の部品クラスが行う．
//////////////////////////////////////////////////////////////////////
class TpgMgr
{
public:

  /// @brief コンストラクタ
  TpgMgr(
    const TpgNetwork& network,       ///< [in] 対象のネットワーク
    FaultType fault_type,            ///< [in] 故障の種類
    const string& dtpg_type,         ///< [in] DTPGのタイプ
    const string& just_type,         ///< [in] 正当化のタイプ
    const SatSolverType& solver_type ///< [in] SATソルバのタイプ
  );

  /// @brief デストラクタ
  ~TpgMgr();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テスト生成を行う．
  void
  run();

  /// @brief DetectOp を登録する．
  void
  add_dop(
    DetectOp* dop ///< [in] 追加するオペレーター
  )
  {
    mDopList.push_back(dop);
  }

  /// @brief 'base' タイプの DetectOp を登録する．
  ///
  /// 結果は内部の FaultStatusMgr に反映される．
  void
  add_base_dop();

  /// @brief 'drop' タイプの DetectOp を登録する．
  ///
  /// 結果は内部の FaultStatusMgr に反映される．
  void
  add_drop_dop();

  /// @brief 'tvlist' タイプの DetectOp を登録する．
  ///
  /// 結果は内部の TVList に格納される．
  void
  add_tvlist_dop();

  /// @brief 'verify' タイプの DetectOp を登録する．
  void
  add_verify_dop(
    DopVerifyResult& verify_result ///< [in] 検証結果を格納するオブジェクト
  );

  /// @brief UntestOp を登録する．
  void
  add_uop(
    UntestOp* uop ///< [in] 追加するオペレーター
  )
  {
    mUopList.push_back(uop);
  }

  /// @brief 'base' タイプの UntestOp を登録する．
  void
  add_base_uop();

  /// @brief 対象のネットワークを返す．
  const TpgNetwork&
  network() const
  {
    return mNetwork;
  }

  /// @brief 故障マネージャを返す．
  FaultStatusMgr&
  fault_status_mgr()
  {
    return mFaultStatusMgr;
  }

  /// @brief 故障シミュレーターを返す．
  Fsim&
  fsim()
  {
    return mFsim;
  }

  /// @brief 検出故障数を返す．
  SizeType
  detect_count() const
  {
    return mStats.detect_count();
  }

  /// @brief 検出不能故障数を返す．
  SizeType
  untest_count() const
  {
    return mStats.untest_count();
  }

  /// @brief アボート故障数を返す．
  SizeType
  abort_count() const
  {
    return mStats.abort_count();
  }

  /// @brief テストパタンのリストを返す．
  vector<TestVector>&
  tv_list()
  {
    return mTVList;
  }

  /// @brief DTPG の統計情報を得る．
  const DtpgStats&
  dtpg_stats() const
  {
    return mStats;
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

  void
  _update(
    const TpgFault* fault,
    const DtpgResult& result
  );

  /// @brief DTPG の統計情報をマージする．
  void
  _merge_stats(
    const DtpgStats& stats ///< [in] DTPG の統計情報
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象のネットワーク
  const TpgNetwork& mNetwork;

  // 故障マネージャ
  FaultStatusMgr mFaultStatusMgr;

  // 故障シミュレータ
  Fsim mFsim;

  // 生成されたテストパタンのリスト
  vector<TestVector> mTVList;

  // DTPG の統計情報
  DtpgStats mStats;

  // テストパタン生成を行う本体
  DtpgDriver* mDriver{nullptr};

  // DetectOp のリスト
  vector<DetectOp*> mDopList;

  // UntestOp のリスト
  vector<UntestOp*> mUopList;

};

END_NAMESPACE_DRUID

#endif // TPGMGR_H
