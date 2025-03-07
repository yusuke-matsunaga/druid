#ifndef SIMPLEDOMCHECKER_H
#define SIMPLEDOMCHECKER_H

/// @file SimpleDomChecker.h
/// @brief SimpleDomChecker のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "StructEngine.h"
#include "BoolDiffEnc.h"
#include "DtpgStats.h"

#include "ym/JsonValue.h"
#include "ym/Timer.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class SimpleDomChecker SimpleDomChecker.h "SimpleDomChecker.h"
/// @brief 支配関係の判定を行うクラス
///
/// f1 が検出可能で f2 が検出不可能なパタンが存在しないことを調べるための
/// クラス．
/// f2 の FFR 外の故障伝搬条件は別途調べてあるものとする．
/// ここでは f2 の FFR 内の故障伝搬条件のみを考慮する．
//////////////////////////////////////////////////////////////////////
class SimpleDomChecker
{
public:

  /// @brief コンストラクタ
  SimpleDomChecker(
    const TpgNetwork& network,                  ///< [in] 対象のネットワーク
    const TpgFFR* ffr1,	                        ///< [in] 支配故障の FFR
    const vector<const TpgFault*>& fault2_list, ///< [in] 被支配故障の候補リスト
    const JsonValue& option = JsonValue{}       ///< [in] オプション
  );

  /// @brief デストラクタ
  ~SimpleDomChecker();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief チェックする．
  /// @return 結果を返す．
  ///
  /// fault1 を検出して fault2 を検出しないテストパタンが
  /// 存在「しない」ことを確かめる．
  /// その場合には支配故障であることがわかる．
  /// fault1 は ffr1 に，fault2 は ffr2 に含まれると
  /// 仮定している．
  bool
  check(
    const TpgFault* fault1, ///< [in] 支配故障の候補
    const TpgFault* fault2  ///< [in] 被支配故障の候補リスト
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 基本のエンコーダ
  std::unique_ptr<StructEngine> mEngine;

  // ffr 用の伝搬条件を表すエンコーダ
  BoolDiffEnc* mBdEnc;

};

END_NAMESPACE_DRUID

#endif // SIMPLEDOMCHECKER_H
