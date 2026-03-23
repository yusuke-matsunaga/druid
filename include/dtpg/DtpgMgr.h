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
#include "types/AssignList.h"
#include "dtpg/DtpgResults.h"
#include "misc/ConfigParam.h"


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
    const ConfigParam& option = {}  ///< [in] オプションを表す JSON オブジェクト
  );

  /// @brief 検出可能かチェックする．
  static
  bool
  check(
    const TpgFault& fault,        ///< [in] 対象の故障
    const AssignList& assign_list ///< [in] 値割り当て
  )
  {
    auto res_array = check(fault, std::vector<AssignList>{assign_list});
    return res_array[0];
  }

  /// @brief 検出可能かチェックする．
  static
  std::vector<bool>
  check(
    const TpgFault& fault,                           ///< [in] 対象の故障
    const std::vector<AssignList>& assign_list_array ///< [in] 値割り当ての配列
  );

};

END_NAMESPACE_DRUID

#endif // DTPGMGR_H
