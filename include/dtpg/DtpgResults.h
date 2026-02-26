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
//////////////////////////////////////////////////////////////////////
class DtpgResults
{
public:

  /// @brief 空のコンストラクタ
  ///
  /// 結果が登録されていない故障の状態は未検出となる．
  DtpgResults() = default;

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
    const TpgFault& fault,      ///< [in] 対象の故障
    const TestVector& testvect  ///< [in] testvect テストベクタ
  );

  /// @brief 検出済みに設定する．
  ///
  /// 既に結果が登録されている場合には std::invalid_argument 例外が送出される．
  void
  set_detected(
    const TpgFault& fault,        ///< [in] 対象の故障
    const AssignList& assign_list ///< [in] 値割り当てのリスト
  );

  /// @brief テスト不能に設定する．
  ///
  /// 既に結果が登録されている場合には std::invalid_argument 例外が送出される．
  void
  set_untestable(
    const TpgFault& fault ///< [in] 対象の故障
  );

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
  /// @name 内容を取得する関数
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

private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 結果の辞書
  // キーは故障番号
  std::unordered_map<SizeType, ResultRep*> mResultDict;

};

END_NAMESPACE_DRUID

#endif // DTPGRESULTS_H
