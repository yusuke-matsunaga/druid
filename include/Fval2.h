#ifndef FVAL2_H
#define FVAL2_H

/// @file Fval2.h
/// @brief Fval2 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @brief 故障値を表す列挙型
//////////////////////////////////////////////////////////////////////
enum class Fval2 : ymuint8 {
  zero = 0,
  one  = 1
};

/// @brief Fval2 のストリーム演算子
inline
ostream&
operator<<(
  ostream& s,
  Fval2 val
)
{
  switch ( val ) {
  case Fval2::zero: s << "0"; break;
  case Fval2::one:  s << "1"; break;
  }
  return s;
}

END_NAMESPACE_DRUID

#endif // FVAL2_H
