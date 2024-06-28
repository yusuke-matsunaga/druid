#ifndef FFRDOMCHECKER_H
#define FFRDOMCHECKER_H

/// @file FFRDomChecker.h
/// @brief FFRDomChecker のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "BaseEnc.h"
#include "BoolDiffEnc.h"
#include "FFREnc.h"
#include "FaultEnc.h"
#include "DtpgStats.h"

#include "ym/JsonValue.h"
#include "ym/Timer.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class FFRDomChecker FFRDomChecker.h "FFRDomChecker.h"
/// @brief 同一FFR内の支配関係の判定を行うクラス
///
/// f1 が検出可能で f2 が検出不可能なパタンが存在しないことを調べるための
/// クラス．
//////////////////////////////////////////////////////////////////////
class FFRDomChecker
{
public:

  /// @brief コンストラクタ
  FFRDomChecker(
    const TpgNetwork& network,                  ///< [in] 対象のネットワーク
    const TpgFFR* ffr,	                        ///< [in] 対象の FFR
    const vector<const TpgFault*>& fault2_list, ///< [in] 被支配故障の候補リスト
    const JsonValue& option = JsonValue{}       ///< [in] オプション
  );

  /// @brief デストラクタ
  ~FFRDomChecker();


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
  /// fault1, fault2 は ffr に含まれると仮定している．
  bool
  check(
    const TpgFault* fault1, ///< [in] 支配故障の候補
    const TpgFault* fault2  ///< [in] 被支配故障の候補
  );

  /// @brief 統計情報を得る．
  const DtpgStats&
  stats() const
  {
    return mStats;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象の FFR
  const TpgFFR* mFFR;

  // 被支配故障の候補のリスト
  const vector<const TpgFault*>& mFault2List;

  // 基本のエンコーダ
  BaseEnc mBaseEnc;

  // mFFR 用の BoolDiffエンコーダ
  BoolDiffEnc* mBdEnc;

  // 被支配故障用の FFRエンコーダ
  FFREnc* mFFREnc;

  // 時間計測を行なうかどうかの制御フラグ
  bool mTimerEnable;

  // 時間計測用のタイマー
  Timer mTimer;

  // 統計情報
  DtpgStats mStats;

};

END_NAMESPACE_DRUID

#endif // FFRDOMCHECKER_H
