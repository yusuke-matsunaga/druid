#ifndef NAIVEDOMCHECKER_H
#define NAIVEDOMCHECKER_H

/// @file NaiveDomChecker.h
/// @brief NaiveDomChecker のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "StructEngine.h"
#include "BoolDiffEnc.h"
#include "FaultEnc.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class NaiveDomChecker NaiveDomChecker.h "NaiveDomChecker.h"
/// @brief ナイーブな DomChecker
//////////////////////////////////////////////////////////////////////
class NaiveDomChecker
{
public:

  /// @brief コンストラクタ
  NaiveDomChecker(
    const TpgNetwork& network,     ///< [in] 対象のネットワーク
    const TpgFault* fault1,        ///< [in] 故障1
    const TpgFault* fault2,        ///< [in] 故障2
    const JsonValue& option        ///< [in] オプション
    = JsonValue{}
  );

  /// @brief デストラクタ
  ~NaiveDomChecker();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief チェックする．
  /// @return 結果を返す．
  ///
  /// fault1 を検出して fault2 を検出しないテストパタンがあるか調べる．
  /// UNSAT の場合には fault1 が fault2 を支配している．
  bool
  check();


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 故障1
  const TpgFault* mFault1;

  // 故障2
  const TpgFault* mFault2;

  // base engine
  StructEngine mEngine;

  // 故障1用の BoolDiff
  BoolDiffEnc* mBdEnc1;

  // 故障2用の BoolDIff
  BoolDiffEnc* mBdEnc2;

  // 故障1用の FaultEnc
  FaultEnc* mFaultEnc1;

  // 故障2用の FaultEnc
  FaultEnc* mFaultEnc2;

};

END_NAMESPACE_DRUID

#endif // NAIVEDOMCHECKER_H
