#ifndef TRIVIALCHECKER1_H
#define TRIVIALCHECKER1_H

/// @file TrivialChecker1.h
/// @brief TrivialChecker1 のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "StructEngine.h"
#include "BoolDiffEnc.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TrivialChecker1 TrivialChecker1.h "TrivialChecker1.h"
/// @brief 支配関係の判定を行うクラス
///
/// 条件が trivial な故障間のチェック用
//////////////////////////////////////////////////////////////////////
class TrivialChecker1
{
public:

  /// @brief コンストラクタ
  TrivialChecker1(
    const TpgNetwork& network,                 ///< [in] 対象のネットワーク
    const vector<const TpgFault*>& fault_list, ///< [in] 支配故障の候補リスト
    const JsonValue& option = JsonValue{}       ///< [in] オプション
  );

  /// @brief デストラクタ
  ~TrivialChecker1();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief チェックする．
  /// @return 結果を返す．
  ///
  /// 支配故障を検出して被支配故障を検出しないテストパタンが
  /// 存在「しない」ことを確かめる．
  /// その場合には支配故障であることがわかる．
  bool
  check(
    const AssignList& cond1, ///< [in] 支配故障の検出条件
    const AssignList& cond2  ///< [in] 被支配故障の検出条件
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 基本のエンコーダ
  StructEngine mEngine;

};

END_NAMESPACE_DRUID

#endif // TRIVIALCHECKER1_H
