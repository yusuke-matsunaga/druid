
/// @file Val3Test.cc
/// @brief Val3Test の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.

#include "gtest/gtest.h"
#include "types/Val3.h"


BEGIN_NAMESPACE_DRUID

TEST(Val3Test, negation)
{
  EXPECT_EQ( Val3::_X, ~(Val3::_X) );
  EXPECT_EQ( Val3::_1, ~(Val3::_0) );
  EXPECT_EQ( Val3::_0, ~(Val3::_1) );
}

TEST(Val3Test, and_op)
{
  Val3 v_list[] = { Val3::_X, Val3::_0, Val3::_1 };

  for ( auto i: { 0, 1, 2 } ) {
    auto v1 = v_list[i];
    for ( auto j: { 0, 1, 2 } ) {
      auto v2 = v_list[j];
      auto ans = Val3::_X;
      if ( v1 == Val3::_0 || v2 == Val3::_0 ) {
	ans = Val3::_0;
      }
      else if ( v1 == Val3::_1 && v2 == Val3::_1 ) {
	ans = Val3::_1;
      }
      EXPECT_EQ( ans, v1 & v2 );
    }
  }
}

TEST(Val3Test, or_op)
{
  Val3 v_list[] = { Val3::_X, Val3::_0, Val3::_1 };

  for ( auto i: { 0, 1, 2 } ) {
    auto v1 = v_list[i];
    for ( auto j: { 0, 1, 2 } ) {
      auto v2 = v_list[j];
      auto ans = Val3::_X;
      if ( v1 == Val3::_1 || v2 == Val3::_1 ) {
	ans = Val3::_1;
      }
      else if ( v1 == Val3::_0 && v2 == Val3::_0 ) {
	ans = Val3::_0;
      }
      EXPECT_EQ( ans, v1 | v2 );
    }
  }
}

TEST(Val3Test, xor_op)
{
  Val3 v_list[] = { Val3::_X, Val3::_0, Val3::_1 };

  for ( auto i: { 0, 1, 2 } ) {
    auto v1 = v_list[i];
    for ( auto j: { 0, 1, 2 } ) {
      auto v2 = v_list[j];
      auto ans = Val3::_X;
      if ( v1 == Val3::_0 ) {
	if ( v2 == Val3::_0 ) {
	  ans = Val3::_0;
	}
	else if ( v2 == Val3::_1 ) {
	  ans = Val3::_1;
	}
      }
      else if ( v1 == Val3::_1 ) {
	if ( v2 == Val3::_0 ) {
	  ans = Val3::_1;
	}
	else if ( v2 == Val3::_1 ) {
	  ans = Val3::_0;
	}
      }
      EXPECT_EQ( ans, v1 ^ v2 );
    }
  }
}

END_NAMESPACE_DRUID
