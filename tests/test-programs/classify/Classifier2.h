#ifndef CLASSIFIER2_H
#define CLASSIFIER2_H

/// @file Classifier2.h
/// @brief Classifier2 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"


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
    bool has_prev_state,                ///< [in] 2時刻分のパタンを持つ時 true
    const vector<TestVector>& tv_list,  ///< [in] テストベクタのリスト
    bool singleton_drop                 ///< [in] singleton drop を行うとき true にするフラグ
  );
};

END_NAMESPACE_DRUID

#endif // CLASSIFIER2_H
