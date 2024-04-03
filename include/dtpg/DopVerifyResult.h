#ifndef DOPVERIFYRESULT_H
#define DOPVERIFYRESULT_H

/// @file DopVerifyResult.h
/// @brief DopVerifyResult のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "TpgFault.h"
#include "TestVector.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DopVerifyResult DopVerifyResult.h "DopVerifyResult.h"
/// @brief DopVerify の結果を格納するクラス
//////////////////////////////////////////////////////////////////////
class DopVerifyResult
{
public:

  /// @brief コンストラクタ
  DopVerifyResult();

  /// @brief デストラクタ
  ~DopVerifyResult();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 成功結果を追加する．
  void
  add_good(
    const TpgFault* f ///< [in] 故障
  );

  /// @brief エラー結果を追加する．
  void
  add_error(
    const TpgFault* f,   ///< [in] 故障
    const TestVector& tv ///< [in] テストベクタ
  );

  /// @brief 成功回数を得る．
  SizeType
  good_count() const;

  /// @brief 成功した故障を得る．
  const TpgFault*
  good_fault(
    SizeType pos  ///< [in] 位置版号 ( 0 <= pos < good_count() )
  ) const;

  /// @brief エラー回数を得る．
  SizeType
  error_count() const;

  /// @brief エラーの故障を得る．
  const TpgFault*
  error_fault(
    SizeType pos  ///< [in] 位置版号 ( 0 <= pos < error_count() )
  ) const;

  /// @brief エラーとなったテストベクタを得る．
  TestVector
  error_testvector(
    SizeType pos  ///< [in] 位置版号 ( 0 <= pos < error_count() )
  ) const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられるデータ構造
  //////////////////////////////////////////////////////////////////////

  struct ErrorCase
  {
    // コンストラクタ
    ErrorCase(
      const TpgFault* f,
      const TestVector& tv
    );

    // 故障
    const TpgFault* mFault;

    // テストベクタ
    TestVector mTestVector;

  };


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 成功した故障のリスト
  vector<const TpgFault*> mGoodList;

  // エラーのリスト
  vector<ErrorCase> mErrorList;

};

END_NAMESPACE_DRUID

#endif // DOPVERIFYRESULT_H
