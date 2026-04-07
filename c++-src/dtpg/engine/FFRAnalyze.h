#ifndef FFRANALYZE_H
#define FFRANALYZE_H

/// @file FFRAnalyze.h
/// @brief FFRAnalyze のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "dtpg/FaultInfo.h"
#include "misc/ConfigParam.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class FFRAnalyze FFRAnalyze.h "FFRAnalyze.h"
/// @brief 同じ FFR 内の故障の情報を調べるクラス
///
/// 結果は引数の fault_info に格納する．
/// このクラスはクラスメソッドのみ
//////////////////////////////////////////////////////////////////////
class FFRAnalyze
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief FFR内の故障の支配関係を調べる．
  ///
  /// fault_list 内の故障は ffr に属するものと仮定する．
  static
  void
  run(
    const TpgFFR& ffr,              ///< [in] FFR
    const TpgFaultList& fault_list, ///< [in] 故障のリスト
    FaultInfo& fault_info,          ///< [in] 結果を格納する配列
    const ConfigParam& option = {}  ///< [in] オプション
  );

  /// @brief 代表故障に対して必須条件を求める．
  static
  void
  get_mandatory_condition(
    const TpgFFR& ffr,              ///< [in] FFR
    const TpgFaultList& fault_list, ///< [in] 故障のリスト
    FaultInfo& fault_info,          ///< [in] 結果を格納する配列
    const ConfigParam& option = {}  ///< [in] オプション
  );

};

END_NAMESPACE_DRUID

#endif // FFRANALYZE_H
