
/// @file GateTypeTest.cc
/// @brief GateTypeTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include <gtest/gtest.h>
#include "GateType.h"
#include "types/Val3.h"


BEGIN_NAMESPACE_DRUID

class GateTypeTest:
  public ::testing::Test
{
public:

  void
  check_cval(
    const GateType* gate_type,
    const std::vector<Val3>& table
  )
  {
    Val3 val3_list[] = {Val3::_X, Val3::_0, Val3::_1};
    for ( SizeType i = 0; i < gate_type->input_num(); ++ i ) {
      for ( SizeType j = 0; j < 3; ++ j ) {
	auto val = val3_list[j];
	auto oval = gate_type->cval(i, val);
	auto exp_oval = table[i * 3 + j];
	EXPECT_EQ( exp_oval, oval )
	  << "pos = " << i
	  << ", val = " << val;
      }
    }
  }

};

TEST_F(GateTypeTest, ppi_type)
{
  SizeType id = 123;
  auto gate_type = GateType::new_ppi(id);
  ASSERT_TRUE( gate_type != nullptr );

  EXPECT_EQ( id, gate_type->id() );
  EXPECT_EQ( 0, gate_type->input_num() );
  EXPECT_TRUE ( gate_type->is_ppi() );
  EXPECT_FALSE( gate_type->is_ppo() );
  EXPECT_FALSE( gate_type->is_primitive() );
  EXPECT_THROW( gate_type->primitive_type(),
		std::logic_error );
  EXPECT_FALSE( gate_type->is_expr() );
  EXPECT_THROW( gate_type->expr(),
		std::logic_error );
  EXPECT_EQ( 0, gate_type->extra_node_num() );
}

TEST_F(GateTypeTest, ppo_type)
{
  SizeType id = 123;
  auto gate_type = GateType::new_ppo(id);
  ASSERT_TRUE( gate_type != nullptr );

  EXPECT_EQ( id, gate_type->id() );
  EXPECT_EQ( 1, gate_type->input_num() );
  EXPECT_FALSE( gate_type->is_ppi() );
  EXPECT_TRUE ( gate_type->is_ppo() );
  EXPECT_FALSE( gate_type->is_primitive() );
  EXPECT_THROW( gate_type->primitive_type(),
		std::logic_error );
  EXPECT_FALSE( gate_type->is_expr() );
  EXPECT_THROW( gate_type->expr(),
		std::logic_error );
  EXPECT_EQ( 0, gate_type->extra_node_num() );

  check_cval(gate_type, {Val3::_X, Val3::_X, Val3::_X});
}

TEST_F(GateTypeTest, C0)
{
  SizeType id = 123;
  auto prim_type = PrimType::C0;
  auto gate_type = GateType::new_primitive(id, 0, prim_type);
  ASSERT_TRUE( gate_type != nullptr );

  EXPECT_EQ( id, gate_type->id() );
  EXPECT_EQ( 0, gate_type->input_num() );
  EXPECT_FALSE( gate_type->is_ppi() );
  EXPECT_FALSE( gate_type->is_ppo() );
  EXPECT_TRUE( gate_type->is_primitive() );
  EXPECT_EQ( prim_type, gate_type->primitive_type() );
  EXPECT_FALSE( gate_type->is_expr() );
  EXPECT_THROW( gate_type->expr(),
		std::logic_error );
  EXPECT_EQ( 0, gate_type->extra_node_num() );
}

TEST_F(GateTypeTest, C1)
{
  SizeType id = 123;
  auto prim_type = PrimType::C1;
  auto gate_type = GateType::new_primitive(id, 0, prim_type);
  ASSERT_TRUE( gate_type != nullptr );

  EXPECT_EQ( id, gate_type->id() );
  EXPECT_EQ( 0, gate_type->input_num() );
  EXPECT_FALSE( gate_type->is_ppi() );
  EXPECT_FALSE( gate_type->is_ppo() );
  EXPECT_TRUE( gate_type->is_primitive() );
  EXPECT_EQ( prim_type, gate_type->primitive_type() );
  EXPECT_FALSE( gate_type->is_expr() );
  EXPECT_THROW( gate_type->expr(),
		std::logic_error );
  EXPECT_EQ( 0, gate_type->extra_node_num() );
}

