#ifndef FFRANALYZE_H
#define FFRANALYZE_H

/// @file FFRAnalyze.h
/// @brief FFRAnalyze のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "minpat/FaultInfo.h"
#include "misc/ConfigParam.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class FFRAnalyze FFRAnalyze.h "FFRAnalyze.h"
/// @brief 同じ FFR 内の故障の情報を調べるクラス
///
/// 実はただの関数
//////////////////////////////////////////////////////////////////////
class FFRAnalyze
{
public:

  /// @brief 処理を行う．
  static
  void
  run(
    const TpgFFR& ffr,              ///< [in] FFR
    const TpgFaultList& fault_list, ///< [in] 故障のリスト
    FaultInfo& fault_info,          ///< [in] 情報を格納するオブジェクト
    const ConfigParam& option = {}  ///< [in] オプション
  );

};

END_NAMESPACE_DRUID

#endif // FFRANALYZE_H
