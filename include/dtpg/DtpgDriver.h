#ifndef DTPGDRIVER_H
#define DTPGDRIVER_H

/// @file DtpgDriver.h
/// @brief DtpgDriver のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ym/SatBool3.h"


BEGIN_NAMESPACE_DRUID

class DtpgDriverImpl;

//////////////////////////////////////////////////////////////////////
/// @class DtpgDriver DtpgDriver.h "DtpgDriver.h"
/// @brief テスト生成を行う基底クラス
//////////////////////////////////////////////////////////////////////
class DtpgDriver
{
public:

  /// @brief コンストラクタ
  DtpgDriver(
    DtpgDriverImpl* impl ///< [in] 実装クラス
  );

  /// @brief デストラクタ
  ~DtpgDriver();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障を検出する条件を求める．
  ///
  /// - f はコンストラクタで指定した FFR 内の故障でなければならない．
  SatBool3
  solve(
    const TpgFault* fault ///< [in] 対象の故障
  );

  /// @brief 故障のテストパタンを求める．
  ///
  /// - f はコンストラクタで指定した FFR 内の故障でなければならない．
  /// - 直前に solve(f) を呼んで SatBool3::True が返された場合のみ有効
  TestVector
  gen_pattern(
    const TpgFault* fault ///< [in] 対象の故障
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 実装クラスのポインタ
  std::unique_ptr<DtpgDriverImpl> mImpl;

};

END_NAMESPACE_DRUID

#endif // DTPGDRIVER_H
