﻿#ifndef VAL3_H
#define VAL3_H

/// @file Val3.h
/// @brief 3値を表す型の定義ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2012-2014 Yusuke Matsunaga
/// All rights reserved.


#include "satpg.h"
#include "ym/SatBool3.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @brief 3値を表す列挙型
//////////////////////////////////////////////////////////////////////
enum Val3 {
  /// @brief 未定
  kValX = 0,
  /// @brief 0
  kVal0 = 1,
  /// @brief 1
  kVal1 = 2
};


/// @relates Val3
/// @brief 否定演算子
/// @param[in] val 値
/// @note kValX の場合には変化しない
inline
Val3
operator~(Val3 val)
{
  // val | ~val
  // ----+-----
  //   0 |    0
  //   1 |    2
  //   2 |    1
  // でビットパタンとしては 01 10 00 = 0x18 となる．

  ymuint d = static_cast<ymuint>(val) * 2;
  return static_cast<Val3>((0x18 >> d) & 0x3);
}

/// @relates Val3
/// @brief AND 演算子
/// @param[in] val1, val2 値
inline
Val3
operator&(Val3 val1,
	  Val3 val2)
{
  //      |        val1
  // val2 |   0      1      2
  // -----+------------------
  //    0 |   0      1      0
  //    1 |   1      1      1
  //    2 |   0      1      2
  // でビットパタンとしては 10 01 00 01 01 01 00 01 00 = 0x24544 となる．

  ymuint d1 = static_cast<ymuint>(val1);
  ymuint d2 = static_cast<ymuint>(val2);
  ymuint idx = (d1 * 3 + d2) * 2;
  return static_cast<Val3>((0x24544 >> idx) & 0x3);
}

/// @relates Val3
/// @brief OR 演算子
/// @param[in] val1, val2 値
inline
Val3
operator|(Val3 val1,
	  Val3 val2)
{
  //      |        val1
  // val2 |   0      1      2
  // -----+------------------
  //    0 |   0      0      2
  //    1 |   0      1      2
  //    2 |   2      2      2
  // でビットパタンとしては 10 10 10 10 01 00 10 00 00 = 0x2A920 となる．

  ymuint d1 = static_cast<ymuint>(val1);
  ymuint d2 = static_cast<ymuint>(val2);
  ymuint idx = (d1 * 3 + d2) * 2;
  return static_cast<Val3>((0x2A920 >> idx) & 0x3);
}

/// @relates Val3
/// @brief XOR 演算子
/// @param[in] val1, val2 値
inline
Val3
operator^(Val3 val1,
	  Val3 val2)
{
  //      |        val1
  // val2 |   0      1      2
  // -----+------------------
  //    0 |   0      0      0
  //    1 |   0      1      2
  //    2 |   0      2      1
  // でビットパタンとしては 01 10 00 10 01 00 00 00 00 = 0x18900 となる．

  ymuint d1 = static_cast<ymuint>(val1);
  ymuint d2 = static_cast<ymuint>(val2);
  ymuint idx = (d1 * 3 + d2) * 2;
  return static_cast<Val3>((0x18900 >> idx) & 0x3);
}

/// @relates Val3
/// @param[in] val 値
/// @brief 値を表す文字を返す．
inline
char
value_name1(Val3 val)
{
  if (val == kValX) {
    return 'X';
  }
  if (val == kVal0) {
    return '0';
  }
  if (val == kVal1) {
    return '1';
  }
  return '-';
}

/// @relates Val3
/// @brief 正常値/故障値の組の値を表す文字列を返す．
/// @param[in] gval 正常値
/// @param[in] fval 故障値
inline
const char*
value_name(Val3 gval,
	   Val3 fval)
{
  if (gval == kValX) {
    if (fval == kValX)      return "X/X";
    else if (fval == kVal0) return "X/0";
    else if (fval == kVal1) return "X/1";
  }
  else if (gval == kVal0) {
    if (fval == kValX)      return "0/X";
    else if (fval == kVal0) return "0/0";
    else if (fval == kVal1) return "0/1";
  }
  else if (gval == kVal1) {
    if (fval == kValX)      return "1/X";
    else if (fval == kVal0) return "1/0";
    else if (fval == kVal1) return "1/1";
  }
  return "illegal data";
}

/// @relates Val3
/// @brief ストリーム出力演算子
/// @param[in] s 出力先のストリーム
/// @param[in] val 値
inline
ostream&
operator<<(ostream& s,
	   Val3 val)
{
  return s << value_name1(val);
}

// @brief SatBool3 から Val3 への変換
inline
Val3
bool3_to_val3(SatBool3 bval)
{
  switch ( bval ) {
  case kB3True:  return kVal1;
  case kB3False: return kVal0;
  case kB3X:     return kValX;
  default: break;
  }
  ASSERT_NOT_REACHED;
  return kValX;
}

END_NAMESPACE_YM_SATPG

#endif // VAL3_H
