#ifndef FFRENCDRIVER_H
#define FFRENCDRIVER_H

/// @file MFFCEncDriver.h
/// @brief MFFCEncDriver のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgDriverImpl.h"
#include "StructEngine.h"
#include "BoolDiffEnc.h"
#include "MFFCEnc.h"
#include "Justifier.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class MFFCEncDriver MFFCEncDriver.h "MFFCEncDriver.h"
/// @brief MFFCEnc を用いたドライバ
//////////////////////////////////////////////////////////////////////
class MFFCEncDriver :
  public DtpgDriverImpl
{
public:

  /// @brief コンストラクタ
  MFFCEncDriver(
    const TpgNetwork& network, ///< [in] 対象のネットワーク
    const TpgMFFC* mffc,       ///< [in] 故障伝搬の起点となる MFFC
    const JsonValue& option    ///< [in] オプション
  );

  /// @brief デストラクタ
  ~MFFCEncDriver() = default;


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

  // 基本のエンコーダ
  StructEngine mEngine;

  // BoolDiffEnc
  BoolDiffEnc* mBdEnc;

  // MFFCEnc
  MFFCEnc* mMFFCEnc;

};

END_NAMESPACE_DRUID

#endif // FFRENCDRIVER_H
