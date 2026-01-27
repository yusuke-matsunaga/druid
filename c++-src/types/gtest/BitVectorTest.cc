
/// @file BitVectorTest.cc
/// @brief BitVectorTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.


#include "gtest/gtest.h"
#include "types/BitVector.h"
#include <random>


BEGIN_NAMESPACE_DRUID

// 長さ0のベクタ
TEST(BitVectorTest, constructor_0)
{
  BitVector bv(0);

  EXPECT_EQ( 0, bv.len() );
  EXPECT_EQ( 0, bv.x_count() );
  EXPECT_EQ( std::string(), bv.bin_str() );
  EXPECT_EQ( std::string(), bv.hex_str() );
}

// 長さ1のベクタ(0)
TEST(BitVectorTest, constructor_1_0)
{
  BitVector bv(1);

  bv.set_val(0, Val3::_0);

  EXPECT_EQ( 1, bv.len() );
  EXPECT_EQ( 0, bv.x_count() );
  EXPECT_EQ( Val3::_0, bv.val(0) );
  EXPECT_EQ( std::string("0"), bv.bin_str() );
  EXPECT_EQ( std::string("0"), bv.hex_str() );
}

// 長さ1のベクタ(1)
TEST(BitVectorTest, constructor_1_1)
{
  BitVector bv(1);

  bv.set_val(0, Val3::_1);

  EXPECT_EQ( 1, bv.len() );
  EXPECT_EQ( 0, bv.x_count() );
  EXPECT_EQ( Val3::_1, bv.val(0) );
  EXPECT_EQ( std::string("1"), bv.bin_str() );
  EXPECT_EQ( std::string("1"), bv.hex_str() );
}

// 長さ1のベクタ(X)
TEST(BitVectorTest, constructor_1_X)
{
  BitVector bv(1);

  bv.set_val(0, Val3::_X);

  EXPECT_EQ( 1, bv.len() );
  EXPECT_EQ( 1, bv.x_count() );
  EXPECT_EQ( Val3::_X, bv.val(0) );
  EXPECT_EQ( std::string("X"), bv.bin_str() );
  EXPECT_EQ( std::string("0"), bv.hex_str() );
}

// 長さ100のベクタ
TEST(BitVectorTest, constructor_100_0)
{
  int len = 100;
  BitVector bv(len);

  for ( int i = 0; i < len; ++ i ) {
    bv.set_val(i, Val3::_0);
  }

  EXPECT_EQ( len, bv.len() );
  EXPECT_EQ( 0, bv.x_count() );
  for ( int i = 0; i < len; ++ i ) {
    EXPECT_EQ( Val3::_0, bv.val(i) );
  }
  std::string str;
  for ( int i = 0; i < len; ++ i ) {
    str += "0";
  }
  EXPECT_EQ( str, bv.bin_str() );
}

// 長さ100のベクタ
TEST(BitVectorTest, constructor_100_1)
{
  int len = 100;
  BitVector bv(len);

  for ( int i = 0; i < len; ++ i ) {
    bv.set_val(i, Val3::_1);
  }

  EXPECT_EQ( len, bv.len() );
  EXPECT_EQ( 0, bv.x_count() );
  for ( int i = 0; i < len; ++ i ) {
    EXPECT_EQ( Val3::_1, bv.val(i) );
  }
  std::string str;
  for ( int i = 0; i < len; ++ i ) {
    str += "1";
  }
  EXPECT_EQ( str, bv.bin_str() );
}

// 長さ100のベクタ
TEST(BitVectorTest, constructor_100_X)
{
  int len = 100;
  BitVector bv(len);

  for ( int i = 0; i < len; ++ i ) {
    bv.set_val(i, Val3::_X);
  }

  EXPECT_EQ( len, bv.len() );
  EXPECT_EQ( len, bv.x_count() );
  for ( int i = 0; i < len; ++ i ) {
    EXPECT_EQ( Val3::_X, bv.val(i) );
  }
  std::string str;
  for ( int i = 0; i < len; ++ i ) {
    str += "X";
  }
  EXPECT_EQ( str, bv.bin_str() );
}

