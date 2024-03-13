
/// @file LFSR_test.cc
/// @brief LFSR_test の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include <gtest/gtest.h>
#include "LFSR.h"
#include "BitVector.h"


BEGIN_NAMESPACE_DRUID

TEST(LFSR_test, constructor1)
{
  LFSR lfsr{3, {1}};

  EXPECT_EQ( 3, lfsr.bitlen() );
  EXPECT_EQ( vector<SizeType>{1}, lfsr.tap_list() );
}

TEST(LFSR_test, set_bits1)
{
  LFSR lfsr{3, {1}};

  auto bv = BitVector::from_bin("100");
  lfsr.set_bits(bv);

  EXPECT_EQ( bv, lfsr.bits() );
}

TEST(LFSR_test, shift1)
{
  LFSR lfsr{3, {1}};

  auto bv = BitVector::from_bin("100");
  lfsr.set_bits(bv);

  EXPECT_EQ( "100", lfsr.bits().bin_str() );

  lfsr.shift();

  EXPECT_EQ( "010", lfsr.bits().bin_str() );

  lfsr.shift();

  EXPECT_EQ( "101", lfsr.bits().bin_str() );

  lfsr.shift();

  EXPECT_EQ( "110", lfsr.bits().bin_str() );

  lfsr.shift();

  EXPECT_EQ( "111", lfsr.bits().bin_str() );

  lfsr.shift();

  EXPECT_EQ( "011", lfsr.bits().bin_str() );

  lfsr.shift();

  EXPECT_EQ( "001", lfsr.bits().bin_str() );

  lfsr.shift();

  EXPECT_EQ( "100", lfsr.bits().bin_str() );
}

TEST(LFSR_test, rshift1)
{
  LFSR lfsr{3, {1}};

  auto bv = BitVector::from_bin("100");
  lfsr.set_bits(bv);

  EXPECT_EQ( "100", lfsr.bits().bin_str() );

  lfsr.rshift();

  EXPECT_EQ( "001", lfsr.bits().bin_str() );

  lfsr.rshift();

  EXPECT_EQ( "011", lfsr.bits().bin_str() );

  lfsr.rshift();

  EXPECT_EQ( "111", lfsr.bits().bin_str() );

  lfsr.rshift();

  EXPECT_EQ( "110", lfsr.bits().bin_str() );

  lfsr.rshift();

  EXPECT_EQ( "101", lfsr.bits().bin_str() );

  lfsr.rshift();

  EXPECT_EQ( "010", lfsr.bits().bin_str() );

  lfsr.rshift();

  EXPECT_EQ( "100", lfsr.bits().bin_str() );
}

END_NAMESPACE_DRUID
