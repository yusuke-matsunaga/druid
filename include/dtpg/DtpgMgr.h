#ifndef DTPGMGR_H
#define DTPGMGR_H

/// @file DtpgMgr.h
/// @brief DtpgMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "types/TpgFaultList.h"
#include "dtpg/DtpgResults.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DtpgMgr DtpgMgr.h "DtpgMgr.h"
/// @brief テストパタン生成を行う本体
///
/// 基本的には与えられた全ての故障を検出するためのテストベクタの生成を行う
/// だけの関数．
//////////////////////////////////////////////////////////////////////
class DtpgMgr
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テスト生成を行う．
  ///
  /// option は以下の通り
  /// - "group_mode"(string): テスト生成を行う単位を指定する．
  ///   * node: ノード単位
  ///   * ffr: FFR単位
  ///   * mffc: MFFC単位
  /// - "multi_thread"(bool): マルチスレッド実行を行う時 true にする．
  static
  DtpgResults
  run(
    const TpgFaultList& fault_list, ///< [in] 対象の故障のリスト
    const JsonValue& option         ///< [in] オプションを表す JSON オブジェクト
    = JsonValue{}
  );

};

END_NAMESPACE_DRUID

#endif // DTPGMGR_H