// 長さ100のベクタ
TEST(BitVectorTest, constructor_100_01)
{
  int len = 100;
  BitVector bv(len);

  for ( int i = 0; i < len; ++ i ) {
    Val3 val = (i % 2 == 0) ? Val3::_0 : Val3::_1;
    bv.set_val(i, val);
  }

  EXPECT_EQ( len, bv.len() );
  EXPECT_EQ( 0, bv.x_count() );
  for ( int i = 0; i < len; ++ i ) {
    Val3 val = (i % 2 == 0) ? Val3::_0 : Val3::_1;
    EXPECT_EQ( val, bv.val(i) );
  }
  std::string str;
  for ( int i = 0; i < len; ++ i ) {
    str += (i % 2 == 0) ? "0" : "1";
  }
  EXPECT_EQ( str, bv.bin_str() );
}

// 長さ100のベクタ
TEST(BitVectorTest, constructor_100_01X)
{
  int len = 100;
  BitVector bv(len);

  int xnum = 0;
  for ( int i = 0; i < len; ++ i ) {
    Val3 val;
    switch ( i % 3 ) {
    case 0: val = Val3::_0; break;
    case 1: val = Val3::_1; break;
    case 2: val = Val3::_X; ++ xnum; break;
    }
    bv.set_val(i, val);
  }

  EXPECT_EQ( len, bv.len() );
  EXPECT_EQ( xnum, bv.x_count() );
  for ( int i = 0; i < len; ++ i ) {
    Val3 val;
    switch ( i % 3 ) {
    case 0: val = Val3::_0; break;
    case 1: val = Val3::_1; break;
    case 2: val = Val3::_X; break;
    }
    EXPECT_EQ( val, bv.val(i) );
  }
  std::string str;
  for ( int i = 0; i < len; ++ i ) {
    switch ( i % 3 ) {
    case 0: str += "0"; break;
    case 1: str += "1"; break;
    case 2: str += "X"; break;
    }
  }
  EXPECT_EQ( str, bv.bin_str() );
}

// from_bin_str() のテスト
TEST(BitVectorTest, from_bin1)
{
  std::string bin_str = "01X";
  BitVector bv = BitVector::from_bin(bin_str);

  EXPECT_EQ( bin_str.size(), bv.len() );
  EXPECT_EQ( bin_str, bv.bin_str() );
}

// from_bin_str() のテスト
TEST(BitVectorTest, from_bin2)
{
  std::string bin_str;
  for ( int i = 0; i < 100; ++ i ) {
    bin_str += "01X";
  }
  BitVector bv = BitVector::from_bin(bin_str);

  EXPECT_EQ( bin_str.size(), bv.len() );
  EXPECT_EQ( bin_str, bv.bin_str() );
}

// from_hex_str() のテスト
TEST(BitVectorTest, from_hex1)
{
  int len = 13;
  std::string hex_str = "A5F0";
  BitVector bv = BitVector::from_hex(len, hex_str);

  EXPECT_EQ( len, bv.len() );
  EXPECT_EQ( hex_str, bv.hex_str() );
}

// from_hex_str() のテスト
TEST(BitVectorTest, from_hex2)
{
  int len = 13;
  std::string hex_str = "A5FF";
  BitVector bv = BitVector::from_hex(len, hex_str);

  EXPECT_EQ( len, bv.len() );
  EXPECT_EQ( "A5F1", bv.hex_str() );
}

// init() のテスト
TEST(BitVectorTest, init)
{
  int len = 100;
  BitVector bv(len);

  for ( int i = 0; i < len; ++ i ) {
    bv.set_val(i, Val3::_0);
  }
  for ( int i = 0; i < len; ++ i ) {
    EXPECT_EQ( Val3::_0, bv.val(i) );
  }

  bv.init();

  for ( int i = 0; i < len; ++ i ) {
    EXPECT_EQ( Val3::_X, bv.val(i) );
  }
}

