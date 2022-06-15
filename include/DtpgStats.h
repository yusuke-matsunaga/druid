#ifndef DTPGSTATS_H
#define DTPGSTATS_H

/// @file DtpgStats.h
/// @brief DtpgStats のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2022 Yusuke Matsunaga
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

  /// @brief 初期化する．
  void
  clear()
  {
    mCnfGenCount = 0;
    mCnfGenTime = 0.0;

    mDetCount = 0;
    mDetTime = 0.0;
    mDetStats.clear();
    mDetStatsMax.clear();

    mRedCount = 0;
    mRedTime = 0.0;
    mRedStats.clear();
    mRedStatsMax.clear();

    mAbortCount = 0;
    mAbortTime = 0.0;
  }

  /// @brief DetStats を更新する
  void
  update_det(
    const SatStats& sat_stats,
    double time
  )
  {
    ++ mDetCount;
    mDetTime += time;
    mDetStats += sat_stats;
    mDetStatsMax.max_assign(sat_stats);
  }

  /// @brief RedStats を更新する
  void
  update_red(
    const SatStats& sat_stats,
    double time
  )
  {
    ++ mRedCount;
    mRedTime += time;
    mRedStats += sat_stats;
    mRedStatsMax.max_assign(sat_stats);
  }

  /// @brief AbortStats を更新する
  void
  update_abort(
    const SatStats& sat_stats,
    double time
  )
  {
    ++ mAbortCount;
    mAbortTime += time;
  }

  /// @brief マージする．
  void
  merge(
    const DtpgStats& src
  )
  {
    mCnfGenCount += src.mCnfGenCount;
    mCnfGenTime += src.mCnfGenTime;
    mDetCount += src.mDetCount;
    mDetTime += src.mDetTime;
    mDetStats += src.mDetStats;
    mDetStatsMax.max_assign(src.mDetStatsMax);
    mRedCount += src.mRedCount;
    mRedTime += src.mRedTime;
    mRedStats += src.mRedStats;
    mRedStatsMax.max_assign(src.mRedStatsMax);
    mAbortCount += src.mAbortCount;
    mAbortTime += src.mAbortTime;
    mBackTraceTime += src.mBackTraceTime;
  }

  /// @brief CNF 式を生成した回数
  int mCnfGenCount;

  /// @brief CNF 式の生成に費やした時間
  double mCnfGenTime;

  /// @brief テスト生成に成功した回数．
  int mDetCount;

  /// @brief テスト生成に成功した時の SAT に要した時間
  double mDetTime;

  /// @brief テスト生成に成功した時の SATソルバの統計情報の和
  SatStats mDetStats;

  /// @brief テスト生成に成功した時の SATソルバの統計情報の最大値
  ///
  /// 個々の値は同時に起こったわけではない．
  SatStats mDetStatsMax;

  /// @brief 冗長故障と判定した回数
  int mRedCount;

  /// @brief 冗長故障と判定した時の SAT に要した時間
  double mRedTime;

  /// @brief 冗長故障と判定した時の SATソルバの統計情報の和
  SatStats mRedStats;

  /// @brief 冗長故障と判定した時の SATソルバの統計情報の最大値
  ///
  /// 個々の値は同時に起こったわけではない．
  SatStats mRedStatsMax;

  /// @brief アボートした回数
  int mAbortCount;

  /// @brief アボートした時の SAT に要した時間
  double mAbortTime;

  /// @brief バックトレースに要した時間
  double mBackTraceTime;

};

#if 0
//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief 空のコンストラクタ
//
// 適切な初期化を行う．
inline
DtpgStats::DtpgStats()
{
  clear();
}

// @brief 初期化する．
inline
void
DtpgStats::clear()
{
  mCnfGenCount = 0;
  mCnfGenTime.set(0.0, 0.0, 0.0);

  mDetCount = 0;
  mDetTime.set(0.0, 0.0, 0.0);
  mDetStats.clear();
  mDetStatsMax.clear();

  mRedCount = 0;
  mRedTime.set(0.0, 0.0, 0.0);
  mRedStats.clear();
  mRedStatsMax.clear();

  mAbortCount = 0;
  mAbortTime.set(0.0, 0.0, 0.0);
}

// @brief DetStats を更新する
inline
void
DtpgStats::update_det(const SatStats& sat_stats,
		      const USTime& time)
{
  ++ mDetCount;
  mDetTime += time;
  mDetStats += sat_stats;
  mDetStatsMax.max_assign(sat_stats);
}

// @brief RedStats を更新する
inline
void
DtpgStats::update_red(const SatStats& sat_stats,
		      const USTime& time)
{
  ++ mRedCount;
  mRedTime += time;
  mRedStats += sat_stats;
  mRedStatsMax.max_assign(sat_stats);
}

// @brief AbortStats を更新する
inline
void
DtpgStats::update_abort(const SatStats& sat_stats,
			const USTime& time)
{
  ++ mAbortCount;
  mAbortTime += time;
}

// @brief マージする．
inline
void
DtpgStats::merge(const DtpgStats& src)
{
  mCnfGenCount += src.mCnfGenCount;
  mCnfGenTime += src.mCnfGenTime;
  mDetCount += src.mDetCount;
  mDetTime += src.mDetTime;
  mDetStats += src.mDetStats;
  mDetStatsMax.max_assign(src.mDetStatsMax);
  mRedCount += src.mRedCount;
  mRedTime += src.mRedTime;
  mRedStats += src.mRedStats;
  mRedStatsMax.max_assign(src.mRedStatsMax);
  mAbortCount += src.mAbortCount;
  mAbortTime += src.mAbortTime;
  mBackTraceTime += src.mBackTraceTime;
}
#endif

END_NAMESPACE_DRUID

#endif // DTPGSTATS_H
