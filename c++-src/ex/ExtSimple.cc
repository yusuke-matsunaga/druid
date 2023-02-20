
/// @file ExtSimple.cc
/// @brief ExtSimple の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ExtSimple.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス ExtSimple
//////////////////////////////////////////////////////////////////////

// @brief 対象の出力を選ぶ．
const TpgNode*
ExtSimple::select_output(
  const vector<const TpgNode*>& output_list
)
{
  // 最初の要素を返す．
  return output_list[0];
}

// @brief 制御値を持つ side input を選ぶ．
const TpgNode*
ExtSimple::select_cnode(
  const vector<const TpgNode*>& node_list
)
{
  // 最初の要素を返す．
  return node_list[0];
}

END_NAMESPACE_DRUID
