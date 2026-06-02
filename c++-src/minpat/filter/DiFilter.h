#ifndef DIFILTER_H
#define DIFILTER_H

/// @file DiFilter.h
/// @brief DiFilter のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "EqDomCand.h"
#include "FaultInfo.h"
#include "misc/ConfigParam.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DiFilter DiFilter.h "DiFilter.h"
/// @brief Dichotomy を用いた支配故障候補のフィルター
//////////////////////////////////////////////////////////////////////
class DiFilter
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 等価故障と支配故障の候補を求める．
  static
  EqDomCand
  run(
    FaultInfo& fault_info,    ///< [in] 対象の故障の情報
    const ConfigParam& option ///< [in] オプション
  );

};

END_NAMESPACE_DRUID

#endif // DIFILTER_H