// set_from_hex() のテスト
TEST(BitVectorTest, set_from_hex)
{
  std::string hex_str = "A50F";
  int len = hex_str.size() * 4;
  BitVector bv(len);

  bv.set_from_hex(hex_str);

  EXPECT_EQ( hex_str, bv.hex_str() );
}

// uniquefy のテスト (set_val)
TEST(BitVectorTest, uniq_set_val)
{
  // bv0 をすべて0で初期化
  int len = 100;
  BitVector bv0(len);
  for ( int i = 0; i < len; ++ i ) {
    bv0.set_val(i, Val3::_0);
  }

  // bv0 を bv1 にコピー
  BitVector bv1(bv0);

  // bv1 の 0 ビット目を 1 に変更
  bv1.set_val(0, Val3::_1);
  EXPECT_EQ( Val3::_1, bv1.val(0) );

  // bv0 が元のままであることを確認．
  for ( int i = 0; i < len; ++ i ) {
    EXPECT_EQ( Val3::_0, bv0.val(i) );
  }
}

// uniquefy のテスト (set_from_hex)
TEST(BitVectorTest, uniq_set_from_hex)
{
  std::string hex_str = "A50F";
  int len = hex_str.size() * 4;
  BitVector bv0(len);
  for ( int i = 0; i < len; ++ i ) {
    bv0.set_val(i, Val3::_0);
  }

  // bv0 を bv1 にコピー
  BitVector bv1(bv0);

  // bv1 を hex_str で再設定
  bv1.set_from_hex(hex_str);
  EXPECT_EQ( hex_str, bv1.hex_str() );

  // bv0 が元のままであることを確認．
  for ( int i = 0; i < len; ++ i ) {
    EXPECT_EQ( Val3::_0, bv0.val(i) );
  }
}

// uniquefy のテスト (set_from_random)
TEST(BitVectorTest, uniq_set_from_random)
{
  std::string hex_str = "A50F";
  int len = hex_str.size() * 4;
  BitVector bv0(len);
  for ( int i = 0; i < len; ++ i ) {
    bv0.set_val(i, Val3::_0);
  }

  // bv0 を bv1 にコピー
  BitVector bv1(bv0);

  // bv1 を乱数で再設定
  std::mt19937 rg;
  bv1.set_from_random(rg);

  // bv0 が元のままであることを確認．
  for ( int i = 0; i < len; ++ i ) {
    EXPECT_EQ( Val3::_0, bv0.val(i) );
  }
}

// && のテスト
TEST(BitVectorTest, compat)
{
  auto bv1 = BitVector::from_bin("01X0");
  auto bv2 = BitVector::from_bin("000X");
  auto bv3 = BitVector::from_bin("X110");

  EXPECT_FALSE( bv1 && bv2 );
  EXPECT_FALSE( bv2 && bv3 );
  EXPECT_TRUE ( bv3 && bv1 );
}

// == のテスト
TEST(BitVectorTest, equal)
{
  auto bv1 = BitVector::from_bin("01X0");
  auto bv2 = BitVector::from_bin("000X");
  auto bv3 = BitVector::from_bin("01X0");

  EXPECT_FALSE( bv1 == bv2 );
  EXPECT_TRUE ( bv1 != bv2 );
  EXPECT_FALSE( bv2 == bv3 );
  EXPECT_TRUE ( bv2 != bv3 );
  EXPECT_TRUE ( bv3 == bv1 );
  EXPECT_FALSE( bv3 != bv1 );
}

// < のテスト
TEST(BitVectorTest, less_than)
{
  auto bv1 = BitVector::from_bin("01X0");
  auto bv2 = BitVector::from_bin("000X");
  auto bv3 = BitVector::from_bin("X1X0");
  auto bv4 = BitVector::from_bin("01X0");

  EXPECT_FALSE( bv1 < bv2 );
  EXPECT_FALSE( bv2 > bv1 );
  EXPECT_FALSE( bv2 < bv3 );
  EXPECT_FALSE( bv3 > bv2 );
  EXPECT_TRUE ( bv1 < bv3 );
  EXPECT_TRUE ( bv3 > bv1 );

  // bv1 == bv4 なので bv1 < bv4 ではない．
  EXPECT_FALSE( bv1 < bv4 );
  EXPECT_FALSE( bv4 > bv1 );
}

