
/// @file GateRepTest.cc
/// @brief GateRepTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include <gtest/gtest.h>
#include "GateType.h"
#include "GateRep.h"
#include "NodeRep.h"
#include "types/FaultType.h"


BEGIN_NAMESPACE_DRUID

class GateRepTest:
  public ::testing::Test
{
public:

  // ファンインのリストを作る．
  std::vector<const NodeRep*>
  make_fanin_list(
    SizeType ni
  )
  {
    std::vector<const NodeRep*> fanin_list(ni);
    for ( SizeType i = 0; i < ni; ++ i ) {
      fanin_list[i] = NodeRep::new_input(i, i);
    }
    return fanin_list;
  }

};

TEST_F(GateRepTest, C0_SA)
{
  SizeType tid = 1;
  auto prim_type = PrimType::C0;
  SizeType ni = 0;
  auto gate_type = GateType::new_primitive(tid, ni, prim_type);
  ASSERT_TRUE( gate_type != nullptr );
  SizeType nid = 2;
  auto node = NodeRep::new_logic(nid, prim_type, {});
  SizeType gid = 3;
  auto gate_rep = GateRep::new_primitive(gid, gate_type, node, FaultType::StuckAt);
  ASSERT_TRUE( gate_rep != nullptr );

  EXPECT_EQ( gid, gate_rep->id() );
  EXPECT_EQ( node, gate_rep->output_node() );
  EXPECT_EQ( ni, gate_rep->input_num() );
  EXPECT_FALSE( gate_rep->is_ppi() );
  EXPECT_FALSE( gate_rep->is_ppo() );
  EXPECT_TRUE ( gate_rep->is_simple() );
  EXPECT_FALSE( gate_rep->is_complex() );
  EXPECT_EQ( prim_type, gate_rep->primitive_type() );
  EXPECT_THROW( gate_rep->expr(),
		std::logic_error );
  EXPECT_EQ( 0, gate_rep->extra_node_num() );
}

TEST_F(GateRepTest, AND2)
{
  SizeType tid = 1;
  SizeType ni = 2;
  auto prim_type = PrimType::And;
  auto gate_type = GateType::new_primitive(tid, ni, prim_type);
  ASSERT_TRUE( gate_type != nullptr );
  SizeType nid = 20;
  auto fanin_list = make_fanin_list(ni);
  auto node = NodeRep::new_logic(nid, prim_type, fanin_list);
  SizeType gid = 3;
  auto gate_rep = GateRep::new_primitive(gid, gate_type, node, FaultType::StuckAt);
  ASSERT_TRUE( gate_rep != nullptr );

  EXPECT_EQ( gid, gate_rep->id() );
  EXPECT_EQ( node, gate_rep->output_node() );
  EXPECT_EQ( ni, gate_rep->input_num() );
  for ( SizeType i = 0; i < ni; ++ i ) {
    EXPECT_EQ( fanin_list[i], gate_rep->input_node(i) );
    auto bi = gate_rep->branch_info(i);
    EXPECT_EQ( node, bi.node );
    EXPECT_EQ( i, bi.ipos);
  }
  EXPECT_FALSE( gate_rep->is_ppi() );
  EXPECT_FALSE( gate_rep->is_ppo() );
  EXPECT_TRUE ( gate_rep->is_simple() );
  EXPECT_FALSE( gate_rep->is_complex() );
  EXPECT_EQ( prim_type, gate_rep->primitive_type() );
  EXPECT_THROW( gate_rep->expr(),
		std::logic_error );
  EXPECT_EQ( 0, gate_rep->extra_node_num() );
}

TEST_F(GateRepTest, AO21)
{
  SizeType tid = 1;
  SizeType ni = 3;
  auto v0 = Expr::literal(0);
  auto v1 = Expr::literal(1);
  auto v2 = Expr::literal(2);
  auto ao_expr = v0 & v1 | v2;
  auto gate_type = GateType::new_cplx(tid, ni, ao_expr);
  ASSERT_TRUE( gate_type != nullptr );
  SizeType nid = 20;
  auto fanin_list = make_fanin_list(ni);
  auto node0 = NodeRep::new_logic(nid, PrimType::And, {fanin_list[0], fanin_list[1]});
  ++ nid;
  auto node1 = NodeRep::new_logic(nid, PrimType::Or, {node0, fanin_list[2]});
  SizeType gid = 3;
  std::vector<GateRep::BranchInfo> branch_info(ni);
  branch_info[0].node = node0;
  branch_info[0].ipos = 0;
  branch_info[1].node = node0;
  branch_info[1].ipos = 1;
  branch_info[2].node = node1;
  branch_info[2].ipos = 1;
  auto gate_rep = GateRep::new_cplx(gid, gate_type, node1, branch_info,
				    FaultType::StuckAt);
  ASSERT_TRUE( gate_rep != nullptr );

  EXPECT_EQ( gid, gate_rep->id() );
  EXPECT_EQ( node1, gate_rep->output_node() );
  EXPECT_EQ( ni, gate_rep->input_num() );
  for ( SizeType i = 0; i < ni; ++ i ) {
    EXPECT_EQ( fanin_list[i], gate_rep->input_node(i) );
    auto bi = gate_rep->branch_info(i);
    EXPECT_EQ( branch_info[i].node, bi.node );
    EXPECT_EQ( branch_info[i].ipos, bi.ipos );
  }
  EXPECT_FALSE( gate_rep->is_ppi() );
  EXPECT_FALSE( gate_rep->is_ppo() );
  EXPECT_FALSE( gate_rep->is_simple() );
  EXPECT_TRUE ( gate_rep->is_complex() );
  EXPECT_THROW( gate_rep->primitive_type(),
		std::logic_error );
  EXPECT_EQ( ao_expr, gate_rep->expr() );
  EXPECT_EQ( 1, gate_rep->extra_node_num() );
}

END_NAMESPACE_DRUID
