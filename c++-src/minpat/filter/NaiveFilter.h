#ifndef NAIVEFILTER_H
#define NAIVEFILTER_H

/// @file NaiveFilter.h
/// @brief NaiveFilter のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "EqDomCand.h"
#include "minpat/FaultInfo.h"
#include "misc/ConfigParam.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class NaiveFilter NaiveFilter.h "NaiveFilter.h"
/// @brief 単純な支配故障候補のフィルター
//////////////////////////////////////////////////////////////////////
class NaiveFilter
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 等価故障と支配故障の候補を求める．
  static
  EqDomCand
  run(
    const FaultInfo& fault_info, ///< [in] 対象の故障の情報
    const ConfigParam& option    ///< [in] オプション
  );

};

END_NAMESPACE_DRUID

#endif // NAIVEFILTER_H
