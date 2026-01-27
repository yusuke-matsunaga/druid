
/// @file FaultRepTest.cc
/// @brief FaultRepTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include <gtest/gtest.h>
#include "FaultRep.h"
#include "GateType.h"
#include "GateRep.h"
#include "NodeRep.h"
#include "types/FaultType.h"
#include "types/Assign.h"


BEGIN_NAMESPACE_DRUID

class FaultRepTest:
  public ::testing::Test
{
public:

  // primitive タイプのゲートを作る．
  GateRep*
  new_primitive(
    PrimType prim_type,
    SizeType input_num,
    FaultType fault_type
  )
  {
    SizeType tid = 345;
    auto gate_type = GateType::new_primitive(tid, input_num, prim_type);
    SizeType nid = 0;
    auto fanin_list = make_fanin_list(input_num);
    auto node = NodeRep::new_logic(nid, prim_type, fanin_list);
    SizeType gid = 0;
    auto gate_rep = GateRep::new_primitive(gid, gate_type, node, fault_type);
    return gate_rep;
  }

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

  // AssignList の内容をチェックする．
  bool
  check_assign(
    const std::vector<SizeType>& assign_list,
    SizeType pos,
    const NodeRep* node,
    int time,
    bool val
  )
  {
    auto packed_val = assign_list[pos];
    if ( Assign::decode_node_id(packed_val) != node->id() ) {
      return false;
    }
    if ( Assign::decode_time(packed_val) != time ) {
      return false;
    }
    if ( Assign::decode_val(packed_val) != val ) {
      return false;
    }
    return true;
  }

};

TEST_F(FaultRepTest, C0_SA0)
{
  SizeType fid = 123;
  auto fault_type = FaultType::StuckAt;
  auto gate = new_primitive(PrimType::C0, 0, fault_type);
  auto fval = Fval2::zero;
  auto f0 = FaultRep::new_stem_fault(fid, gate, fval, fault_type);
  ASSERT_TRUE( f0 != nullptr );

  EXPECT_EQ( fid, f0->id() );
  EXPECT_EQ( gate, f0->gate() );
  EXPECT_EQ( fault_type, f0->fault_type() );
  EXPECT_EQ( fval, f0->fval() );
  EXPECT_TRUE( f0->is_stem() );
  EXPECT_FALSE( f0->is_branch() );
  EXPECT_THROW( f0->branch_pos(),
		std::logic_error );
  EXPECT_THROW( f0->input_vals(),
		std::logic_error );
  EXPECT_EQ( gate->output_node(), f0->origin_node() );
  auto assign_list = f0->excitation_condition();
  ASSERT_EQ( 1, assign_list.size() );
  EXPECT_TRUE( check_assign(assign_list, 0,
			    gate->output_node(), 1, true) );

}

TEST_F(FaultRepTest, C0_SA1)
{
  SizeType fid = 123;
  auto fault_type = FaultType::StuckAt;
  auto gate = new_primitive(PrimType::C0, 0, fault_type);
  auto fval = Fval2::one;
  auto f0 = FaultRep::new_stem_fault(fid, gate, fval, fault_type);
  ASSERT_TRUE( f0 != nullptr );

  EXPECT_EQ( fid, f0->id() );
  EXPECT_EQ( gate, f0->gate() );
  EXPECT_EQ( fault_type, f0->fault_type() );
  EXPECT_EQ( fval, f0->fval() );
  EXPECT_TRUE( f0->is_stem() );
  EXPECT_FALSE( f0->is_branch() );
  EXPECT_THROW( f0->branch_pos(),
		std::logic_error );
  EXPECT_THROW( f0->input_vals(),
		std::logic_error );
  EXPECT_EQ( gate->output_node(), f0->origin_node() );
  auto assign_list = f0->excitation_condition();
  ASSERT_EQ( 1, assign_list.size() );
  EXPECT_TRUE( check_assign(assign_list, 0,
			    gate->output_node(), 1, false) );

}

