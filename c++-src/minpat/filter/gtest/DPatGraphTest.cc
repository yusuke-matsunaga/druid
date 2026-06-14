
/// @file DPatGraphTest.cc
/// @brief DPatGraphTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include <gtest/gtest.h>
#include "DPatGraph.h"
#include "types/PackedVal.h"


BEGIN_NAMESPACE_DRUID

TEST(DPatGraphTest, test1)
{
  PackedVal pat0 = 0;
  PackedVal pat1 = 1;
  std::vector<PackedVal> dpat_list{pat0, pat1};
  DPatGraph dpat_graph(dpat_list);

  auto succ_list0 = dpat_graph.imm_succ_list(pat0);
  ASSERT_EQ( 1, succ_list0.size() );
  EXPECT_EQ( pat1, succ_list0[0] );

  auto succ_list1 = dpat_graph.imm_succ_list(pat1);
  EXPECT_EQ( 0, succ_list1.size() );
}

TEST(DPatGraphTest, test2)
{
  PackedVal pat0 = 0;
  PackedVal pat1 = 1;
  PackedVal pat2 = 2;
  PackedVal pat3 = 3;
  std::vector<PackedVal> dpat_list{pat0, pat1, pat2, pat3};
  DPatGraph dpat_graph(dpat_list);

  auto succ_list0 = dpat_graph.imm_succ_list(pat0);
  ASSERT_EQ( 2, succ_list0.size() );
  EXPECT_EQ( pat1, succ_list0[0] );
  EXPECT_EQ( pat2, succ_list0[1] );

  auto succ_list1 = dpat_graph.imm_succ_list(pat1);
  ASSERT_EQ( 1, succ_list1.size() );
  EXPECT_EQ( pat3, succ_list1[0] );

  auto succ_list2 = dpat_graph.imm_succ_list(pat2);
  ASSERT_EQ( 1, succ_list2.size() );
  EXPECT_EQ( pat3, succ_list2[0] );

  auto succ_list3 = dpat_graph.imm_succ_list(pat3);
  EXPECT_EQ( 0, succ_list3.size() );
}

TEST(DPatGraphTest, test3)
{
  PackedVal pat0 = 0;
  PackedVal pat1 = 1;
  PackedVal pat2 = 2;
  PackedVal pat3 = 3;
  std::vector<PackedVal> dpat_list{pat0, pat0, pat0, pat2, pat0,
				   pat0, pat0, pat0, pat0, pat0,
				   pat2, pat3, pat0, pat0, pat0,
				   pat2, pat0, pat0, pat0, pat0,
				   pat1};
  DPatGraph dpat_graph(dpat_list);
  dpat_graph.print(std::cout);

  auto succ_list0 = dpat_graph.imm_succ_list(pat0);
  ASSERT_EQ( 2, succ_list0.size() );
  EXPECT_EQ( pat2, succ_list0[0] );
  EXPECT_EQ( pat1, succ_list0[1] );

  auto succ_list1 = dpat_graph.imm_succ_list(pat1);
  ASSERT_EQ( 1, succ_list1.size() );
  EXPECT_EQ( pat3, succ_list1[0] );

  auto succ_list2 = dpat_graph.imm_succ_list(pat2);
  ASSERT_EQ( 1, succ_list2.size() );
  EXPECT_EQ( pat3, succ_list2[0] );

  auto succ_list3 = dpat_graph.imm_succ_list(pat3);
  EXPECT_EQ( 0, succ_list3.size() );
}

END_NAMESPACE_DRUID
