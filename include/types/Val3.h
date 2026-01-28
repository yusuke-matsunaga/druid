#ifndef VAL3_H
#define VAL3_H

/// @file Val3.h
/// @brief 3値を表す型の定義ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ym/SatBool3.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @brief 3値を表す列挙型
/// @ingroup TypesGroup
//////////////////////////////////////////////////////////////////////
enum class Val3 : std::uint8_t {
  _X = 0, ///< 未定
  _0 = 1, ///< 0
  _1 = 2  ///< 1
};

/// @relates Val3
/// @brief 否定演算子
///
/// Val3::_X の場合には変化しない
inline
Val3
operator~(
  Val3 val ///< [in] 値
)
{
  // val | ~val
  // ----+-----
  //   0 |    0
  //   1 |    2
  //   2 |    1
  // でビットパタンとしては 01 10 00 = 0x18 となる．

  std::uint8_t d = static_cast<std::uint8_t>(val) * 2;
  return static_cast<Val3>((0x18 >> d) & 0x3);
}

/// @relates Val3
/// @brief AND 演算子
inline
Val3
operator&(
  Val3 val1, ///< [in] 値1
  Val3 val2  ///< [in] 値2
)
{
  //      |        val1
  // val2 |   0      1      2
  // -----+------------------
  //    0 |   0      1      0
  //    1 |   1      1      1
  //    2 |   0      1      2
  // でビットパタンとしては 10 01 00 01 01 01 00 01 00 = 0x24544 となる．

  std::uint8_t d1 = static_cast<std::uint8_t>(val1);
  std::uint8_t d2 = static_cast<std::uint8_t>(val2);
  std::uint8_t idx = (d1 * 3 + d2) * 2;
  return static_cast<Val3>((0x24544 >> idx) & 0x3);
}

/// @relates Val3
/// @brief OR 演算子
inline
Val3
operator|(
  Val3 val1, ///< [in] 値1
  Val3 val2  ///< [in] 値2
)
{
  //      |        val1
  // val2 |   0      1      2
  // -----+------------------
  //    0 |   0      0      2
  //    1 |   0      1      2
  //    2 |   2      2      2
  // でビットパタンとしては 10 10 10 10 01 00 10 00 00 = 0x2A920 となる．

  std::uint8_t d1 = static_cast<std::uint8_t>(val1);
  std::uint8_t d2 = static_cast<std::uint8_t>(val2);
  std::uint8_t idx = (d1 * 3 + d2) * 2;
  return static_cast<Val3>((0x2A920 >> idx) & 0x3);
}

/// @relates Val3
/// @brief XOR 演算子
inline
Val3
operator^(
  Val3 val1, ///< [in] 値1
  Val3 val2  ///< [in] 値2
)
{
  //      |        val1
  // val2 |   0      1      2
  // -----+------------------
  //    0 |   0      0      0
  //    1 |   0      1      2
  //    2 |   0      2      1
  // でビットパタンとしては 01 10 00 10 01 00 00 00 00 = 0x18900 となる．

  std::uint8_t d1 = static_cast<std::uint8_t>(val1);
  std::uint8_t d2 = static_cast<std::uint8_t>(val2);
  std::uint8_t idx = (d1 * 3 + d2) * 2;
  return static_cast<Val3>((0x18900 >> idx) & 0x3);
}

/// @relates Val3
/// @brief 値を表す文字を返す．
inline
char
value_name1(
  Val3 val ///< [in] 値
)
{
  switch ( val ) {
  case Val3::_X: return 'X';
  case Val3::_0: return '0';
  case Val3::_1: return '1';
  default:       return '-';
  }
}

/// @relates Val3
/// @brief 正常値/故障値の組の値を表す文字列を返す．
inline
const char*
value_name(
  Val3 gval, ///< [in] 正常値
  Val3 fval  ///< [in] 故障値
)
{
  switch ( gval ) {
  case Val3::_X :
    switch ( fval ) {
    case Val3::_X: return "X/X";
    case Val3::_0: return "X/0";
    case Val3::_1: return "X/1";
    }
    break;
  case Val3::_0:
    switch ( fval ) {
    case Val3::_X: return "0/X";
    case Val3::_0: return "0/0";
    case Val3::_1: return "0/1";
    }
    break;
  case Val3::_1:
    switch ( fval ) {
    case Val3::_X: return "1/X";
    case Val3::_0: return "1/0";
    case Val3::_1: return "1/1";
    }
    break;
  }
  return "illegal data";
}

/// @relates Val3
/// @brief ストリーム出力演算子
inline
std::ostream&
operator<<(
  std::ostream& s, ///< [in] 出力先のストリーム
  Val3 val    ///< [in] 値
)
{
  return s << value_name1(val);
}

/// @brief SatBool3 から Val3 への変換
inline
Val3
bool3_to_val3(
  SatBool3 bval
)
{
  switch ( bval ) {
  case SatBool3::True:  return Val3::_1;
  case SatBool3::False: return Val3::_0;
  case SatBool3::X:     return Val3::_X;
  default: break;
  }
  throw std::logic_error{"never be reached"};
}

END_NAMESPACE_DRUID

#endif // VAL3_H
