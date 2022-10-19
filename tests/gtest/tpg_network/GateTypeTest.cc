
/// @file GateTypeTest.cc
/// @brief GateTypeTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.


#include "gtest/gtest.h"
#include "GateType.h"


BEGIN_NAMESPACE_DRUID

TEST(GateTypeTest, gate_type_to_int)
{
  EXPECT_EQ( 0, static_cast<int>(GateType::Const0) );
  EXPECT_EQ( 1, static_cast<int>(GateType::Const1) );
  EXPECT_EQ( 2, static_cast<int>(GateType::Input) );
  EXPECT_EQ( 3, static_cast<int>(GateType::Buff) );
  EXPECT_EQ( 4, static_cast<int>(GateType::Not) );
  EXPECT_EQ( 5, static_cast<int>(GateType::And) );
  EXPECT_EQ( 6, static_cast<int>(GateType::Nand) );
  EXPECT_EQ( 7, static_cast<int>(GateType::Or) );
  EXPECT_EQ( 8, static_cast<int>(GateType::Nor) );
  EXPECT_EQ( 9, static_cast<int>(GateType::Xor) );
  EXPECT_EQ(10, static_cast<int>(GateType::Xnor) );
}

#if 0
TEST(GateTypeTest, int_to_gate_type)
{
  EXPECT_EQ( GateType::Const0, __int_to_gate_type(0) );
  EXPECT_EQ( GateType::Const1, __int_to_gate_type(1) );
  EXPECT_EQ( GateType::Input,  __int_to_gate_type(2) );
  EXPECT_EQ( GateType::Buff,   __int_to_gate_type(3) );
  EXPECT_EQ( GateType::Not,    __int_to_gate_type(4) );
  EXPECT_EQ( GateType::And,    __int_to_gate_type(5) );
  EXPECT_EQ( GateType::Nand,   __int_to_gate_type(6) );
  EXPECT_EQ( GateType::Or,     __int_to_gate_type(7) );
  EXPECT_EQ( GateType::Nor,    __int_to_gate_type(8) );
  EXPECT_EQ( GateType::Xor,    __int_to_gate_type(9) );
  EXPECT_EQ( GateType::Xnor,   __int_to_gate_type(10) );
}

TEST(GateTypeTest, loop)
{
  for ( auto i: { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 } ) {
    EXPECT_EQ( i, static_cast<int>(__int_to_gate_type(i)) );
  }
}
#endif

END_NAMESPACE_DRUID
