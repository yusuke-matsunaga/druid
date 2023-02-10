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

class TpgDriver;

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

  /// @brief UntestOp を登録する．
  void
  add_uop(
    UntestOp* uop ///< [in] 追加するオペレーター
  )
  {
    mUopList.push_back(uop);
  }

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
    return mDetCount;
  }

  /// @brief 検出不能故障数を返す．
  SizeType
  untest_count() const
  {
    return mUntestCount;
  }

  /// @brief アボート故障数を返す．
  SizeType
  abort_count() const
  {
    return mAbortCount;
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

  /// @brief テストパタン生成後の情報の更新を行う．
  void
  _update(
    const TpgFault* fault,   ///< [in] 故障
    const DtpgResult& result ///< [in] テストパタン生成の結果
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

  // 検出故障数
  SizeType mDetCount;

  // 検出不能故障数
  SizeType mUntestCount;

  // アボート故障数
  SizeType mAbortCount;

  // 生成されたテストパタンのリスト
  vector<TestVector> mTVList;

  // DTPG の統計情報
  DtpgStats mStats;

  // テストパタン生成を行う本体
  TpgDriver* mDriver{nullptr};

  // DetectOp のリスト
  vector<DetectOp*> mDopList;

  // UntestOp のリスト
  vector<UntestOp*> mUopList;

};

END_NAMESPACE_DRUID

#endif // TPGMGR_H
