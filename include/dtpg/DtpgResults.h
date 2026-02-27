#ifndef DTPGRESULTS_H
#define DTPGRESULTS_H

/// @file DtpgResults.h
/// @brief DtpgResults のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "types/FaultStatus.h"
#include "types/TpgFault.h"
#include "types/TestVector.h"
#include "types/AssignList.h"
#include "ym/SatStats.h"


BEGIN_NAMESPACE_DRUID

class ResultRep;

//////////////////////////////////////////////////////////////////////
/// @class DtpgResults DtpgResults.h "dtpg/DtpgResults.h"
/// @brief DTPG の結果を表すクラス
/// @ingroup DtpgGroup
///
/// 具体的には各故障ごとに
/// - パタン生成の結果(検出，冗長，未検出)
/// - テストパタン/値割り当て
/// を持つ．
/// もちろん，テストパタン/値割り当ては検出できた時のみ意味を持つ．
///
/// 加えて統計情報も持つ．
//////////////////////////////////////////////////////////////////////
class DtpgResults
{
public:

  /// @brief 空のコンストラクタ
  ///
  /// 結果が登録されていない故障の状態は未検出となる．
  DtpgResults();

  /// @brief デストラクタ
  ~DtpgResults() = default;


public:
  //////////////////////////////////////////////////////////////////////
  /// @name 内容を設定する関数
  /// @{
  //////////////////////////////////////////////////////////////////////

  /// @brief クリアする．
  void
  clear();

  /// @brief 検出済みに設定する．
  ///
  /// 既に結果が登録されている場合には std::invalid_argument 例外が送出される．
  void
  set_detected(
    const TpgFault& fault,         ///< [in] 対象の故障
    const AssignList& assign_list, ///< [in] 値割り当てのリスト
    const TestVector& testvect     ///< [in] testvect テストベクタ
  );

  /// @brief テスト不能に設定する．
  ///
  /// 既に結果が登録されている場合には std::invalid_argument 例外が送出される．
  void
  set_untestable(
    const TpgFault& fault ///< [in] 対象の故障
  );

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

  /// @brief SAT の統計情報を更新する．
  void
  update_sat_stats(
    const SatStats& src_stats
  )
  {
    mSatStats += src_stats;
    mSatStatsMax.max_assign(src_stats);
  }

  /// @brief 内容をマージする．
  ///
  /// 両方にデータがある場合は src で上書きされる．
  void
  merge(
    const DtpgResults& src ///< [in] マージする元
  );

  //////////////////////////////////////////////////////////////////////
  /// @}
  //////////////////////////////////////////////////////////////////////


public:
  //////////////////////////////////////////////////////////////////////
  /// @name テスト生成の結果を取得する関数
  /// @{
  //////////////////////////////////////////////////////////////////////

  /// @brief 結果を返す．
  FaultStatus
  status(
    const TpgFault& fault ///< [in] 対象の故障
  ) const;

  /// @brief テストベクタを持つ時 true を返す．
  bool
  has_testvector(
    const TpgFault& fault ///< [in] 対象の故障
  ) const;

  /// @brief テストベクタを返す．
  ///
  /// has_testvector() == false の場合は std::logic_error 例外を送出する．
  const TestVector&
  testvector(
    const TpgFault& fault ///< [in] 対象の故障
  ) const;

  /// @brief 値割り当てを持つ時 true を返す．
  bool
  has_assign_list(
    const TpgFault& fault ///< [in] 対象の故障
  ) const;

  /// @brief 値割り当てを返す．
  ///
  /// has_assign_list() == false の場合は std::logic_error 例外を送出する．
  const AssignList&
  assign_list(
    const TpgFault& fault ///< [in] 対象の故障
  ) const;

  //////////////////////////////////////////////////////////////////////
  /// @}
  //////////////////////////////////////////////////////////////////////


public:
  //////////////////////////////////////////////////////////////////////
  /// @name 統計情報の取得用の関数
  /// @{
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

  //////////////////////////////////////////////////////////////////////
  /// @}
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 結果の辞書
  // キーは故障番号
  std::unordered_map<SizeType, ResultRep*> mResultDict;

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

#endif // DTPGRESULTS_H
