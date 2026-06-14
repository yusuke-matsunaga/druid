#ifndef FILTER_H
#define FILTER_H

/// @file Filter.h
/// @brief Filter のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "FaultInfo.h"
#include "misc/ConfigParam.h"


BEGIN_NAMESPACE_DRUID

class EqDomCand;

//////////////////////////////////////////////////////////////////////
/// @class Filter Filter.h "Filter.h"
/// @brief 等価故障と支配故障候補のフィルター
///
/// 実はただの関数
//////////////////////////////////////////////////////////////////////
class Filter
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 等価故障と支配故障の候補を求める．
  static
  std::unique_ptr<EqDomCand>
  run(
    const FaultInfo& fault_info, ///< [in] 対象の故障の情報
    const ConfigParam& option    ///< [in] オプション
  );

  /// @brief 等価故障と支配故障の候補を求める．
  ///
  /// デバッグ用
  /// 並行して NaiveCandMgr を実行する．
  static
  std::unique_ptr<EqDomCand>
  run2(
    const FaultInfo& fault_info, ///< [in] 対象の故障の情報
    const ConfigParam& option    ///< [in] オプション
  );

};

END_NAMESPACE_DRUID

#endif // FILTER_H
