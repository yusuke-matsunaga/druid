
/// @file Expr2CnfTest.cc
/// @brief Expr2Cnf のテストプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include <gtest/gtest.h>
#include "Expr2Cnf.h"


BEGIN_NAMESPACE_DRUID

class Expr2CnfTest:
  public ::testing::Test
{
public:

  void
  SetUp()
  {
  }

  void
  TearDown()
  {
  }

  // SAT ソルバ
  SatSolver mSolver;

};


TEST_F(Expr2CnfTest, one)
{
  std::unordered_map<SizeType, SatLiteral> lit_map;
  Expr2Cnf conv(mSolver, lit_map);

  auto lits = conv.make_cnf(Expr::one());
  EXPECT_TRUE( lits.empty() );
}

TEST_F(Expr2CnfTest, lit1)
{
  std::unordered_map<SizeType, SatLiteral> lit_map;
  auto lit0 = mSolver.new_variable(true);
  lit_map.emplace(0, lit0);
  Expr2Cnf conv(mSolver, lit_map);

  auto lits = conv.make_cnf(Expr::literal(0));
  ASSERT_EQ( 1, lits.size() );
  EXPECT_EQ( lit0, lits.front() );
}

TEST_F(Expr2CnfTest, lit2)
{
  std::unordered_map<SizeType, SatLiteral> lit_map;
  auto lit0 = mSolver.new_variable(true);
  auto lit1 = mSolver.new_variable(true);
  lit_map.emplace(0, lit0);
  lit_map.emplace(1, lit1);
  Expr2Cnf conv(mSolver, lit_map);

  auto lits = conv.make_cnf(Expr::literal(1, true));
  ASSERT_EQ( 1, lits.size() );
  EXPECT_EQ( ~lit1, lits.front() );
}

TEST_F(Expr2CnfTest, and1)
{
  std::unordered_map<SizeType, SatLiteral> lit_map;
  auto lit0 = mSolver.new_variable(true);
  auto lit1 = mSolver.new_variable(true);
  lit_map.emplace(0, lit0);
  lit_map.emplace(1, lit1);
  Expr2Cnf conv(mSolver, lit_map);

  auto expr = Expr::literal(0) & Expr::literal(1);
  auto lits = conv.make_cnf(expr);
  ASSERT_EQ( 2, lits.size() );
  EXPECT_EQ( lit0, lits[0] );
  EXPECT_EQ( lit1, lits[1] );
}

TEST_F(Expr2CnfTest, or1)
{
  std::unordered_map<SizeType, SatLiteral> lit_map;
  auto lit0 = mSolver.new_variable(true);
  auto lit1 = mSolver.new_variable(true);
  lit_map.emplace(0, lit0);
  lit_map.emplace(1, lit1);
  Expr2Cnf conv(mSolver, lit_map);

  auto expr = Expr::literal(0) | Expr::literal(1);
  auto lits = conv.make_cnf(expr);
  EXPECT_EQ( 1, lits.size() );

  auto assumptions = lits;
  assumptions.push_back(~lit0);
  assumptions.push_back(~lit1);

  auto res = mSolver.solve(assumptions);
  EXPECT_EQ( SatBool3::False, res );
}

END_NAMESPACE_DRUID
