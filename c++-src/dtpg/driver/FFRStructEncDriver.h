#ifndef FFRSTRUCTENCDRIVER_H
#define FFRSTRUCTENCDRIVER_H

/// @file FFRStructEncDriver.h
/// @brief FFRStructEncDriver のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgDriverImpl.h"
#include "StructEnc.h"
#include "Justifier.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class FFRStructEncDriver FFRStructEncDriver.h "FFRStructEncDriver.h"
/// @brief FFRStructEnc を用いたドライバ
//////////////////////////////////////////////////////////////////////
class FFRStructEncDriver :
  public DtpgDriverImpl
{
public:

  /// @brief コンストラクタ
  FFRStructEncDriver(
    const TpgNetwork& network, ///< [in] 対象のネットワーク
    const TpgFFR* ffr,	       ///< [in] 故障伝搬の起点となる FFR
    const JsonValue& option    ///< [in] オプション
  );

  /// @brief デストラクタ
  ~FFRStructEncDriver();


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
  ) override;

  /// @brief テストパタン生成を行う．
  TestVector
  gen_pattern(
    const TpgFault* fault ///< [in] 対象の故障
  ) override;

  /// @brief CNF の生成時間を返す．
  double
  cnf_time() const override;

  /// @brief SATの統計情報を返す．
  SatStats
  sat_stats() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // エンジン本体
  StructEnc mStructEnc;

  // FFR の根のノード
  const TpgNode* mRoot;

  // justifier
  Justifier mJustifier;

  // CNF の生成時間
  double mCnfTime;

};

END_NAMESPACE_DRUID

#endif // FFRSTRUCTENCDRIVER_H
