#ifndef DTPGRESULTS_H
#define DTPGRESULTS_H

/// @file DtpgResults.h
/// @brief DtpgResults のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "dtpg/DtpgStats.h"
#include "types/FaultStatus.h"
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
  DtpgResults(
    SizeType size ///< [in] 故障番号の最大値
  );

  /// @brief コピーコンストラクタ
  DtpgResults(
    const DtpgResults& src
  );

  /// @brief デストラクタ
  ~DtpgResults();


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
    const TpgFault& fault,     ///< [in] 対象の故障
    const SuffCond& cond,      ///< [in] 値割り当てのリスト
    const TestVector& testvect ///< [in] testvect テストベクタ
  );

  /// @brief テスト不能に設定する．
  ///
  /// 既に結果が登録されている場合には std::invalid_argument 例外が送出される．
  void
  set_untestable(
    const TpgFault& fault ///< [in] 対象の故障
  );

  /// @brief 統計情報をマージする．
  void
  merge_stats(
    const DtpgStats& stats ///< [in] 統計情報
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

  /// @brief 値割り当てを返す．
  const SuffCond&
  cond(
    const TpgFault& fault ///< [in] 対象の故障
  ) const;

  /// @brief テストベクタを返す．
  const TestVector&
  testvector(
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

  /// @brief 統計情報を得る．
  const DtpgStats&
  stats() const
  {
    return mStats;
  }

  /// @brief トータルの計算回数
  SizeType
  total_count() const { return mStats.total_count(); }

  /// @brief テスト生成が成功した回数を返す．
  SizeType
  detect_count() const { return mStats.detect_count(); }

  /// @brief テスト生成が成功した時の計算時間の合計を返す．
  double
  detect_time() const { return mStats.detect_time(); }

  /// @brief 冗長故障を特定した回数を返す．
  SizeType
  untest_count() const { return mStats.untest_count(); }

  /// @brief 冗長故障を特定した時の計算時間の合計を返す．
  double
  untest_time() const { return mStats.untest_time(); }

  /// @brief アボートした回数を返す．
  SizeType
  abort_count() const { return mStats.abort_count(); }

  /// @brief アボートした時の計算時間の合計を返す．
  double
  abort_time() const { return mStats.abort_time(); }

  /// @brief CNF の生成回数を返す．
  SizeType
  cnfgen_count() const { return mStats.cnfgen_count(); }

  /// @brief CNF の生成にかかった計算時間の合計を返す．
  double
  cnfgen_time() const { return mStats.cnfgen_time(); }

  /// @brief SAT の統計情報を返す．
  const SatStats&
  sat_stats() const { return mStats.sat_stats(); }

  /// @brief SAT の統計情報の最大値を返す．
  const SatStats&
  sat_stats_max() const { return mStats.sat_stats_max(); }

  /// @brief バックトレースにかかった計算時間の合計を返す．
  double
  backtrace_time() const { return mStats.backtrace_time(); }

  //////////////////////////////////////////////////////////////////////
  /// @}
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 結果が未設定かチェックする．
  void
  _check_result(
    const TpgFault& fault
  ) const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 結果の配列
  // キーは故障番号
  std::vector<std::unique_ptr<ResultRep>> mResultArray;

  // 統計情報
  DtpgStats mStats;

};

END_NAMESPACE_DRUID

#endif // DTPGRESULTS_H
