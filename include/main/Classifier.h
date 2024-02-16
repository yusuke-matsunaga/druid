#ifndef CLASSIFIER_H
#define CLASSIFIER_H

/// @file Classifier.h
/// @brief Classifier のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "Fsim.h"


BEGIN_NAMESPACE_DRUID


//////////////////////////////////////////////////////////////////////
/// @class Classifier Classifier.h "Classifier.h"
/// @brief
//////////////////////////////////////////////////////////////////////
class Classifier
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
    bool drop_singleton,                ///< [in] singleton をドロップする時 true
    bool multi
  );
};

END_NAMESPACE_DRUID

#endif // CLASSIFIER_H
