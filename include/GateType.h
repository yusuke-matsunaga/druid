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
enum class GateType {
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


//////////////////////////////////////////////////////////////////////
// cython とのインターフェイス用の暫定的な関数
//////////////////////////////////////////////////////////////////////

/// @brief GateType の値を int に変換する．
inline
int
__gate_type_to_int(GateType gate_type)
{
  switch ( gate_type ) {
  case GateType::Const0: return 0;
  case GateType::Const1: return 1;
  case GateType::Input:  return 2;
  case GateType::Buff:   return 3;
  case GateType::Not:    return 4;
  case GateType::And:    return 5;
  case GateType::Nand:   return 6;
  case GateType::Or:     return 7;
  case GateType::Nor:    return 8;
  case GateType::Xor:    return 9;
  case GateType::Xnor:   return 10;
  }
  ASSERT_NOT_REACHED;
  return 0;
}

/// @brief int の値を GateType に変換する．
inline
GateType
__int_to_gate_type(int val)
{
  switch ( val ) {
  case  0: return GateType::Const0;
  case  1: return GateType::Const1;
  case  2: return GateType::Input;
  case  3: return GateType::Buff;
  case  4: return GateType::Not;
  case  5: return GateType::And;
  case  6: return GateType::Nand;
  case  7: return GateType::Or;
  case  8: return GateType::Nor;
  case  9: return GateType::Xor;
  case 10: return GateType::Xnor;
  }
  ASSERT_NOT_REACHED;
  return GateType::Const0;
}

END_NAMESPACE_DRUID

#endif // GATETYPE_H
