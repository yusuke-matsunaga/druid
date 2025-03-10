#ifndef CONDGENCHECKER_H
#define CONDGENCHECKER_H

/// @file CondGenChecker.h
/// @brief CondGenChecker のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "StructEngine.h"
#include "BoolDiffEnc.h"
#include "DetCond.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class CondGenChecker CondGenChecker.h "CondGenChecker.h"
/// @brief CondGen の結果の検証を行うクラス
//////////////////////////////////////////////////////////////////////
class CondGenChecker
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @breif 結果の検証を行う．
  static
  bool
  check(
    const TpgNetwork& network,            ///< [in] 対象のネットワーク
    const TpgFFR* ffr,                    ///< [in] 対象の FFR
    const DetCond& cond,                  ///< [in] CondGen::root_cond() の結果
    const JsonValue& option = JsonValue{} ///< [in] オプション
  );

};

END_NAMESPACE_DRUID

#endif // CONDGENCHECKER_H
