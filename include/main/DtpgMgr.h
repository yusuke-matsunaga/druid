#ifndef DTPGMGR_H
#define DTPGMGR_H

/// @file DtpgMgr.h
/// @brief DtpgMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "TpgFaultMgr.h"
#include "Fsim.h"
#include "TestVector.h"
#include "DtpgStats.h"
#include "DopVerifyResult.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

class DtpgDriver;

//////////////////////////////////////////////////////////////////////
/// @class DtpgMgr DtpgMgr.h "DtpgMgr.h"
/// @brief テストパタン生成を行う本体
///
/// 実際の処理は個々の部品クラスが行う．
//////////////////////////////////////////////////////////////////////
class DtpgMgr
{
public:

  /// @brief コンストラクタ
  DtpgMgr(
    const TpgNetwork& network, ///< [in] 対象のネットワーク
    TpgFaultMgr& fault_mgr,    ///< [in] 故障マネージャ
    const JsonValue& option    ///< [in] オプションを表す JSON オブジェクト
  );

  /// @brief デストラクタ
  ~DtpgMgr();


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

  /// @brief 組み込み型の DetectOp を登録する．
  void
  add_dop(
    const JsonValue& js_obj ///< [in] オペレータを指定するオブジェクト
  );

  /// @brief UntestOp を登録する．
  void
  add_uop(
    UntestOp* uop ///< [in] 追加するオペレーター
  )
  {
    mUopList.push_back(uop);
  }

  /// @brief 組み込み型の UntestOp を登録する．
  void
  add_uop(
    const JsonValue& js_obj ///< [in] オペレータを指定するオブジェクト
  );

  /// @brief 対象のネットワークを返す．
  const TpgNetwork&
  network() const
  {
    return mNetwork;
  }

  /// @brief 故障マネージャを返す．
  TpgFaultMgr&
  fault_mgr()
  {
    return mFaultMgr;
  }

  /// @brief 故障シミュレーターを返す．
  Fsim&
  fsim()
  {
    return mFsim;
  }

  /// @brief 全故障数を返す．
  SizeType
  fault_num() const
  {
    return mFaultMgr.rep_fault_list().size();
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

  /// @brief 検証結果を返す．
  const DopVerifyResult
  verify_result() const
  {
    return mVerifyResult;
  }

  /// @brief テストパタン生成が成功した時の結果を更新する．
  void
  update_det(
    const TpgFault& fault, ///< [in] 対象の故障
    const TestVector& tv,  ///< [in] テストパタン
    double sat_time,       ///< [in] SATにかかった時間
    double backtrace_time  ///< [in] バックトレースにかかった時間
  );

  /// @brief 冗長故障の特定が行えた時の結果を更新する．
  void
  update_untest(
    const TpgFault& fault, ///< [in] 対象の故障
    double sat_time        ///< [in] SATにかかった時間
  );

  /// @brief アボートした時の結果を更新する．
  void
  update_abort(
    const TpgFault& fault, ///< [in] 対象の故障
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


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象のネットワーク
  const TpgNetwork& mNetwork;

  // 故障マネージャ
  TpgFaultMgr& mFaultMgr;

  // 故障シミュレータ
  Fsim mFsim;

  // 生成されたテストパタンのリスト
  vector<TestVector> mTVList;

  // DTPG の統計情報
  DtpgStats mStats;

  // 検証結果
  DopVerifyResult mVerifyResult;

  // テストパタン生成を行う本体
  DtpgDriver* mDriver{nullptr};

  // DetectOp のリスト
  vector<DetectOp*> mDopList;

  // UntestOp のリスト
  vector<UntestOp*> mUopList;

};

END_NAMESPACE_DRUID

#endif // DTPGMGR_H
