#ifndef GATETYPE_H
#define GATETYPE_H

/// @file GateType.h
/// @brief GateType の定義ファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @brief 論理ゲートの種類を表す列挙型
//////////////////////////////////////////////////////////////////////
enum class GateType : ymuint8 {
  Const0 = 0, ///< 定数0
  Const1 = 1, ///< 定数1
  Input  = 2, ///< 入力
  Buff   = 3, ///< バッファ
  Not    = 4, ///< Not
  And    = 5, ///< And
  Nand   = 6, ///< Nand
  Or     = 7, ///< Or
  Nor    = 8, ///< Nor
  Xor    = 9, ///< Xor
  Xnor   = 10, ///< Xnor
};

/// @brief GateType のストリーム演算子
ostream&
operator<<(
  ostream& s,
  GateType gate_type
);

END_NAMESPACE_DRUID

#endif // GATETYPE_H
