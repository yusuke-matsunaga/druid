#ifndef NODEENCDRIVER_H
#define NODEENCDRIVER_H

/// @file NodeEncDriver.h
/// @brief NodeEncDriver のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgDriverImpl.h"
#include "StructEngine.h"
#include "BoolDiffEnc.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class NodeEncDriver NodeEncDriver.h "NodeEncDriver.h"
/// @brief BoolDiffEnc を用いたドライバ
//////////////////////////////////////////////////////////////////////
class NodeEncDriver :
  public DtpgDriverImpl
{
public:

  /// @brief コンストラクタ
  NodeEncDriver(
    const TpgNetwork& network, ///< [in] 対象のネットワーク
    const TpgNode* node,       ///< [in] 故障伝搬の起点となるノード
    const JsonValue& option    ///< [in] オプション
  );

  /// @brief デストラクタ
  ~NodeEncDriver();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障を検出する条件を求める．
  ///
  /// - fault はコンストラクタで指定した FFR 内の故障でなければならない．
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
  std::unique_ptr<StructEngine> mEngine;

  // BoolDiff
  BoolDiffEnc* mBdEnc;

};

END_NAMESPACE_DRUID

#endif // NODEENCDRIVER_H
