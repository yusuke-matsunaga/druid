#ifndef FAULTANALYZE_H
#define FAULTANALYZE_H

/// @file FaultAnalyze.h
/// @brief FaultAnalyze のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "minpat/FaultInfo.h"
#include "misc/ConfigParam.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class FaultAnalyze FaultAnalyze.h "FaultAnalyze.h"
/// @brief 故障の検出状況の情報を保持するクラス
///
/// 結果は引数の fault_info に格納される．
//////////////////////////////////////////////////////////////////////
class FaultAnalyze
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障の解析を行う．
  static
  FaultInfo
  run(
    const TpgFaultList& fault_list, ///< [in] 故障のリスト
    const ConfigParam& option = {}  ///< [in] オプション
  );

};

END_NAMESPACE_DRUID

#endif // FAULTANALYZE_H
