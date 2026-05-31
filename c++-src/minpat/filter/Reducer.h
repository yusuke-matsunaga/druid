#ifndef REDUCER_H
#define REDUCER_H

/// @file Reducer.h
/// @brief Reducer のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "minpat/FaultInfo.h"
#include "misc/ConfigParam.h"


BEGIN_NAMESPACE_DRUID

class EqDomCand;

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

  /// @brief 故障グループの細分化を行ってから支配関係を調べる．
  static
  void
  run(
    FaultInfo& fault_info,    ///< [in] 故障情報を収めたオブジェクト
    const EqDomCand& cand,    ///< [in] 等価故障/支配故障の候補の情報
    const ConfigParam& option ///< [in] オプション
  );

};

END_NAMESPACE_DRUID

#endif // REDUCER_H