// <= のテスト
TEST(BitVectorTest, less_than_or_equal)
{
  auto bv1 = BitVector::from_bin("01X0");
  auto bv2 = BitVector::from_bin("000X");
  auto bv3 = BitVector::from_bin("X1X0");
  auto bv4 = BitVector::from_bin("01X0");

  EXPECT_FALSE( bv1 <= bv2 );
  EXPECT_FALSE( bv2 >= bv1 );
  EXPECT_FALSE( bv2 <= bv3 );
  EXPECT_FALSE( bv3 >= bv2 );
  EXPECT_TRUE ( bv1 <= bv3 );
  EXPECT_TRUE ( bv3 >= bv1 );

  // bv1 == bv4 なので bv1 <= bv4 もなりたつ．
  EXPECT_TRUE ( bv1 <= bv4 );
  EXPECT_TRUE ( bv4 >= bv1 );
}

// &= のテスト
TEST(BitVectorTest, merge_assign)
{
  auto bv1 = BitVector::from_bin("0X1X");
  auto bv2 = BitVector::from_bin("01X0");
  auto bv3 = BitVector::from_bin("0110");

  ASSERT_TRUE( bv1 && bv2 );

  bv1 &= bv2;

  EXPECT_EQ( bv3, bv1 );
}

// & のテスト
TEST(BitVectorTest, merge)
{
  auto bv1 = BitVector::from_bin("0X1X");
  auto bv2 = BitVector::from_bin("01X0");
  auto bv3 = BitVector::from_bin("0110");

  ASSERT_TRUE( bv1 && bv2 );

  auto bv4 = bv1 & bv2;

  EXPECT_EQ( bv3, bv4 );
}

// lshift() のテスト
TEST(BitVectorTest, lshift1)
{
  std::string bin_str = "01X0";
  BitVector bv = BitVector::from_bin(bin_str);

  bv.lshift(Val3::_1);

  EXPECT_EQ( bin_str.size(), bv.len() );
  EXPECT_EQ( "101X", bv.bin_str());
}

// lshift() のテスト
TEST(BitVectorTest, lshift2)
{
  std::string bin_str = "01X0";
  BitVector bv = BitVector::from_bin(bin_str);

  bv.lshift(Val3::_0);

  EXPECT_EQ( bin_str.size(), bv.len() );
  EXPECT_EQ( "001X", bv.bin_str());
}

// lshift() のテスト
TEST(BitVectorTest, lshift3)
{
  std::string bin_str = "01X0";
  BitVector bv = BitVector::from_bin(bin_str);

  bv.lshift(Val3::_X);

  EXPECT_EQ( bin_str.size(), bv.len() );
  EXPECT_EQ( "X01X", bv.bin_str());
}

// rshift() のテスト
TEST(BitVectorTest, rshift1)
{
  std::string bin_str = "01X0";
  BitVector bv = BitVector::from_bin(bin_str);

  bv.rshift(Val3::_1);

  EXPECT_EQ( bin_str.size(), bv.len() );
  EXPECT_EQ( "1X01", bv.bin_str());
}

// rshift() のテスト
TEST(BitVectorTest, rshift2)
{
  std::string bin_str = "01X0";
  BitVector bv = BitVector::from_bin(bin_str);

  bv.rshift(Val3::_0);

  EXPECT_EQ( bin_str.size(), bv.len() );
  EXPECT_EQ( "1X00", bv.bin_str());
}

// rshift() のテスト
TEST(BitVectorTest, rshift3)
{
  std::string bin_str = "01X0";
  BitVector bv = BitVector::from_bin(bin_str);

  bv.rshift(Val3::_X);

  EXPECT_EQ( bin_str.size(), bv.len() );
  EXPECT_EQ( "1X0X", bv.bin_str());
}

END_NAMESPACE_DRUID