TEST_F(GateTypeTest, BUFF)
{
  SizeType id = 123;
  auto prim_type = PrimType::Buff;
  auto gate_type = GateType::new_primitive(id, 1, prim_type);
  ASSERT_TRUE( gate_type != nullptr );

  EXPECT_EQ( id, gate_type->id() );
  EXPECT_EQ( 1, gate_type->input_num() );
  EXPECT_FALSE( gate_type->is_ppi() );
  EXPECT_FALSE( gate_type->is_ppo() );
  EXPECT_TRUE( gate_type->is_primitive() );
  EXPECT_EQ( prim_type, gate_type->primitive_type() );
  EXPECT_FALSE( gate_type->is_expr() );
  EXPECT_THROW( gate_type->expr(),
		std::logic_error );
  EXPECT_EQ( 0, gate_type->extra_node_num() );

  check_cval(gate_type, {Val3::_X, Val3::_0, Val3::_1});
}

TEST_F(GateTypeTest, NOT)
{
  SizeType id = 123;
  auto prim_type = PrimType::Not;
  auto gate_type = GateType::new_primitive(id, 1, prim_type);
  ASSERT_TRUE( gate_type != nullptr );

  EXPECT_EQ( id, gate_type->id() );
  EXPECT_EQ( 1, gate_type->input_num() );
  EXPECT_FALSE( gate_type->is_ppi() );
  EXPECT_FALSE( gate_type->is_ppo() );
  EXPECT_TRUE( gate_type->is_primitive() );
  EXPECT_EQ( prim_type, gate_type->primitive_type() );
  EXPECT_FALSE( gate_type->is_expr() );
  EXPECT_THROW( gate_type->expr(),
		std::logic_error );
  EXPECT_EQ( 0, gate_type->extra_node_num() );

  check_cval(gate_type, {Val3::_X, Val3::_1, Val3::_0});
}

TEST_F(GateTypeTest, AND2)
{
  SizeType id = 123;
  auto prim_type = PrimType::And;
  SizeType ni = 2;
  auto gate_type = GateType::new_primitive(id, ni, prim_type);
  ASSERT_TRUE( gate_type != nullptr );

  EXPECT_EQ( id, gate_type->id() );
  EXPECT_EQ( ni, gate_type->input_num() );
  EXPECT_FALSE( gate_type->is_ppi() );
  EXPECT_FALSE( gate_type->is_ppo() );
  EXPECT_TRUE( gate_type->is_primitive() );
  EXPECT_EQ( prim_type, gate_type->primitive_type() );
  EXPECT_FALSE( gate_type->is_expr() );
  EXPECT_THROW( gate_type->expr(),
		std::logic_error );
  EXPECT_EQ( 0, gate_type->extra_node_num() );

  check_cval(gate_type,
	     {Val3::_X, Val3::_0, Val3::_X,
	      Val3::_X, Val3::_0, Val3::_X});
}

TEST_F(GateTypeTest, AND3)
{
  SizeType id = 123;
  auto prim_type = PrimType::And;
  SizeType ni = 3;
  auto gate_type = GateType::new_primitive(id, ni, prim_type);
  ASSERT_TRUE( gate_type != nullptr );

  EXPECT_EQ( id, gate_type->id() );
  EXPECT_EQ( ni, gate_type->input_num() );
  EXPECT_FALSE( gate_type->is_ppi() );
  EXPECT_FALSE( gate_type->is_ppo() );
  EXPECT_TRUE( gate_type->is_primitive() );
  EXPECT_EQ( prim_type, gate_type->primitive_type() );
  EXPECT_FALSE( gate_type->is_expr() );
  EXPECT_THROW( gate_type->expr(),
		std::logic_error );
  EXPECT_EQ( 0, gate_type->extra_node_num() );

  check_cval(gate_type,
	     {Val3::_X, Val3::_0, Val3::_X,
	      Val3::_X, Val3::_0, Val3::_X,
	      Val3::_X, Val3::_0, Val3::_X});
}

