#ifndef MPREDUCER_H
#define MPREDUCER_H

/// @file MpReducer.h
/// @brief MpReducer のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class MpReducer MpReducer.h "MpReducer.h"
/// @brief 故障の削減を行うクラス
///
/// 実際にはクラスメソッドしか持たない．
//////////////////////////////////////////////////////////////////////
class MpReducer
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
    const JsonValue& option = {}    ///< [in] オプション
  );

};

END_NAMESPACE_DRUID

#endif // MPREDUCER_H