TEST_F(FaultRepTest, C1_SA0)
{
  SizeType fid = 123;
  auto fault_type = FaultType::StuckAt;
  auto gate = new_primitive(PrimType::C1, 0, fault_type);
  auto fval = Fval2::zero;
  auto f0 = FaultRep::new_stem_fault(fid, gate, fval, fault_type);
  ASSERT_TRUE( f0 != nullptr );

  EXPECT_EQ( fid, f0->id() );
  EXPECT_EQ( gate, f0->gate() );
  EXPECT_EQ( fault_type, f0->fault_type() );
  EXPECT_EQ( fval, f0->fval() );
  EXPECT_TRUE( f0->is_stem() );
  EXPECT_FALSE( f0->is_branch() );
  EXPECT_THROW( f0->branch_pos(),
		std::logic_error );
  EXPECT_THROW( f0->input_vals(),
		std::logic_error );
  EXPECT_EQ( gate->output_node(), f0->origin_node() );
  auto assign_list = f0->excitation_condition();
  ASSERT_EQ( 1, assign_list.size() );
  EXPECT_TRUE( check_assign(assign_list, 0,
			    gate->output_node(), 1, true) );

}

TEST_F(FaultRepTest, C1_SA1)
{
  SizeType fid = 123;
  auto fault_type = FaultType::StuckAt;
  auto gate = new_primitive(PrimType::C1, 0, fault_type);
  auto fval = Fval2::one;
  auto f0 = FaultRep::new_stem_fault(fid, gate, fval, fault_type);
  ASSERT_TRUE( f0 != nullptr );

  EXPECT_EQ( fid, f0->id() );
  EXPECT_EQ( gate, f0->gate() );
  EXPECT_EQ( fault_type, f0->fault_type() );
  EXPECT_EQ( fval, f0->fval() );
  EXPECT_TRUE( f0->is_stem() );
  EXPECT_FALSE( f0->is_branch() );
  EXPECT_THROW( f0->branch_pos(),
		std::logic_error );
  EXPECT_THROW( f0->input_vals(),
		std::logic_error );
  EXPECT_EQ( gate->output_node(), f0->origin_node() );
  auto assign_list = f0->excitation_condition();
  ASSERT_EQ( 1, assign_list.size() );
  EXPECT_TRUE( check_assign(assign_list, 0,
			    gate->output_node(), 1, false) );

}

TEST_F(FaultRepTest, C0_TD0)
{
  SizeType fid = 123;
  auto fault_type = FaultType::TransitionDelay;
  auto gate = new_primitive(PrimType::C0, 0, fault_type);
  auto fval = Fval2::zero;
  auto f0 = FaultRep::new_stem_fault(fid, gate, fval, fault_type);
  ASSERT_TRUE( f0 != nullptr );

  EXPECT_EQ( fid, f0->id() );
  EXPECT_EQ( gate, f0->gate() );
  EXPECT_EQ( fault_type, f0->fault_type() );
  EXPECT_EQ( fval, f0->fval() );
  EXPECT_TRUE( f0->is_stem() );
  EXPECT_FALSE( f0->is_branch() );
  EXPECT_THROW( f0->branch_pos(),
		std::logic_error );
  EXPECT_THROW( f0->input_vals(),
		std::logic_error );
  EXPECT_EQ( gate->output_node(), f0->origin_node() );
  auto assign_list = f0->excitation_condition();
  ASSERT_EQ( 2, assign_list.size() );
  EXPECT_TRUE( check_assign(assign_list, 0,
			    gate->output_node(), 0, false) );
  EXPECT_TRUE( check_assign(assign_list, 1,
			    gate->output_node(), 1, true) );

}

TEST_F(FaultRepTest, C0_TD1)
{
  SizeType fid = 123;
  auto fault_type = FaultType::TransitionDelay;
  auto gate = new_primitive(PrimType::C0, 0, fault_type);
  auto fval = Fval2::one;
  auto f0 = FaultRep::new_stem_fault(fid, gate, fval, fault_type);
  ASSERT_TRUE( f0 != nullptr );

  EXPECT_EQ( fid, f0->id() );
  EXPECT_EQ( gate, f0->gate() );
  EXPECT_EQ( fault_type, f0->fault_type() );
  EXPECT_EQ( fval, f0->fval() );
  EXPECT_TRUE( f0->is_stem() );
  EXPECT_FALSE( f0->is_branch() );
  EXPECT_THROW( f0->branch_pos(),
		std::logic_error );
  EXPECT_THROW( f0->input_vals(),
		std::logic_error );
  EXPECT_EQ( gate->output_node(), f0->origin_node() );
  auto assign_list = f0->excitation_condition();
  ASSERT_EQ( 2, assign_list.size() );
  EXPECT_TRUE( check_assign(assign_list, 0,
			    gate->output_node(), 0, true) );
  EXPECT_TRUE( check_assign(assign_list, 1,
			    gate->output_node(), 1, false) );

}

