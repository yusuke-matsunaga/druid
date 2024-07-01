#ifndef DOMCHECKER_H
#define DOMCHECKER_H

/// @file DomChecker.h
/// @brief DomChecker のヘッダファイル
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
/// @class DomChecker DomChecker.h "DomChecker.h"
/// @brief 支配関係の判定を行うクラス
///
/// f1 が検出可能で f2 が検出不可能なパタンが存在しないことを調べるための
/// クラス．
/// 同一の FFR 内の故障の検出条件は共通部分が多いので FFR 単位で指定する．
///
/// ffr1 と ffr2 同じ場合にも正しく動くはずだが，非効率的なので
/// 別途処理したほうがよい(FFRDomChecker)．
//////////////////////////////////////////////////////////////////////
class DomChecker
{
public:

  /// @brief コンストラクタ
  DomChecker(
    const TpgNetwork& network,            ///< [in] 対象のネットワーク
    const TpgFFR* ffr1,	                  ///< [in] 支配故障の FFR
    const TpgFFR* ffr2,	                  ///< [in] 被支配故障の FFR
    const JsonValue& option = JsonValue{} ///< [in] オプション
  );

  /// @brief デストラクタ
  ~DomChecker();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief FFRのみの故障伝搬条件でチェックする．
  bool
  check0();

  /// @brief 事前チェックをする．
  /// @return 結果を返す．
  ///
  /// fault1 を検出して ffr2 の根の出力から故障伝搬を行わないケースが
  /// 「ない」ときに true を返す．
  /// 以降は fault1 の検出と ffr2 内のローカルな故障伝搬だけを考慮すれば
  /// よいことになる．
  bool
  precheck(
    const TpgFault* fault1 ///< [in] 対象の故障
  );

  /// @brief チェックする．
  /// @return 結果を返す．
  ///
  /// fault1 を検出して fault2 を検出しないテストパタンが
  /// 存在「しない」ことを確かめる．
  /// その場合には支配故障であることがわかる．
  /// fault1 は ffr1 に，fault2 は ffr2 に含まれると
  /// 仮定している．
  SizeType
  check(
    const TpgFault* fault1,                     ///< [in] 支配故障の候補
    const vector<const TpgFault*>& fault2_list, ///< [in] 被支配故障の候補リスト
    vector<bool>& del_mark                      ///< [out] 削除マーク
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 支配故障を含む FFR
  const TpgFFR* mFFR1;

  // 被支配故障を含む FFR
  const TpgFFR* mFFR2;

  // 基本のエンコーダ
  BaseEnc mBaseEnc;

  // mFFR1 用の BoolDiffエンコーダ
  BoolDiffEnc* mBdEnc1;

  // mFFR2 用の BoolDiffエンコーダ
  BoolDiffEnc* mBdEnc2;

};

END_NAMESPACE_DRUID

#endif // DOMCHECKER_H
