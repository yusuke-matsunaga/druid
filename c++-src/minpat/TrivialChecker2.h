#ifndef TRIVIALCHECKER2_H
#define TRIVIALCHECKER2_H

/// @file TrivialChecker2.h
/// @brief TrivialChecker2 のヘッダファイル
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
/// @class TrivialChecker2 TrivialChecker2.h "TrivialChecker2.h"
/// @brief 支配関係の判定を行うクラス
///
/// 被支配故障の条件が trivial な場合のチェックを行う．
//////////////////////////////////////////////////////////////////////
class TrivialChecker2
{
public:

  /// @brief コンストラクタ
  TrivialChecker2(
    const TpgNetwork& network,                  ///< [in] 対象のネットワーク
    const TpgFFR* ffr1,                         ///< [in] 支配故障の FFR
    const vector<const TpgFault*>& fault2_list, ///< [in] 被支配故障の候補リスト
    const JsonValue& option = JsonValue{}       ///< [in] オプション
  );

  /// @brief デストラクタ
  ~TrivialChecker2();


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
    const TpgFault* fault1,      ///< [in] 支配故障
    const TpgFault* fault2,      ///< [in] 被支配故障の候補
    const NodeTimeValList& cond2 ///< [in] 被支配故障の検出条件
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 基本のエンコーダ
  BaseEnc mBaseEnc;

  // mFFR1 用の BoolDiffエンコーダ
  BoolDiffEnc* mBdEnc1;

  // 故障番号をキーとして制御変数を格納する辞書
  unordered_map<SizeType, SatLiteral> mVarMap;

};

END_NAMESPACE_DRUID

#endif // TRIVIALCHECKER2_H