TEST_F(FaultRepTest, BUFF_STEM_SA0)
{
  SizeType fid = 123;
  auto fault_type = FaultType::StuckAt;
  auto gate = new_primitive(PrimType::Buff, 1, fault_type);
  auto fval = Fval2::zero;
  auto f0 = FaultRep::new_stem_fault(fid, gate, fval, fault_type);
  ASSERT_TRUE( f0 != nullptr );

  EXPECT_EQ( fid, f0->id() );
  EXPECT_EQ( gate, f0->gate() );
  EXPECT_EQ( fault_type, f0->fault_type() );
  EXPECT_EQ( fval, f0->fval() );
  EXPECT_TRUE( f0->is_stem() );
  EXPECT_FALSE( f0->is_branch() );
  EXPECT_THROW( f0->branch_pos(),
		std::logic_error );
  EXPECT_THROW( f0->input_vals(),
		std::logic_error );
  EXPECT_EQ( gate->output_node(), f0->origin_node() );
  auto assign_list = f0->excitation_condition();
  ASSERT_EQ( 1, assign_list.size() );
  EXPECT_TRUE( check_assign(assign_list, 0,
			    gate->output_node(), 1, true) );

}

TEST_F(FaultRepTest, BUFF_STEM_SA1)
{
  SizeType fid = 123;
  auto fault_type = FaultType::StuckAt;
  auto gate = new_primitive(PrimType::Buff, 1, fault_type);
  auto fval = Fval2::one;
  auto f0 = FaultRep::new_stem_fault(fid, gate, fval, fault_type);
  ASSERT_TRUE( f0 != nullptr );

  EXPECT_EQ( fid, f0->id() );
  EXPECT_EQ( gate, f0->gate() );
  EXPECT_EQ( fault_type, f0->fault_type() );
  EXPECT_EQ( fval, f0->fval() );
  EXPECT_TRUE( f0->is_stem() );
  EXPECT_FALSE( f0->is_branch() );
  EXPECT_THROW( f0->branch_pos(),
		std::logic_error );
  EXPECT_THROW( f0->input_vals(),
		std::logic_error );
  EXPECT_EQ( gate->output_node(), f0->origin_node() );
  auto assign_list = f0->excitation_condition();
  ASSERT_EQ( 1, assign_list.size() );
  EXPECT_TRUE( check_assign(assign_list, 0,
			    gate->output_node(), 1, false) );

}

TEST_F(FaultRepTest, BUFF_STEM_TD0)
{
  SizeType fid = 123;
  auto fault_type = FaultType::TransitionDelay;
  auto gate = new_primitive(PrimType::Buff, 1, fault_type);
  auto fval = Fval2::zero;
  auto f0 = FaultRep::new_stem_fault(fid, gate, fval, fault_type);
  ASSERT_TRUE( f0 != nullptr );

  EXPECT_EQ( fid, f0->id() );
  EXPECT_EQ( gate, f0->gate() );
  EXPECT_EQ( fault_type, f0->fault_type() );
  EXPECT_EQ( fval, f0->fval() );
  EXPECT_TRUE( f0->is_stem() );
  EXPECT_FALSE( f0->is_branch() );
  EXPECT_THROW( f0->branch_pos(),
		std::logic_error );
  EXPECT_THROW( f0->input_vals(),
		std::logic_error );
  EXPECT_EQ( gate->output_node(), f0->origin_node() );
  auto assign_list = f0->excitation_condition();
  ASSERT_EQ( 2, assign_list.size() );
  EXPECT_TRUE( check_assign(assign_list, 0,
			    gate->output_node(), 0, false) );
  EXPECT_TRUE( check_assign(assign_list, 1,
			    gate->output_node(), 1, true) );

}

TEST_F(FaultRepTest, BUFF_STEM_TD1)
{
  SizeType fid = 123;
  auto fault_type = FaultType::TransitionDelay;
  auto gate = new_primitive(PrimType::Buff, 1, fault_type);
  auto fval = Fval2::one;
  auto f0 = FaultRep::new_stem_fault(fid, gate, fval, fault_type);
  ASSERT_TRUE( f0 != nullptr );

  EXPECT_EQ( fid, f0->id() );
  EXPECT_EQ( gate, f0->gate() );
  EXPECT_EQ( fault_type, f0->fault_type() );
  EXPECT_EQ( fval, f0->fval() );
  EXPECT_TRUE( f0->is_stem() );
  EXPECT_FALSE( f0->is_branch() );
  EXPECT_THROW( f0->branch_pos(),
		std::logic_error );
  EXPECT_THROW( f0->input_vals(),
		std::logic_error );
  EXPECT_EQ( gate->output_node(), f0->origin_node() );
  auto assign_list = f0->excitation_condition();
  ASSERT_EQ( 2, assign_list.size() );
  EXPECT_TRUE( check_assign(assign_list, 0,
			    gate->output_node(), 0, true) );
  EXPECT_TRUE( check_assign(assign_list, 1,
			    gate->output_node(), 1, false) );

}

