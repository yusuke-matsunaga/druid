#ifndef MFFCANALYZE_H
#define MFFCANALYZE_H

/// @file MFFCAnalyze.h
/// @brief MFFCAnalyze のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "dtpg/FaultInfo.h"
#include "misc/ConfigParam.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class MFFCAnalyze MFFCAnalyze.h "MFFCAnalyze.h"
/// @brief 同じ MFFC 内の故障の支配関係を調べるクラス
///
/// 実はただの関数
//////////////////////////////////////////////////////////////////////
class MFFCAnalyze
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 処理を行う関数
  static
  void
  run(
    const TpgMFFC& mffc,            ///< [in] MFFC
    const TpgFaultList& fault_list, ///< [in] 故障リスト
    FaultInfo& fault_info,          ///< [in] 情報を格納するオブジェクト
    const ConfigParam& option = {}  ///< [in] オプション
  );

};

END_NAMESPACE_DRUID

#endif // MFFCANALYZE_H
