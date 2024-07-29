#ifndef TRIVIALCHECKER3_H
#define TRIVIALCHECKER3_H

/// @file TrivialChecker3.h
/// @brief TrivialChecker3 のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "BaseEnc.h"
#include "BoolDiffEnc.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TrivialChecker3 TrivialChecker3.h "TrivialChecker3.h"
/// @brief 支配関係の判定を行うクラス
///
/// 支配故障の条件が trivial なケース用
/// 被支配故障のみ BoolDiffEnc を用意する．
//////////////////////////////////////////////////////////////////////
class TrivialChecker3
{
public:

  /// @brief コンストラクタ
  TrivialChecker3(
    const TpgNetwork& network,                  ///< [in] 対象のネットワーク
    const vector<const TpgFault*>& fault1_list, ///< [in] 支配故障の候補リスト
    const TpgFFR* ffr2,	                        ///< [in] 被支配故障の FFR
    const JsonValue& option = JsonValue{}       ///< [in] オプション
  );

  /// @brief デストラクタ
  ~TrivialChecker3();


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
    const AssignList& assignments ///< [in] 支配故障の検出条件
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 基本のエンコーダ
  BaseEnc mBaseEnc;

  // mFFR2 用の BoolDiffエンコーダ
  BoolDiffEnc* mBdEnc2;

};

END_NAMESPACE_DRUID

#endif // TRIVIALCHECKER3_H
