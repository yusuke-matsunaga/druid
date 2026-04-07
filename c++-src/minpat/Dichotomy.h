#ifndef DICHOTOMY_H
#define DICHOTOMY_H

/// @file Dichotomy.h
/// @brief Dichotomy のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "DiGroupMgr.h"
#include "misc/ConfigParam.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class Dichotomy Dichotomy.h "Dichotomy.h"
/// @brief 二分法を用いて故障グループの細分化を行うクラス
///
/// 機能的には内部に状態をもたないが，分割結果を表す DiGroup
/// のリソース管理のためのメンバを持つ．
/// 逆に言うとこのクラスから返される DiGroup* の寿命は
/// このクラスの寿命と等しい．
//////////////////////////////////////////////////////////////////////
class Dichotomy
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障グループの細分化を行う．
  static
  DiGroupMgr
  run(
    const TpgFaultList& fault_list, ///< [in] 元となる故障グループ
    const ConfigParam& option       ///< [in] オプション
  );

};

END_NAMESPACE_DRUID

#endif // DICHOTOMY_H
