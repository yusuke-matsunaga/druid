#ifndef MINPAT_H
#define MINPAT_H

/// @file MinPat.h
/// @brief MinPat のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "types/TpgNetwork.h"
#include "types/TpgFaultList.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class MinPat MinPat.h "MinPat.h"
/// @brief 最小パタン集合を作るクラス
///
/// このクラスは実体を持たない．
//////////////////////////////////////////////////////////////////////
class MinPat
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief パタン圧縮を行う．
  static
  std::vector<TestVector>
  run(
    const TpgNetwork& network,      ///< [in] 対象のネットワーク
    const TpgFaultList& fault_list, ///< [in] 対象の故障のリスト
    const JsonValue& option = {}    ///< [in] オプション
  );

};

END_NAMESPACE_DRUID

#endif // MINPAT_H