TEST_F(FaultRepTest, AND2_STEM_SA0)
{
  SizeType fid = 123;
  auto fault_type = FaultType::StuckAt;
  auto gate = new_primitive(PrimType::And, 2, fault_type);
  auto fval = Fval2::zero;
  auto f0 = FaultRep::new_stem_fault(fid, gate, fval, fault_type);
  ASSERT_TRUE( f0 != nullptr );

  EXPECT_EQ( fid, f0->id() );
  EXPECT_EQ( gate, f0->gate() );
  EXPECT_EQ( fault_type, f0->fault_type() );
  EXPECT_EQ( fval, f0->fval() );
  EXPECT_TRUE( f0->is_stem() );
  EXPECT_FALSE( f0->is_branch() );
  EXPECT_THROW( f0->branch_pos(),
		std::logic_error );
  EXPECT_THROW( f0->input_vals(),
		std::logic_error );
  EXPECT_EQ( gate->output_node(), f0->origin_node() );
  auto assign_list = f0->excitation_condition();
  ASSERT_EQ( 1, assign_list.size() );
  EXPECT_TRUE( check_assign(assign_list, 0,
			    gate->output_node(), 1, true) );

}

TEST_F(FaultRepTest, AND2_STEM_SA1)
{
  SizeType fid = 123;
  auto fault_type = FaultType::StuckAt;
  auto gate = new_primitive(PrimType::And, 2, fault_type);
  auto fval = Fval2::one;
  auto f0 = FaultRep::new_stem_fault(fid, gate, fval, fault_type);
  ASSERT_TRUE( f0 != nullptr );

  EXPECT_EQ( fid, f0->id() );
  EXPECT_EQ( gate, f0->gate() );
  EXPECT_EQ( fault_type, f0->fault_type() );
  EXPECT_EQ( fval, f0->fval() );
  EXPECT_TRUE( f0->is_stem() );
  EXPECT_FALSE( f0->is_branch() );
  EXPECT_THROW( f0->branch_pos(),
		std::logic_error );
  EXPECT_THROW( f0->input_vals(),
		std::logic_error );
  EXPECT_EQ( gate->output_node(), f0->origin_node() );
  auto assign_list = f0->excitation_condition();
  ASSERT_EQ( 1, assign_list.size() );
  EXPECT_TRUE( check_assign(assign_list, 0,
			    gate->output_node(), 1, false) );

}

TEST_F(FaultRepTest, BUFF_BRANCH0_SA0)
{
  SizeType fid = 123;
  auto fault_type = FaultType::StuckAt;
  auto gate = new_primitive(PrimType::Buff, 1, fault_type);
  SizeType ipos = 0;
  auto fval = Fval2::zero;
  auto f0 = FaultRep::new_branch_fault(fid, gate, ipos, fval, fault_type);
  ASSERT_TRUE( f0 != nullptr );

  EXPECT_EQ( fid, f0->id() );
  EXPECT_EQ( gate, f0->gate() );
  EXPECT_EQ( fault_type, f0->fault_type() );
  EXPECT_EQ( fval, f0->fval() );
  EXPECT_FALSE( f0->is_stem() );
  EXPECT_TRUE( f0->is_branch() );
  EXPECT_EQ( ipos, f0->branch_pos() );
  EXPECT_THROW( f0->input_vals(),
		std::logic_error );
  EXPECT_EQ( gate->output_node(), f0->origin_node() );
  auto assign_list = f0->excitation_condition();
  ASSERT_EQ( 1, assign_list.size() );
  EXPECT_TRUE( check_assign(assign_list, 0,
			    gate->input_node(ipos), 1, true) );

}

