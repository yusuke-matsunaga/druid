#ifndef DTPGRESULT_H
#define DTPGRESULT_H

/// @file DtpgResult.h
/// @brief DtpgResult のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
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

  /// @brief FaultStatus::Untestable の結果を生成するクラスメソッド
  static
  DtpgResult
  make_untestable();

  /// @brief FaultStatus::Undetected の結果を生成するクラスメソッド
  static
  DtpgResult
  make_undetected();

  /// @brief 空のコンストラクタ
  ///
  /// FaultStatus::Undetected となる．
  DtpgResult();

  /// @brief テストベクタを指定したコンストラクタ
  /// @param[in] testvect テストベクタ
  ///
  /// もちろん結果は FaultStatus::Detected となる．
  explicit
  DtpgResult(const TestVector& testvect);

  /// @brief コピーコンストラクタ
  DtpgResult(const DtpgResult& src) = default;

  /// @brief コピー代入演算子
  DtpgResult&
  operator=(const DtpgResult& src) = default;

  /// @brief デストラクタ
  ~DtpgResult();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 結果を返す．
  FaultStatus
  status() const;

  /// @brief テストベクタを返す．
  const TestVector&
  testvector() const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief FaultStatus::Untestable を設定するコンストラクタ
  /// @param[in] dummy ダミーの引数
  ///
  /// dummy の値は用いられない．
  DtpgResult(int dummy);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 結果
  FaultStatus mStatus;

  // テストベクタ
  TestVector mTestVector;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief FaultStatus::Undetected の結果を生成するクラスメソッド
inline
DtpgResult
DtpgResult::make_undetected()
{
  return DtpgResult();
}

// @brief FaultStatus::Untestable の結果を生成するクラスメソッド
inline
DtpgResult
DtpgResult::make_untestable()
{
  return DtpgResult(0);
}

// @brief 空のコンストラクタ
//
// FaultStatus::Undetected
inline
DtpgResult::DtpgResult() :
  mStatus(FaultStatus::Undetected)
{
}

// @brief SatBool3::False を設定するコンストラクタ
// @param[in] dummy ダミーの引数
//
// dummy の値は用いられない．
inline
DtpgResult::DtpgResult(int dummy) :
  mStatus(FaultStatus::Untestable)
{
}

// @brief テストベクタを指定したコンストラクタ
// @param[in] testvect テストベクタ
inline
DtpgResult::DtpgResult(const TestVector& testvect) :
  mStatus(FaultStatus::Detected),
  mTestVector(testvect)
{
}

// @brief デストラクタ
inline
DtpgResult::~DtpgResult()
{
}

// @brief 結果を返す．
inline
FaultStatus
DtpgResult::status() const
{
  return mStatus;
}

// @brief テストベクタを返す．
inline
const TestVector&
DtpgResult::testvector() const
{
  return mTestVector;
}

END_NAMESPACE_DRUID

#endif // DTPGRESULT_H
