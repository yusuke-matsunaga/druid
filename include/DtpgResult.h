#ifndef DTPGRESULT_H
#define DTPGRESULT_H

/// @file DtpgResult.h
/// @brief DtpgResult のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "FaultStatus.h"
#include "TestVector.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DtpgResult DtpgResult.h "DtpgResult.h"
/// @brief DTPG の結果を表すクラス
///
/// 具体的には
/// - パタン生成の結果(検出，冗長，アボート)
/// - テストパタン
/// を持つ．
/// もちろん，テストパタンは検出できた時のみ意味を持つ．
//////////////////////////////////////////////////////////////////////
class DtpgResult
{
public:

  /// @brief 空のコンストラクタ
  ///
  /// status が省略された場合は FaultStatus::Undetected となる．
  /// status は FaultStatus::Detected 以外でなければならない．
  explicit
  DtpgResult(
    FaultStatus status = FaultStatus::Undetected
  ) : mStatus{status}
  {
    ASSERT_COND( status != FaultStatus::Detected );
  }

  /// @brief テストベクタを指定したコンストラクタ
  ///
  /// もちろん結果は FaultStatus::Detected となる．
  explicit
  DtpgResult(
    const TestVector& testvect  ///< [in] testvect テストベクタ
  ) : mStatus{FaultStatus::Detected},
      mTestVector{testvect}
  {
  }

  /// @brief コピーコンストラクタ
  DtpgResult(
    const DtpgResult& src
  ) = default;

  /// @brief コピー代入演算子
  DtpgResult&
  operator=(
    const DtpgResult& src
  ) = default;

  /// @brief デストラクタ
  ~DtpgResult() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 結果を返す．
  FaultStatus
  status() const
  {
    return mStatus;
  }

  /// @brief テストベクタを返す．
  const TestVector&
  testvector() const
  {
    return mTestVector;
  }

private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 結果
  FaultStatus mStatus{FaultStatus::Undetected};

  // テストベクタ
  TestVector mTestVector;

};

END_NAMESPACE_DRUID

#endif // DTPGRESULT_H