TEST_F(FaultRepTest, BUFF_BRANCH0_SA1)
{
  SizeType fid = 123;
  auto fault_type = FaultType::StuckAt;
  auto gate = new_primitive(PrimType::Buff, 1, fault_type);
  SizeType ipos = 0;
  auto fval = Fval2::one;
  auto f0 = FaultRep::new_branch_fault(fid, gate, ipos, fval, fault_type);
  ASSERT_TRUE( f0 != nullptr );

  EXPECT_EQ( fid, f0->id() );
  EXPECT_EQ( gate, f0->gate() );
  EXPECT_EQ( fault_type, f0->fault_type() );
  EXPECT_EQ( fval, f0->fval() );
  EXPECT_FALSE( f0->is_stem() );
  EXPECT_TRUE( f0->is_branch() );
  EXPECT_EQ( ipos, f0->branch_pos() );
  EXPECT_THROW( f0->input_vals(),
		std::logic_error );
  EXPECT_EQ( gate->output_node(), f0->origin_node() );
  auto assign_list = f0->excitation_condition();
  ASSERT_EQ( 1, assign_list.size() );
  EXPECT_TRUE( check_assign(assign_list, 0,
			    gate->input_node(ipos), 1, false) );

}

TEST_F(FaultRepTest, BUFF_BRANCH0_TD0)
{
  SizeType fid = 123;
  auto fault_type = FaultType::TransitionDelay;
  auto gate = new_primitive(PrimType::Buff, 1, fault_type);
  SizeType ipos = 0;
  auto fval = Fval2::zero;
  auto f0 = FaultRep::new_branch_fault(fid, gate, ipos, fval, fault_type);
  ASSERT_TRUE( f0 != nullptr );

  EXPECT_EQ( fid, f0->id() );
  EXPECT_EQ( gate, f0->gate() );
  EXPECT_EQ( fault_type, f0->fault_type() );
  EXPECT_EQ( fval, f0->fval() );
  EXPECT_FALSE( f0->is_stem() );
  EXPECT_TRUE( f0->is_branch() );
  EXPECT_EQ( ipos, f0->branch_pos() );
  EXPECT_THROW( f0->input_vals(),
		std::logic_error );
  EXPECT_EQ( gate->output_node(), f0->origin_node() );
  auto assign_list = f0->excitation_condition();
  ASSERT_EQ( 2, assign_list.size() );
  EXPECT_TRUE( check_assign(assign_list, 0,
			    gate->input_node(ipos), 0, false) );
  EXPECT_TRUE( check_assign(assign_list, 1,
			    gate->input_node(ipos), 1, true) );

}

TEST_F(FaultRepTest, BUFF_BRANCH0_TD1)
{
  SizeType fid = 123;
  auto fault_type = FaultType::TransitionDelay;
  auto gate = new_primitive(PrimType::Buff, 1, fault_type);
  SizeType ipos = 0;
  auto fval = Fval2::one;
  auto f0 = FaultRep::new_branch_fault(fid, gate, ipos, fval, fault_type);
  ASSERT_TRUE( f0 != nullptr );

  EXPECT_EQ( fid, f0->id() );
  EXPECT_EQ( gate, f0->gate() );
  EXPECT_EQ( fault_type, f0->fault_type() );
  EXPECT_EQ( fval, f0->fval() );
  EXPECT_FALSE( f0->is_stem() );
  EXPECT_TRUE( f0->is_branch() );
  EXPECT_EQ( ipos, f0->branch_pos() );
  EXPECT_THROW( f0->input_vals(),
		std::logic_error );
  EXPECT_EQ( gate->output_node(), f0->origin_node() );
  auto assign_list = f0->excitation_condition();
  ASSERT_EQ( 2, assign_list.size() );
  EXPECT_TRUE( check_assign(assign_list, 0,
			    gate->input_node(ipos), 0, true) );
  EXPECT_TRUE( check_assign(assign_list, 1,
			    gate->input_node(ipos), 1, false) );

}

TEST_F(FaultRepTest, AND2_BRANCH0_SA0)
{
  SizeType fid = 123;
  auto fault_type = FaultType::StuckAt;
  auto gate = new_primitive(PrimType::And, 2, fault_type);
  for ( SizeType ipos = 0; ipos < 2; ++ ipos ) {
    auto fval = Fval2::zero;
    auto f0 = FaultRep::new_branch_fault(fid, gate, ipos, fval, fault_type);
    ASSERT_TRUE( f0 != nullptr );

    EXPECT_EQ( fid, f0->id() );
    EXPECT_EQ( gate, f0->gate() );
    EXPECT_EQ( fault_type, f0->fault_type() );
    EXPECT_EQ( fval, f0->fval() );
    EXPECT_FALSE( f0->is_stem() );
    EXPECT_TRUE( f0->is_branch() );
    EXPECT_EQ( ipos, f0->branch_pos() );
    EXPECT_THROW( f0->input_vals(),
		  std::logic_error );
    EXPECT_EQ( gate->output_node(), f0->origin_node() );
    auto assign_list = f0->excitation_condition();
    ASSERT_EQ( 2, assign_list.size() );
    EXPECT_TRUE( check_assign(assign_list, 0,
			      gate->input_node(ipos), 1, true) );
    EXPECT_TRUE( check_assign(assign_list, 1,
			      gate->input_node(1 - ipos), 1, true) );
  }
}

