#ifndef DTPGSTATS_H
#define DTPGSTATS_H

/// @file DtpgStats.h
/// @brief DtpgStats のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ym/SatStats.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DtpgStats DtpgStats.h "DtpgStats.h"
/// @brief DTPG の統計情報を表すクラス
//////////////////////////////////////////////////////////////////////
struct DtpgStats
{

  /// @brief 空のコンストラクタ
  ///
  /// 適切な初期化を行う．
  DtpgStats()
  {
    clear();
  }

  /// @brief デストラクタ
  ~DtpgStats() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 取得用の関数
  //////////////////////////////////////////////////////////////////////

  /// @brief トータルの計算回数
  SizeType
  total_count() const
  {
    return mDetCount + mUntestCount + mAbortCount;
  }

  /// @brief テスト生成が成功した回数を返す．
  SizeType
  detect_count() const { return mDetCount; }

  /// @brief テスト生成が成功した時の計算時間の合計を返す．
  double
  detect_time() const { return mDetTime; }

  /// @brief 冗長故障を特定した回数を返す．
  SizeType
  untest_count() const { return mUntestCount; }

  /// @brief 冗長故障を特定した時の計算時間の合計を返す．
  double
  untest_time() const { return mUntestTime; }

  /// @brief アボートした回数を返す．
  SizeType
  abort_count() const { return mAbortCount; }

  /// @brief アボートした時の計算時間の合計を返す．
  double
  abort_time() const { return mAbortTime; }

  /// @brief CNF の生成回数を返す．
  SizeType
  cnfgen_count() const { return mCnfGenCount; }

  /// @brief CNF の生成にかかった計算時間の合計を返す．
  double
  cnfgen_time() const { return mCnfGenTime; }

  /// @brief SAT の統計情報を返す．
  const SatStats&
  sat_stats() const { return mSatStats; }

  /// @brief SAT の統計情報の最大値を返す．
  const SatStats&
  sat_stats_max() const { return mSatStatsMax; }

  /// @brief バックトレースにかかった計算時間の合計を返す．
  double
  backtrace_time() const { return mBackTraceTime; }


public:
  //////////////////////////////////////////////////////////////////////
  // 設定用の関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 初期化する．
  void
  clear()
  {
    mDetCount = 0;
    mDetTime = 0.0;

    mUntestCount = 0;
    mUntestTime = 0.0;

    mAbortCount = 0;
    mAbortTime = 0.0;

    mCnfGenCount = 0;
    mCnfGenTime = 0.0;

    mSatStats.clear();
    mSatStatsMax.clear();

    mBackTraceTime = 0.0;
  }

  /// @brief DetStats を更新する
  void
  update_det(
    double sat_time,      ///< [in] SATにかかった時間
    double backtrace_time ///< [in] バックトレースにかかった時間
  )
  {
    ++ mDetCount;
    mDetTime += sat_time;
    mBackTraceTime += backtrace_time;
  }

  /// @brief RedStats を更新する
  void
  update_untest(
    double time ///< [in] SATにかかった時間
  )
  {
    ++ mUntestCount;
    mUntestTime += time;
  }

  /// @brief AbortStats を更新する
  void
  update_abort(
    double time ///< [in] SATにかかった時間
  )
  {
    ++ mAbortCount;
    mAbortTime += time;
  }

  /// @brief CNF 生成の情報を更新する．
  void
  update_cnf(
    double time ///< [in] CNF生成にかかった時間
  )
  {
    ++ mCnfGenCount;
    mCnfGenTime += time;
  }

  /// @brief 情報をマージする．
  void
  merge(
    const DtpgStats& src
  )
  {
    mDetCount += src.mDetCount;
    mDetTime += src.mDetTime;
    mUntestCount += src.mUntestCount;
    mUntestTime += src.mUntestTime;
    mAbortCount += src.mAbortCount;
    mAbortTime += src.mAbortTime;
    mCnfGenCount += src.mCnfGenCount;
    mCnfGenTime += src.mCnfGenTime;
    mSatStats += src.mSatStats;
    mSatStatsMax.max_assign(src.mSatStatsMax);
    mBackTraceTime += src.mBackTraceTime;
  }

  /// @brief SAT の統計情報を更新する．
  void
  update_sat_stats(
    const SatStats& src_stats
  )
  {
    mSatStats += src_stats;
    mSatStatsMax.max_assign(src_stats);
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  /// @brief テスト生成に成功した回数．
  SizeType mDetCount;

  /// @brief テスト生成に成功した時の SAT に要した時間
  double mDetTime;

  /// @brief 冗長故障と判定した回数
  SizeType mUntestCount;

  /// @brief 冗長故障と判定した時の SAT に要した時間
  double mUntestTime;

  /// @brief アボートした回数
  SizeType mAbortCount;

  /// @brief アボートした時の SAT に要した時間
  double mAbortTime;

  /// @brief CNF 式を生成した回数
  SizeType mCnfGenCount;

  /// @brief SATソルバの統計情報の和
  SatStats mSatStats;

  /// @brief SATソルバの統計情報の最大値
  ///
  /// 個々の値は同時に起こったわけではない．
  SatStats mSatStatsMax;

  /// @brief CNF 式の生成に費やした時間
  double mCnfGenTime;

  /// @brief バックトレースに要した時間
  double mBackTraceTime;

};

END_NAMESPACE_DRUID

#endif // DTPGSTATS_H
