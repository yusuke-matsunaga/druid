#ifndef FAULTANALYZER_H
#define FAULTANALYZER_H

/// @file FaultAnalyzer.h
/// @brief FaultAnalyzer のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "BaseEnc.h"
#include "BoolDiffEnc.h"
#include "NodeTimeValList.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

class FaultInfo;

//////////////////////////////////////////////////////////////////////
/// @class FaultAnalyzer FaultAnalyzer.h "FaultAnalyzer.h"
/// @brief 故障の検出条件を解析するためのクラス
///
/// FFR 単位で処理を行う．
//////////////////////////////////////////////////////////////////////
class FaultAnalyzer
{
public:

  /// @brief コンストラクタ
  FaultAnalyzer(
    const TpgNetwork& network, ///< [in] 対象のネットワーク
    const TpgFFR* ffr,         ///< [in] 対象の FFR
    const JsonValue& option    ///< [in] オプション
    = JsonValue{}
  );

  /// @brief デストラクタ
  ~FaultAnalyzer() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障検出の十分条件と必要条件を求める．
  /// @return 検出可能の時 true を返す．
  bool
  run(
    FaultInfo& finfo ///< [inout] 対象の故障情報を格納するオブジェクト
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 基本エンコーダ
  BaseEnc mBaseEnc;

  // FFR の出力のブール微分
  BoolDiffEnc* mBdEnc;

  // FFR の出力の故障伝搬の必要条件
  NodeTimeValList mRootMandCond;

  // FFR の出力の故障伝搬可能性
  SatBool3 mRootStatus;

};

END_NAMESPACE_DRUID

#endif // FAULTANALYZER_H
