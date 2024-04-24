#ifndef MFFCENGINEDRIVER_H
#define MFFCENGINEDRIVER_H

/// @file MFFCEngineDriver.h
/// @brief MFFCEngineDriver のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgDriverImpl.h"
#include "MFFCEngine.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class MFFCEngineDriver MFFCEngineDriver.h "MFFCEngineDriver.h"
/// @brief MFFCEngine を用いたドライバ
//////////////////////////////////////////////////////////////////////
class MFFCEngineDriver :
  public DtpgDriverImpl
{
public:

  /// @brief コンストラクタ
  MFFCEngineDriver(
    const TpgNetwork& network, ///< [in] 対象のネットワーク
    const TpgMFFC* mffc,       ///< [in] 故障伝搬の起点となる MFFC
    const JsonValue& option    ///< [in] オプション
  );

  /// @brief デストラクタ
  ~MFFCEngineDriver();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障を検出する条件を求める．
  ///
  /// - f はコンストラクタで指定した MFFC 内の故障でなければならない．
  SatBool3
  solve(
    const TpgFault* fault ///< [in] 対象の故障
  ) override;

  /// @brief テストパタン生成を行う．
  TestVector
  gen_pattern(
    const TpgFault* fault ///< [in] 対象の故障
  ) override;

  /// @brief SATの統計情報を返す．
  SatStats
  sat_stats() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // エンジン本体
  MFFCEngine mEngine;

};

END_NAMESPACE_DRUID

#endif // MFFCENGINEDRIVER_H
