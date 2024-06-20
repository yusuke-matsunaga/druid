#ifndef NODEENGINEDRIVER_H
#define NODEENGINEDRIVER_H

/// @file NodeEngineDriver.h
/// @brief NodeEngineDriver のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgDriverImpl.h"
#include "DtpgEngine_Node.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class NodeEngineDriver NodeEngineDriver.h "NodeEngineDriver.h"
/// @brief DtpgEngine_Node を用いたドライバ
//////////////////////////////////////////////////////////////////////
class NodeEngineDriver :
  public DtpgDriverImpl
{
public:

  /// @brief コンストラクタ
  NodeEngineDriver(
    const TpgNetwork& network, ///< [in] 対象のネットワーク
    const TpgNode* node,       ///< [in] 故障伝搬の起点となるノード
    const JsonValue& option    ///< [in] オプション
  );

  /// @brief デストラクタ
  ~NodeEngineDriver();


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
  DtpgEngine_Node mEngine;

};

END_NAMESPACE_DRUID

#endif // NODEENGINEDRIVER_H
