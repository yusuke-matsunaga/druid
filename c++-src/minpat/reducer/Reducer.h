#ifndef REDUCER_H
#define REDUCER_H

/// @file Reducer.h
/// @brief Reducer のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "FaultInfo.h"
#include "misc/ConfigParam.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class Reducer Reducer.h "Reducer.h"
/// @brief 故障の支配関係を用いて対象の故障を削減するクラス
///
/// 結果は引数の fault_info に反映される．
//////////////////////////////////////////////////////////////////////
class Reducer
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 支配関係を用いて対象の故障を削減する．
  static
  void
  run(
    FaultInfo& fault_info,    ///< [in] 故障情報を収めたオブジェクト
    const ConfigParam& option ///< [in] オプション
  );

};

END_NAMESPACE_DRUID

#endif // REDUCER_H
