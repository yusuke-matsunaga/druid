#ifndef DTPGMGR_H
#define DTPGMGR_H

/// @file DtpgMgr.h
/// @brief DtpgMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023, 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "Fsim.h"
#include "TestVector.h"
#include "TpgFault.h"
#include "DtpgStats.h"
#include "DopVerifyResult.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DtpgMgr DtpgMgr.h "DtpgMgr.h"
/// @brief テストパタン生成を行う本体
///
/// 基本的には与えられた全ての故障を検出するためのテストベクタの生成を行う
/// だけの関数．
/// ただし，ある故障に対するテストベクタが求まった時点でそのテストベクタで
/// 故障シミュレーションを行って検出できる故障を除外するなどの追加の処理
/// を行えるように故障検出時などに呼び出されるコールバック関数を引数にとる．
/// また，検出済みとなった故障の状態を保持するためにクラスの形を取っている．
//////////////////////////////////////////////////////////////////////
class DtpgMgr
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テスト生成を行う．
  ///
  /// 基本的には status_mgr に含まれる全故障を対象とするが，
  /// status_mgr 上で Detected/Untestable とマークされている故障は
  /// スキップする．
  /// status_mgr は const ではないのでテスト生成の途中で故障の状態は
  /// 変化しうる(生成されたテストベクタを用いた故障シミュレーションに
  /// よる故障ドロップなど)
  static
  DtpgStats
  run(
    const TpgNetwork& network,     ///< [in] 対象のネットワーク
    TpgFaultStatusMgr& status_mgr, ///< [inout] 故障の状態を表すオブジェクト
    FaultTvCallback det_func,      ///< [in] 検出時に呼ばれる関数
    FaultCallback untest_func,     ///< [in] 検出不能の判定時に呼ばれる関数
    FaultCallback abort_func,      ///< [in] アボート時に呼ばれる関数
    const JsonValue& option        ///< [in] オプションを表す JSON オブジェクト
  );

};

END_NAMESPACE_DRUID

#endif // DTPGMGR_H
