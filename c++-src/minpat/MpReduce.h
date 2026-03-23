#ifndef MPREDUCE_H
#define MPREDUCE_H

/// @file MpReduce.h
/// @brief MpReduce のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "misc/ConfigParam.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class MpReduce MpReduce.h "MpReduce.h"
/// @brief 故障の削減を行うクラス
///
/// 実際にはクラスメソッドしか持たない．
//////////////////////////////////////////////////////////////////////
class MpReduce
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障の削減を行う．
  static
  TpgFaultList
  run(
    const TpgFaultList& fault_list, ///< [in] 故障のリスト
    const ConfigParam& option = {}  ///< [in] オプション
  );

};

END_NAMESPACE_DRUID

#endif // MPREDUCE_H
