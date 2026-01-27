#ifndef FVAL2_H
#define FVAL2_H

/// @file Fval2.h
/// @brief Fval2 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "Val3.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @brief 故障値を表す列挙型
//////////////////////////////////////////////////////////////////////
enum class Fval2 : std::uint8_t {
  zero = 0,
  one  = 1
};

/// @brief 0縮退故障のときに true を返す．
inline
bool
is_sa0(
  Fval2 val
)
{
  return val == Fval2::zero;
}

/// @brief 1縮退故障のときに true を返す．
inline
bool
is_sa1(
  Fval2 val
)
{
  return !is_sa0(val);
}

/// @brief 0 -> 1 遷移故障の時に true を返す．
inline
bool
is_rise_transition(
  Fval2 val
)
{
  return val == Fval2::zero;
}

/// @brief 1 -> 0 遷移故障のときに true を返す．
inline
bool
is_fall_transition(
  Fval2 val
)
{
  return !is_rise_transition(val);
}

/// @brief Val3 との等価比較演算
inline
bool
operator==(
  Fval2 left,
  Val3 right
)
{
  if ( left == Fval2::zero && right == Val3::_0 ) {
    return true;
  }
  if ( left == Fval2::one && right == Val3::_1 ) {
    return true;
  }
  return false;
}

inline
bool
operator!=(
  Fval2 left,
  Val3 right
)
{
  return !operator==(left, right);
}

inline
bool
operator==(
  Val3 left,
  Fval2 right
)
{
  return operator==(right, left);
}

inline
bool
operator!=(
  Val3 left,
  Fval2 right
)
{
  return !operator==(left, right);
}

/// @brief Fval2 のストリーム演算子
inline
std::ostream&
operator<<(
  std::ostream& s,
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
