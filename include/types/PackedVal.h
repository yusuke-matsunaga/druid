#ifndef PACKEDVAL_H
#define PACKEDVAL_H

/// @file PackedVal.h
/// @brief 1ワードにパックしたビットベクタ型の定義ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @brief 1ワードのビットベクタを表す型
//////////////////////////////////////////////////////////////////////
using PackedVal = std::uint64_t;

/// @brief 全てのビットが0の定数
const PackedVal PV_ALL0 = 0UL;

/// @brief 全てのビットが1の定数
const PackedVal PV_ALL1 = ~0UL;

/// @brief PackedVal のビット長
const int PV_BITLEN = 64;


/// @brief 2つのビットベクタの差分を求める．
///
/// 異なっているビットに1を立てた2値のビットベクタを返す．
inline
PackedVal
diff(
  PackedVal left, ///< [in] オペランド1
  PackedVal right ///< [in] オペランド2
)
{
  // って中身はただの XOR 演算
  return left ^ right;
}

/// @brief word 中の1のビット数を数える．
/// @return word 中の1のビット数
inline
SizeType
count_ones(
  PackedVal word /// @param[in] 対象のワード
)
{
  const PackedVal mask1  = 0x5555555555555555UL;
  const PackedVal mask2  = 0x3333333333333333UL;
  const PackedVal mask4  = 0x0f0f0f0f0f0f0f0fUL;
  const PackedVal mask8  = 0x00ff00ff00ff00ffUL;
  const PackedVal mask16 = 0x0000ffff0000ffffUL;
  const PackedVal mask32 = 0x00000000ffffffffUL;

  word = (word & mask1)  + ((word >>  1) & mask1);
  word = (word & mask2)  + ((word >>  2) & mask2);
  word = (word & mask4)  + ((word >>  4) & mask4);
  word = (word & mask8)  + ((word >>  8) & mask8);
  word = (word & mask16) + ((word >> 16) & mask16);
  word = (word & mask32) + ((word >> 32) & mask32);
  return word;
}

END_NAMESPACE_DRUID

#endif // PACKEDVAL_H