TEST_F(FaultRepTest, AND2_BRANCH0_SA1)
{
  SizeType fid = 123;
  auto fault_type = FaultType::StuckAt;
  auto gate = new_primitive(PrimType::And, 2, fault_type);
  for ( SizeType ipos = 0; ipos < 2; ++ ipos ) {
    auto fval = Fval2::one;
    auto f0 = FaultRep::new_branch_fault(fid, gate, ipos, fval, fault_type);
    ASSERT_TRUE( f0 != nullptr );

    EXPECT_EQ( fid, f0->id() );
    EXPECT_EQ( gate, f0->gate() );
    EXPECT_EQ( fault_type, f0->fault_type() );
    EXPECT_EQ( fval, f0->fval() );
    EXPECT_FALSE( f0->is_stem() );
    EXPECT_TRUE( f0->is_branch() );
    EXPECT_EQ( ipos, f0->branch_pos() );
    EXPECT_THROW( f0->input_vals(),
		  std::logic_error );
    EXPECT_EQ( gate->output_node(), f0->origin_node() );
    auto assign_list = f0->excitation_condition();
    ASSERT_EQ( 2, assign_list.size() );
    EXPECT_TRUE( check_assign(assign_list, 0,
			      gate->input_node(ipos), 1, false) );
    EXPECT_TRUE( check_assign(assign_list, 1,
			      gate->input_node(1 - ipos), 1, true) );
  }
}

TEST_F(FaultRepTest, AND2_BRANCH0_TD0)
{
  SizeType fid = 123;
  auto fault_type = FaultType::TransitionDelay;
  auto gate = new_primitive(PrimType::And, 2, fault_type);
  for ( SizeType ipos = 0; ipos < 2; ++ ipos ) {
    auto fval = Fval2::zero;
    auto f0 = FaultRep::new_branch_fault(fid, gate, ipos, fval, fault_type);
    ASSERT_TRUE( f0 != nullptr );

    EXPECT_EQ( fid, f0->id() );
    EXPECT_EQ( gate, f0->gate() );
    EXPECT_EQ( fault_type, f0->fault_type() );
    EXPECT_EQ( fval, f0->fval() );
    EXPECT_FALSE( f0->is_stem() );
    EXPECT_TRUE( f0->is_branch() );
    EXPECT_EQ( ipos, f0->branch_pos() );
    EXPECT_THROW( f0->input_vals(),
		  std::logic_error );
    EXPECT_EQ( gate->output_node(), f0->origin_node() );
    auto assign_list = f0->excitation_condition();
    ASSERT_EQ( 3, assign_list.size() );
    EXPECT_TRUE( check_assign(assign_list, 0,
			      gate->input_node(ipos), 0, false) );
    EXPECT_TRUE( check_assign(assign_list, 1,
			      gate->input_node(ipos), 1, true) );
    EXPECT_TRUE( check_assign(assign_list, 2,
			      gate->input_node(1 - ipos), 1, true) );
  }

}

TEST_F(FaultRepTest, AND2_BRANCH0_TD1)
{
  SizeType fid = 123;
  auto fault_type = FaultType::TransitionDelay;
  auto gate = new_primitive(PrimType::And, 2, fault_type);
  for ( SizeType ipos = 0; ipos < 2; ++ ipos ) {
    auto fval = Fval2::one;
    auto f0 = FaultRep::new_branch_fault(fid, gate, ipos, fval, fault_type);
    ASSERT_TRUE( f0 != nullptr );

    EXPECT_EQ( fid, f0->id() );
    EXPECT_EQ( gate, f0->gate() );
    EXPECT_EQ( fault_type, f0->fault_type() );
    EXPECT_EQ( fval, f0->fval() );
    EXPECT_FALSE( f0->is_stem() );
    EXPECT_TRUE( f0->is_branch() );
    EXPECT_EQ( ipos, f0->branch_pos() );
    EXPECT_THROW( f0->input_vals(),
		  std::logic_error );
    EXPECT_EQ( gate->output_node(), f0->origin_node() );
    auto assign_list = f0->excitation_condition();
    ASSERT_EQ( 3, assign_list.size() );
    EXPECT_TRUE( check_assign(assign_list, 0,
			      gate->input_node(ipos), 0, true) );
    EXPECT_TRUE( check_assign(assign_list, 1,
			      gate->input_node(ipos), 1, false) );
    EXPECT_TRUE( check_assign(assign_list, 2,
			      gate->input_node(1 - ipos), 1, true) );
  }

}