TEST_F(GateTypeTest, NAND2)
{
  SizeType id = 123;
  auto prim_type = PrimType::Nand;
  SizeType ni = 2;
  auto gate_type = GateType::new_primitive(id, ni, prim_type);
  ASSERT_TRUE( gate_type != nullptr );

  EXPECT_EQ( id, gate_type->id() );
  EXPECT_EQ( ni, gate_type->input_num() );
  EXPECT_FALSE( gate_type->is_ppi() );
  EXPECT_FALSE( gate_type->is_ppo() );
  EXPECT_TRUE( gate_type->is_primitive() );
  EXPECT_EQ( prim_type, gate_type->primitive_type() );
  EXPECT_FALSE( gate_type->is_expr() );
  EXPECT_THROW( gate_type->expr(),
		std::logic_error );
  EXPECT_EQ( 0, gate_type->extra_node_num() );

  check_cval(gate_type,
	     {Val3::_X, Val3::_1, Val3::_X,
	      Val3::_X, Val3::_1, Val3::_X});
}

TEST_F(GateTypeTest, OR2)
{
  SizeType id = 123;
  auto prim_type = PrimType::Or;
  SizeType ni = 2;
  auto gate_type = GateType::new_primitive(id, ni, prim_type);
  ASSERT_TRUE( gate_type != nullptr );

  EXPECT_EQ( id, gate_type->id() );
  EXPECT_EQ( ni, gate_type->input_num() );
  EXPECT_FALSE( gate_type->is_ppi() );
  EXPECT_FALSE( gate_type->is_ppo() );
  EXPECT_TRUE( gate_type->is_primitive() );
  EXPECT_EQ( prim_type, gate_type->primitive_type() );
  EXPECT_FALSE( gate_type->is_expr() );
  EXPECT_THROW( gate_type->expr(),
		std::logic_error );
  EXPECT_EQ( 0, gate_type->extra_node_num() );

  check_cval(gate_type,
	     {Val3::_X, Val3::_X, Val3::_1,
	      Val3::_X, Val3::_X, Val3::_1});
}

TEST_F(GateTypeTest, NOR2)
{
  SizeType id = 123;
  auto prim_type = PrimType::Nor;
  SizeType ni = 2;
  auto gate_type = GateType::new_primitive(id, ni, prim_type);
  ASSERT_TRUE( gate_type != nullptr );

  EXPECT_EQ( id, gate_type->id() );
  EXPECT_EQ( ni, gate_type->input_num() );
  EXPECT_FALSE( gate_type->is_ppi() );
  EXPECT_FALSE( gate_type->is_ppo() );
  EXPECT_TRUE( gate_type->is_primitive() );
  EXPECT_EQ( prim_type, gate_type->primitive_type() );
  EXPECT_FALSE( gate_type->is_expr() );
  EXPECT_THROW( gate_type->expr(),
		std::logic_error );
  EXPECT_EQ( 0, gate_type->extra_node_num() );

  check_cval(gate_type,
	     {Val3::_X, Val3::_X, Val3::_0,
	      Val3::_X, Val3::_X, Val3::_0});
}

TEST_F(GateTypeTest, XOR2)
{
  SizeType id = 123;
  auto prim_type = PrimType::Xor;
  SizeType ni = 2;
  auto gate_type = GateType::new_primitive(id, ni, prim_type);
  ASSERT_TRUE( gate_type != nullptr );

  EXPECT_EQ( id, gate_type->id() );
  EXPECT_EQ( ni, gate_type->input_num() );
  EXPECT_FALSE( gate_type->is_ppi() );
  EXPECT_FALSE( gate_type->is_ppo() );
  EXPECT_TRUE( gate_type->is_primitive() );
  EXPECT_EQ( prim_type, gate_type->primitive_type() );
  EXPECT_FALSE( gate_type->is_expr() );
  EXPECT_THROW( gate_type->expr(),
		std::logic_error );
  EXPECT_EQ( 0, gate_type->extra_node_num() );

  check_cval(gate_type,
	     {Val3::_X, Val3::_X, Val3::_X,
	      Val3::_X, Val3::_X, Val3::_X});
}

