
/// @file DPatTest.cc
/// @brief DPatTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include <gtest/gtest.h>
#include "DPat.h"


BEGIN_NAMESPACE_DRUID

TEST(DPatTest, constr1)
{
  SizeType SIZE = 5;
  DPat dpat(SIZE);

  EXPECT_EQ( SIZE, dpat.size() );
  for ( SizeType i = 0; i < SIZE; ++ i ) {
    EXPECT_FALSE( dpat.get(i) );
  }
}

TEST(DPatTest, constr2)
{
  SizeType SIZE = 5;
  PackedVal PAT0 = 3;
  DPat dpat(SIZE, PAT0);

  EXPECT_EQ( SIZE, dpat.size() );
  for ( SizeType i = 0; i < SIZE; ++ i ) {
    bool exp_val = ((1ULL << i) & PAT0) ? true : false;
    EXPECT_EQ( exp_val, dpat.get(i) );
  }
}

END_NAMESPACE_DRUID