TEST_F(FaultRepTest, OR2_BRANCH0_SA0)
{
  SizeType fid = 123;
  auto fault_type = FaultType::StuckAt;
  auto gate = new_primitive(PrimType::Or, 2, fault_type);
  for ( SizeType ipos = 0; ipos < 2; ++ ipos ) {
    auto fval = Fval2::zero;
    auto f0 = FaultRep::new_branch_fault(fid, gate, ipos, fval, fault_type);
    ASSERT_TRUE( f0 != nullptr );

    EXPECT_EQ( fid, f0->id() );
    EXPECT_EQ( gate, f0->gate() );
    EXPECT_EQ( fault_type, f0->fault_type() );
    EXPECT_EQ( fval, f0->fval() );
    EXPECT_FALSE( f0->is_stem() );
    EXPECT_TRUE( f0->is_branch() );
    EXPECT_EQ( ipos, f0->branch_pos() );
    EXPECT_THROW( f0->input_vals(),
		  std::logic_error );
    EXPECT_EQ( gate->output_node(), f0->origin_node() );
    auto assign_list = f0->excitation_condition();
    ASSERT_EQ( 2, assign_list.size() );
    EXPECT_TRUE( check_assign(assign_list, 0,
			      gate->input_node(ipos), 1, true) );
    EXPECT_TRUE( check_assign(assign_list, 1,
			      gate->input_node(1 - ipos), 1, false) );
  }

}

TEST_F(FaultRepTest, OR2_BRANCH0_SA1)
{
  SizeType fid = 123;
  auto fault_type = FaultType::StuckAt;
  auto gate = new_primitive(PrimType::Or, 2, fault_type);
  for ( SizeType ipos = 0; ipos < 2; ++ ipos ) {
    auto fval = Fval2::one;
    auto f0 = FaultRep::new_branch_fault(fid, gate, ipos, fval, fault_type);
    ASSERT_TRUE( f0 != nullptr );

    EXPECT_EQ( fid, f0->id() );
    EXPECT_EQ( gate, f0->gate() );
    EXPECT_EQ( fault_type, f0->fault_type() );
    EXPECT_EQ( fval, f0->fval() );
    EXPECT_FALSE( f0->is_stem() );
    EXPECT_TRUE( f0->is_branch() );
    EXPECT_EQ( ipos, f0->branch_pos() );
    EXPECT_THROW( f0->input_vals(),
		  std::logic_error );
    EXPECT_EQ( gate->output_node(), f0->origin_node() );
    auto assign_list = f0->excitation_condition();
    ASSERT_EQ( 2, assign_list.size() );
    EXPECT_TRUE( check_assign(assign_list, 0,
			      gate->input_node(ipos), 1, false) );
    EXPECT_TRUE( check_assign(assign_list, 1,
			      gate->input_node(1 - ipos), 1, false) );
  }

}

TEST_F(FaultRepTest, OR2_BRANCH0_TD0)
{
  SizeType fid = 123;
  auto fault_type = FaultType::TransitionDelay;
  auto gate = new_primitive(PrimType::Or, 2, fault_type);
  for ( SizeType ipos = 0; ipos < 2; ++ ipos ) {
    auto fval = Fval2::zero;
    auto f0 = FaultRep::new_branch_fault(fid, gate, ipos, fval, fault_type);
    ASSERT_TRUE( f0 != nullptr );

    EXPECT_EQ( fid, f0->id() );
    EXPECT_EQ( gate, f0->gate() );
    EXPECT_EQ( fault_type, f0->fault_type() );
    EXPECT_EQ( fval, f0->fval() );
    EXPECT_FALSE( f0->is_stem() );
    EXPECT_TRUE( f0->is_branch() );
    EXPECT_EQ( ipos, f0->branch_pos() );
    EXPECT_THROW( f0->input_vals(),
		  std::logic_error );
    EXPECT_EQ( gate->output_node(), f0->origin_node() );
    auto assign_list = f0->excitation_condition();
    ASSERT_EQ( 3, assign_list.size() );
    EXPECT_TRUE( check_assign(assign_list, 0,
			      gate->input_node(ipos), 0, false) );
    EXPECT_TRUE( check_assign(assign_list, 1,
			      gate->input_node(ipos), 1, true) );
    EXPECT_TRUE( check_assign(assign_list, 2,
			      gate->input_node(1 - ipos), 1, false) );
  }

}