TEST_F(GateTypeTest, XNOR2)
{
  SizeType id = 123;
  auto prim_type = PrimType::Xnor;
  SizeType ni = 2;
  auto gate_type = GateType::new_primitive(id, ni, prim_type);
  ASSERT_TRUE( gate_type != nullptr );

  EXPECT_EQ( id, gate_type->id() );
  EXPECT_EQ( ni, gate_type->input_num() );
  EXPECT_FALSE( gate_type->is_ppi() );
  EXPECT_FALSE( gate_type->is_ppo() );
  EXPECT_TRUE( gate_type->is_primitive() );
  EXPECT_EQ( prim_type, gate_type->primitive_type() );
  EXPECT_FALSE( gate_type->is_expr() );
  EXPECT_THROW( gate_type->expr(),
		std::logic_error );
  EXPECT_EQ( 0, gate_type->extra_node_num() );

  check_cval(gate_type,
	     {Val3::_X, Val3::_X, Val3::_X,
	      Val3::_X, Val3::_X, Val3::_X});
}

TEST_F(GateTypeTest, AND3_EXPR)
{
  SizeType id = 123;
  SizeType ni = 3;
  auto v0 = Expr::literal(0);
  auto v1 = Expr::literal(1);
  auto v2 = Expr::literal(2);
  auto and_expr = v0 & v1 & v2;
  auto gate_type = GateType::new_cplx(id, ni, and_expr);
  ASSERT_TRUE( gate_type != nullptr );

  EXPECT_EQ( id, gate_type->id() );
  EXPECT_EQ( ni, gate_type->input_num() );
  EXPECT_FALSE( gate_type->is_ppi() );
  EXPECT_FALSE( gate_type->is_ppo() );
  EXPECT_TRUE( gate_type->is_primitive() );
  EXPECT_EQ( PrimType::And, gate_type->primitive_type() );
  EXPECT_FALSE( gate_type->is_expr() );
  EXPECT_THROW( gate_type->expr(),
		std::logic_error );
  EXPECT_EQ( 0, gate_type->extra_node_num() );

  check_cval(gate_type,
	     {Val3::_X, Val3::_0, Val3::_X,
	      Val3::_X, Val3::_0, Val3::_X,
	      Val3::_X, Val3::_0, Val3::_X});
}

TEST_F(GateTypeTest, AO21)
{
  SizeType id = 123;
  SizeType ni = 3;
  auto v0 = Expr::literal(0);
  auto v1 = Expr::literal(1);
  auto v2 = Expr::literal(2);
  auto ao_expr = v0 & v1 | v2;
  auto gate_type = GateType::new_cplx(id, ni, ao_expr);
  ASSERT_TRUE( gate_type != nullptr );

  EXPECT_EQ( id, gate_type->id() );
  EXPECT_EQ( ni, gate_type->input_num() );
  EXPECT_FALSE( gate_type->is_ppi() );
  EXPECT_FALSE( gate_type->is_ppo() );
  EXPECT_FALSE( gate_type->is_primitive() );
  EXPECT_THROW( gate_type->primitive_type(),
		std::logic_error );
  EXPECT_TRUE ( gate_type->is_expr() );
  EXPECT_EQ( ao_expr, gate_type->expr() );
  EXPECT_EQ( 1, gate_type->extra_node_num() );

  check_cval(gate_type,
	     {Val3::_X, Val3::_X, Val3::_X,
	      Val3::_X, Val3::_X, Val3::_X,
	      Val3::_X, Val3::_X, Val3::_1});
}

END_NAMESPACE_DRUID
