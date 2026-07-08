
/// @file DPatGraphTest.cc
/// @brief DPatGraphTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include <gtest/gtest.h>
#include "DPatGraph.h"


BEGIN_NAMESPACE_DRUID

TEST(DPatGraphTest, test1)
{
  PackedVal pat0 = 0;
  PackedVal pat1 = 1;
  DPat dpat0(8, pat0);
  DPat dpat1(8, pat1);
  std::vector<DPat> dpat_list{dpat0, dpat1};
  DPatGraph dpat_graph(dpat_list);

  auto succ_list0 = dpat_graph.imm_succ_list(dpat0);
  ASSERT_EQ( 1, succ_list0.size() );
  EXPECT_EQ( dpat1, succ_list0[0] );

  auto succ_list1 = dpat_graph.imm_succ_list(dpat1);
  EXPECT_EQ( 0, succ_list1.size() );
}

TEST(DPatGraphTest, test2)
{
  PackedVal pat0 = 0;
  PackedVal pat1 = 1;
  PackedVal pat2 = 2;
  PackedVal pat3 = 3;
  DPat dpat0(8, pat0);
  DPat dpat1(8, pat1);
  DPat dpat2(8, pat2);
  DPat dpat3(8, pat3);
  std::vector<DPat> dpat_list{dpat0, dpat1, dpat2, dpat3};
  DPatGraph dpat_graph(dpat_list);

  auto succ_list0 = dpat_graph.imm_succ_list(dpat0);
  ASSERT_EQ( 2, succ_list0.size() );
  EXPECT_EQ( dpat1, succ_list0[0] );
  EXPECT_EQ( dpat2, succ_list0[1] );

  auto succ_list1 = dpat_graph.imm_succ_list(dpat1);
  ASSERT_EQ( 1, succ_list1.size() );
  EXPECT_EQ( dpat3, succ_list1[0] );

  auto succ_list2 = dpat_graph.imm_succ_list(dpat2);
  ASSERT_EQ( 1, succ_list2.size() );
  EXPECT_EQ( dpat3, succ_list2[0] );

  auto succ_list3 = dpat_graph.imm_succ_list(dpat3);
  EXPECT_EQ( 0, succ_list3.size() );
}

TEST(DPatGraphTest, test3)
{
  PackedVal pat0 = 0;
  PackedVal pat1 = 1;
  PackedVal pat2 = 2;
  PackedVal pat3 = 3;
  DPat dpat0(8, pat0);
  DPat dpat1(8, pat1);
  DPat dpat2(8, pat2);
  DPat dpat3(8, pat3);
  std::vector<DPat> dpat_list{dpat0, dpat0, dpat0, dpat2, dpat0,
			      dpat0, dpat0, dpat0, dpat0, dpat0,
			      dpat2, dpat3, dpat0, dpat0, dpat0,
			      dpat2, dpat0, dpat0, dpat0, dpat0,
			      dpat1};
  DPatGraph dpat_graph(dpat_list);
  dpat_graph.print(std::cout);

  auto succ_list0 = dpat_graph.imm_succ_list(dpat0);
  ASSERT_EQ( 2, succ_list0.size() );
  EXPECT_EQ( dpat2, succ_list0[0] );
  EXPECT_EQ( dpat1, succ_list0[1] );

  auto succ_list1 = dpat_graph.imm_succ_list(dpat1);
  ASSERT_EQ( 1, succ_list1.size() );
  EXPECT_EQ( dpat3, succ_list1[0] );

  auto succ_list2 = dpat_graph.imm_succ_list(dpat2);
  ASSERT_EQ( 1, succ_list2.size() );
  EXPECT_EQ( dpat3, succ_list2[0] );

  auto succ_list3 = dpat_graph.imm_succ_list(dpat3);
  EXPECT_EQ( 0, succ_list3.size() );
}

END_NAMESPACE_DRUID
