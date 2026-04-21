#ifndef DICHOTOMY_H
#define DICHOTOMY_H

/// @file Dichotomy.h
/// @brief Dichotomy のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "minpat/FaultInfo.h"
#include "misc/ConfigParam.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class Dichotomy Dichotomy.h "Dichotomy.h"
/// @brief 二分法を用いて故障グループの細分化を行うクラス
//////////////////////////////////////////////////////////////////////
class Dichotomy
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障グループの細分化を行ってから支配関係を調べる．
  static
  void
  run(
    FaultInfo& fault_info,    ///< [in] 故障情報を収めたオブジェクト
    const ConfigParam& option ///< [in] オプション
  );

};

END_NAMESPACE_DRUID

#endif // DICHOTOMY_H
