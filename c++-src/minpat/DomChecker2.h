﻿#ifndef DOMCHECKER2_H
#define DOMCHECKER2_H

/// @file DomChecker2.h
/// @brief DomChecker2 のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "BaseEnc.h"
#include "BoolDiffEnc.h"
#include "BoolDiffEnc2.h"
#include "FFREnc.h"
#include "FaultEnc.h"
#include "DtpgStats.h"

#include "ym/JsonValue.h"
#include "ym/Timer.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DomChecker2 DomChecker2.h "DomChecker2.h"
/// @brief 支配関係の判定を行うクラス
///
/// f1 が検出可能で f2 が検出不可能なパタンが存在しないことを調べるための
/// クラス．
/// 同一の FFR 内の故障の検出条件は共通部分が多いので FFR 単位で指定する．
///
/// ffr1 と ffr2 同じ場合にも正しく動くはずだが，非効率的なので
/// 別途処理したほうがよい(FFRDomChecker2)．
//////////////////////////////////////////////////////////////////////
class DomChecker2
{
public:

  /// @brief コンストラクタ
  DomChecker2(
    const TpgNetwork& network,              ///< [in] 対象のネットワーク
    const TpgFFR* ffr1,	                    ///< [in] 支配故障の FFR
    const vector<const TpgFFR*>& ffr2_list, ///< [in] 被支配故障の FFR のリスト
    const JsonValue& option = JsonValue{}   ///< [in] オプション
  );

  /// @brief デストラクタ
  ~DomChecker2();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief チェックをする．
  /// @return 結果を返す．
  ///
  /// fault1 を検出して ffr2 の根の出力から故障伝搬を行わないケースが
  /// 「ない」ときに true を返す．
  /// 以降は fault1 の検出と ffr2 内のローカルな故障伝搬だけを考慮すれば
  /// よいことになる．
  bool
  check(
    const TpgFault* fault1, ///< [in] 対象の故障
    const TpgFFR* ffr2      ///< [in] 被支配故障の FFR
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 基本のエンコーダ
  BaseEnc mBaseEnc;

  // mFFR1 用の BoolDiffエンコーダ
  BoolDiffEnc* mBdEnc1;

  // mFFR2 用の BoolDiff2エンコーダ
  BoolDiffEnc2* mBdEnc2;

};

END_NAMESPACE_DRUID

#endif // DOMCHECKER2_H
