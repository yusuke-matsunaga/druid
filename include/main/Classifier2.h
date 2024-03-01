#ifndef CLASSIFIER2_H
#define CLASSIFIER2_H

/// @file Classifier2.h
/// @brief Classifier2 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "Fsim.h"


BEGIN_NAMESPACE_DRUID


//////////////////////////////////////////////////////////////////////
/// @class Classifier2 Classifier2.h "Classifier2.h"
/// @brief
//////////////////////////////////////////////////////////////////////
class Classifier2
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 与えられたテストベクタで故障を分類する．
  static
  vector<vector<TpgFault>>
  run(
    const TpgNetwork& network,          ///< [in] 対象のネットワーク
    const vector<TpgFault>& fault_list, ///< [in] 故障リスト
    FaultType fault_type,               ///< [in] 故障の種類
    const vector<TestVector>& tv_list,  ///< [in] テストベクタのリスト
    bool ppsfp,                         ///< [in] PPSFP を使う時 true
    bool multi
  );
};

END_NAMESPACE_DRUID

#endif // CLASSIFIER2_H