TEST_F(FaultRepTest, OR2_BRANCH0_TD1)
{
  SizeType fid = 123;
  auto fault_type = FaultType::TransitionDelay;
  auto gate = new_primitive(PrimType::Or, 2, fault_type);
  for ( SizeType ipos = 0; ipos < 2; ++ ipos ) {
    auto fval = Fval2::one;
    auto f0 = FaultRep::new_branch_fault(fid, gate, ipos, fval, fault_type);
    ASSERT_TRUE( f0 != nullptr );

    EXPECT_EQ( fid, f0->id() );
    EXPECT_EQ( gate, f0->gate() );
    EXPECT_EQ( fault_type, f0->fault_type() );
    EXPECT_EQ( fval, f0->fval() );
    EXPECT_FALSE( f0->is_stem() );
    EXPECT_TRUE( f0->is_branch() );
    EXPECT_EQ( ipos, f0->branch_pos() );
    EXPECT_THROW( f0->input_vals(),
		  std::logic_error );
    EXPECT_EQ( gate->output_node(), f0->origin_node() );
    auto assign_list = f0->excitation_condition();
    ASSERT_EQ( 3, assign_list.size() );
    EXPECT_TRUE( check_assign(assign_list, 0,
			      gate->input_node(ipos), 0, true) );
    EXPECT_TRUE( check_assign(assign_list, 1,
			      gate->input_node(ipos), 1, false) );
    EXPECT_TRUE( check_assign(assign_list, 2,
			      gate->input_node(1 - ipos), 1, false) );
  }

}

TEST_F(FaultRepTest, XOR2_BRANCH0_SA0)
{
  SizeType fid = 123;
  auto fault_type = FaultType::StuckAt;
  auto gate = new_primitive(PrimType::Xor, 2, fault_type);
  for ( SizeType ipos = 0; ipos < 2; ++ ipos ) {
    auto fval = Fval2::zero;
    auto f0 = FaultRep::new_branch_fault(fid, gate, ipos, fval, fault_type);
    ASSERT_TRUE( f0 != nullptr );

    EXPECT_EQ( fid, f0->id() );
    EXPECT_EQ( gate, f0->gate() );
    EXPECT_EQ( fault_type, f0->fault_type() );
    EXPECT_EQ( fval, f0->fval() );
    EXPECT_FALSE( f0->is_stem() );
    EXPECT_TRUE( f0->is_branch() );
    EXPECT_EQ( ipos, f0->branch_pos() );
    EXPECT_THROW( f0->input_vals(),
		  std::logic_error );
    EXPECT_EQ( gate->output_node(), f0->origin_node() );
    auto assign_list = f0->excitation_condition();
    ASSERT_EQ( 1, assign_list.size() );
    EXPECT_TRUE( check_assign(assign_list, 0,
			      gate->input_node(ipos), 1, true) );
  }

}

TEST_F(FaultRepTest, XOR2_BRANCH0_SA1)
{
  SizeType fid = 123;
  auto fault_type = FaultType::StuckAt;
  auto gate = new_primitive(PrimType::Xor, 2, fault_type);
  for ( SizeType ipos = 0; ipos < 2; ++ ipos ) {
    auto fval = Fval2::one;
    auto f0 = FaultRep::new_branch_fault(fid, gate, ipos, fval, fault_type);
    ASSERT_TRUE( f0 != nullptr );

    EXPECT_EQ( fid, f0->id() );
    EXPECT_EQ( gate, f0->gate() );
    EXPECT_EQ( fault_type, f0->fault_type() );
    EXPECT_EQ( fval, f0->fval() );
    EXPECT_FALSE( f0->is_stem() );
    EXPECT_TRUE( f0->is_branch() );
    EXPECT_EQ( ipos, f0->branch_pos() );
    EXPECT_THROW( f0->input_vals(),
		  std::logic_error );
    EXPECT_EQ( gate->output_node(), f0->origin_node() );
    auto assign_list = f0->excitation_condition();
    ASSERT_EQ( 1, assign_list.size() );
    EXPECT_TRUE( check_assign(assign_list, 0,
			      gate->input_node(ipos), 1, false) );
  }

}

END_NAMESPACE_DRUID
