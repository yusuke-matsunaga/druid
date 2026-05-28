#ifndef NAIVEREDUCE_H
#define NAIVEREDUCE_H

/// @file NaiveReduce.h
/// @brief NaiveReduce のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "minpat/FaultInfo.h"
#include "misc/ConfigParam.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class NaiveReduce NaiveReduce.h "NaiveReduce.h"
/// @brief 単純な方法で支配故障を求めるクラス
//////////////////////////////////////////////////////////////////////
class NaiveReduce
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

#endif // NAIVEREDUCE_H
