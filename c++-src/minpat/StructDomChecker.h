#ifndef STRUCTDOMCHECKER_H
#define STRUCTDOMCHECKER_H

/// @file StructDomChecker.h
/// @brief StructDomChecker のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "StructEnc.h"
#include "ym/SatBool3.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class StructDomChecker StructDomChecker.h "StructDomChecker.h"
/// @brief StructEnc を利用した DomChecker
//////////////////////////////////////////////////////////////////////
class StructDomChecker
{
public:

  /// @brief コンストラクタ
  StructDomChecker(
    const TpgNetwork& network, ///< [in] 対象のネットワーク
    const TpgFault* fault1,    ///< [in] 故障1
    const TpgFault* fault2,    ///< [in] 故障2
    const JsonValue& option    ///< [in] オプション
    = JsonValue{}
  );

  /// @brief デストラクタ
  ~StructDomChecker();


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
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // エンジン
  StructEnc mEngine;

  // 故障1
  const TpgFault* mFault1;

  // 故障2
  const TpgFault* mFault2;

};

END_NAMESPACE_DRUID

#endif // STRUCTDOMCHECKER_H
