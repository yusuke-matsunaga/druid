#ifndef EXTRACT_H
#define EXTRACT_H

/// @file extract.h
/// @brief extract のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

/// @brief 十分条件を取り出す．
extern
NodeValList
extract_sufficient_condition(
  const TpgNode* root,    ///< [in] 起点となるノード
  const VidMap& gvar_map, ///< [in] 正常値のマップ
  const VidMap& fvar_map, ///< [in] 故障値のマップ
  const SatModel& model,  ///< [in] SAT問題の解
  const JsonValue& option ///< [in] オプション
  = JsonValue::null()
);

END_NAMESPACE_DRUID

#endif // EXTRACT_H
